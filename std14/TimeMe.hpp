// Time this program
#pragma once

#ifdef VIOLOCAL

#include <chrono>
#include <cstdio>

namespace { namespace TimeMe {
    using namespace ::std::chrono;
    struct TimeMe {
        high_resolution_clock::time_point tp;
        inline TimeMe() : tp(high_resolution_clock::now()) {}
        inline ~TimeMe() {
            auto dur = duration_cast<microseconds>(high_resolution_clock::now() - tp);
            ::std::fprintf(stderr, "%.6f s\n", (double) dur.count() / 1e6);
        }
    } TheTimeMe;
}}

#endif
