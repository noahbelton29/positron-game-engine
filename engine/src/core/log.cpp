//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 25/03/2026.
//

#include "positron/core/log.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <backward.hpp>
#include <csignal>
#include <cstdlib>
#include <sstream>

namespace Positron {
    std::shared_ptr<spdlog::logger> Log::logger_;

    static void signalHandler(int signal) {
        const char *name;
        switch (signal) {
            case SIGSEGV:
                name = "Segmentation fault (SIGSEGV)";
                break;
            case SIGABRT:
                name = "Abort signal (SIGABRT)";
                break;
            case SIGILL:
                name = "Illegal instruction (SIGILL)";
                break;
            case SIGFPE:
                name = "Floating point exception (SIGFPE)";
                break;
            default:
                name = "Unknown signal";
                break;
        }

        POSITRON_ERROR("Fatal signal: {}", name);

        backward::StackTrace st;
        st.load_here(32);
        st.skip_n_firsts(3);

        backward::TraceResolver resolver;
        resolver.load_stacktrace(st);

        std::ostringstream oss;
        oss << "Stack trace:\n";

        for (size_t i = st.size(); i > 0; --i) {
            backward::ResolvedTrace trace = resolver.resolve(st[i - 1]);

            auto contains = [](const std::string &str, const std::string &sub) {
                return str.find(sub) != std::string::npos;
            };

            if (trace.source.filename.empty())
                continue;

            // Linux system paths
            if (contains(trace.source.filename, "/usr/"))
                continue;
            if (contains(trace.source.filename, "libc-start"))
                continue;
            if (contains(trace.source.filename, "libc_start"))
                continue;

            // Windows system paths
            if (contains(trace.source.filename, "\\Windows\\"))
                continue;
            if (contains(trace.source.filename, "\\MSVC\\"))
                continue;
            if (contains(trace.source.filename, "ucrt"))
                continue;
            if (contains(trace.source.filename, "vcruntime"))
                continue;

            std::string file = trace.source.filename;
            if (size_t slash = file.find_last_of('/'); slash != std::string::npos)
                file = file.substr(slash + 1);

            if (std::ifstream src(trace.source.filename); src.is_open()) {
                std::vector<std::string> lines;
                std::string              line;
                while (std::getline(src, line))
                    lines.push_back(line);

                size_t crashLine = trace.source.line;
                size_t start     = crashLine >= 3 ? crashLine - 3 : 0;
                size_t end       = std::min(crashLine + 1, lines.size());

                oss << "  " << file << ":" << crashLine << " in " << trace.source.function << "\n";

                for (size_t l = start; l < end; ++l) {
                    std::string trimmed = lines[l];
                    if (size_t ws = trimmed.find_first_not_of(" \t"); ws != std::string::npos)
                        trimmed = trimmed.substr(ws);

                    if (l + 1 == crashLine)
                        oss << "  > " << (l + 1) << ": " << trimmed << "\n";
                    else
                        oss << "    " << (l + 1) << ": " << trimmed << "\n";
                }
            }
        }

        POSITRON_ERROR("{}", oss.str());
        Log::getLogger()->flush();
        std::exit(signal);
    }

    void Log::init() {
        auto console = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        auto file    = std::make_shared<spdlog::sinks::basic_file_sink_mt>("positron.log", true);

        console->set_pattern("%^[%d-%m-%Y %T] %v%$");
        file->set_pattern("[%d-%m-%Y %T] [%l] %v");

        logger_ = std::make_shared<spdlog::logger>("Positron", spdlog::sinks_init_list{console, file});
        logger_->set_level(spdlog::level::trace);
        logger_->flush_on(spdlog::level::warn);

        spdlog::register_logger(logger_);

        std::signal(SIGSEGV, signalHandler);
        std::signal(SIGABRT, signalHandler);
        std::signal(SIGILL, signalHandler);
        std::signal(SIGFPE, signalHandler);
    }
} // namespace Positron
