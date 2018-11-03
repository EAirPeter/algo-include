#pragma once

namespace {
    template<class Int>
    inline Int ISqrt(Int val) {
        Int res = 0;
        Int bit = (Int) 1 << ((sizeof(Int) << 3) - 2);
        while (bit > val)
            bit >>= 2;
        while (bit) {
            if (val >= res + bit) {
                val -= res + bit;
                res += bit << 1;
            }
            res >>= 1;
            bit >>= 2;
        }
        return res;
    }
}
