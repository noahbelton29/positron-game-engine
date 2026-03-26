//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 26/03/2026.
//

#pragma once

#include "positron/core/export.h"

#include <string>

namespace Positron {
    enum class FilterMode {
        Linear,
        Nearest,
    };

    class POSITRON_API Texture {
    public:
        virtual ~Texture() = default;

        virtual void bind(unsigned int slot = 0) const = 0;
        virtual void unbind() const                    = 0;

        [[nodiscard]] virtual unsigned int width() const  = 0;
        [[nodiscard]] virtual unsigned int height() const = 0;
        [[nodiscard]] virtual unsigned int id() const     = 0;

        [[nodiscard]] virtual const std::string &path() const = 0;

        static Texture *load(const std::string &filePath, FilterMode filter = FilterMode::Linear);
    };
} // namespace Positron
