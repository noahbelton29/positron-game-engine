//
// Created by noahbelton on 25/03/2026.
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
