// Time this program
#pragma once

#ifdef VIOLOCAL

#include <windows.h>

#include <cstdio>

namespace { namespace TimeMe {
    struct TimeMe {
        ::LARGE_INTEGER qpf;
        ::LARGE_INTEGER qpc;
        inline TimeMe() {
            ::QueryPerformanceFrequency(&qpf);
            ::QueryPerformanceCounter(&qpc);
        }
        inline ~TimeMe() {
            ::LARGE_INTEGER now;
            ::QueryPerformanceCounter(&now);
            ::std::fprintf(stderr, "%.6f s\n", (double) (now.QuadPart - qpc.QuadPart) / (double) qpf.QuadPart);
        }
    } TheTimeMe;
}}

#endif
