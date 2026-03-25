//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 25/03/2026.
//

#pragma once

#include "positron/core/export.h"

namespace Positron {
    class POSITRON_API Mesh {
    public:
        virtual ~Mesh()                    = default;
        virtual void bind() const          = 0;
        virtual void unbind() const        = 0;
        virtual int  getIndexCount() const = 0;
    };
} // namespace Positron
