//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 26/03/2026.
//

#include "positron/renderer/opengl/gl_texture.h"
#include "positron/core/log.h"

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Positron {
    GLTexture::GLTexture(const std::string &filePath, const FilterMode filter) : path_(filePath) {
        stbi_set_flip_vertically_on_load(true);

        int            width, height, channels;
        unsigned char *pixels = stbi_load(filePath.c_str(), &width, &height, &channels, 0);

        if (!pixels) {
            POSITRON_ERROR("GLTexture: failed to load '{}' — {}", filePath, stbi_failure_reason());
            return;
        }

        width_  = static_cast<unsigned int>(width);
        height_ = static_cast<unsigned int>(height);

        GLenum internalFormat = GL_RGB8;
        GLenum dataFormat     = GL_RGB;

        if (channels == 4) {
            internalFormat = GL_RGBA8;
            dataFormat     = GL_RGBA;
        } else if (channels == 1) {
            internalFormat = GL_R8;
            dataFormat     = GL_RED;
        }

        const GLint magFilter = (filter == FilterMode::Nearest) ? GL_NEAREST : GL_LINEAR;
        const GLint minFilter = (filter == FilterMode::Nearest) ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR;

        glGenTextures(1, &id_);
        glBindTexture(GL_TEXTURE_2D, id_);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, pixels);
        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(pixels);

        POSITRON_INFO("GLTexture: loaded '{}' ({}x{}, {} ch, {})", filePath, width_, height_, channels,
                      filter == FilterMode::Nearest ? "nearest" : "linear");
    }

    GLTexture::~GLTexture() {
        if (id_)
            glDeleteTextures(1, &id_);
    }

    void GLTexture::bind(const unsigned int slot) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, id_);
    }

    void GLTexture::unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }

    Texture *Texture::load(const std::string &filePath, const FilterMode filter) {
        return new GLTexture(filePath, filter);
    }
} // namespace Positron
