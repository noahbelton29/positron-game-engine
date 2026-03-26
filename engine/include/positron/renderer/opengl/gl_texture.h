//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 26/03/2026.
//

#pragma once

#include "positron/core/export.h"
#include "positron/renderer/texture.h"

#include <string>

namespace Positron {

    class POSITRON_API GLTexture : public Texture {
    public:
        explicit GLTexture(const std::string &filePath, FilterMode filter = FilterMode::Linear);
        ~GLTexture() override;

        GLTexture(const GLTexture &)            = delete;
        GLTexture &operator=(const GLTexture &) = delete;

        void bind(unsigned int slot = 0) const override;
        void unbind() const override;

        [[nodiscard]] unsigned int width() const override { return width_; }
        [[nodiscard]] unsigned int height() const override { return height_; }
        [[nodiscard]] unsigned int id() const override { return id_; }

        [[nodiscard]] const std::string &path() const override { return path_; }

    private:
        unsigned int id_     = 0;
        unsigned int width_  = 0;
        unsigned int height_ = 0;
        std::string  path_;
    };

} // namespace Positron
