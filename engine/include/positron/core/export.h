//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 25/03/2026.
//

#pragma once

#ifdef _WIN32
#ifdef POSITRON_BUILD_DLL
#define POSITRON_API __declspec(dllexport)
#else
#define POSITRON_API __declspec(dllimport)
#endif
#else
#define POSITRON_API __attribute__((visibility("default")))
#endif
