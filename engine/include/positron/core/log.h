//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 25/03/2026.
//

#pragma once

#include <memory>
#include <spdlog/spdlog.h>

namespace Positron {
    class Log {
    public:
        static void init();

        static std::shared_ptr<spdlog::logger> &getLogger() { return logger_; }

    private:
        static std::shared_ptr<spdlog::logger> logger_;
    };
} // namespace Positron

#define POSITRON_TRACE(...) ::Positron::Log::getLogger()->trace(__VA_ARGS__)
#define POSITRON_DEBUG(...) ::Positron::Log::getLogger()->debug(__VA_ARGS__)
#define POSITRON_INFO(...) ::Positron::Log::getLogger()->info(__VA_ARGS__)
#define POSITRON_WARN(...) ::Positron::Log::getLogger()->warn(__VA_ARGS__)
#define POSITRON_ERROR(...) ::Positron::Log::getLogger()->error(__VA_ARGS__)
