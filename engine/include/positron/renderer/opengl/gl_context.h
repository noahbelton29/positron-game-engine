//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 25/03/2026.
//

#pragma once

#include "positron/core/export.h"

#include <string>

namespace Positron {
    class POSITRON_API GLContext {
    public:
        static bool init();
        static void shutdown();

        [[nodiscard]] static std::string getRenderer();
        [[nodiscard]] static std::string getVendor();
        [[nodiscard]] static std::string getVersion();
        [[nodiscard]] static std::string getGLSLVersion();
    };
} // namespace Positron
