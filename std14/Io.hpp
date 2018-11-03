// Input & Output
#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdio>

namespace Io {
    char IBuf[IBufLen];
    char OBuf[OBufLen];
    const char* IPtr = IBuf;
    char* OPtr = OBuf;
    struct ImplInit {
        inline ImplInit() {
            ::std::fread(IBuf, 1, IBufLen, stdin);
        }
        inline ~ImplInit() {
            ::std::fwrite(OBuf, 1, (size_t) (OPtr - OBuf), stdout);
        }
    } Init;
    template<char ch>
    inline void XNext() noexcept {
        while (*IPtr < ch)
            ++IPtr;
    }
    template<class Int>
    inline Int XGetDig() noexcept {
        Int res = (Int) (*IPtr++ & 0x0f);
        while (*IPtr >= '0')
            res = res * 10 + (Int) (*IPtr++ & 0x0f);
        return res;
    }
    template<class Int>
    inline Int GetU() noexcept {
        XNext<'0'>();
        return XGetDig<Int>();
    }
    template<class Int>
    inline Int GetI() noexcept {
        XNext<'-'>();
        return *IPtr == '-' ? ++IPtr, -XGetDig<Int>() : XGetDig<Int>();
    }
    template<class Int>
    inline void XPutDig(Int val) noexcept {
        auto cnt = 0;
        while (val) {
            OPtr[cnt++] = (char) ((val % 10) | '0');
            val /= 10;
        }
        ::std::reverse(OPtr, OPtr + cnt);
        OPtr += cnt;
    }
    template<class Int>
    inline void PutU(Int val) noexcept {
        if (val)
            XPutDig(val);
        else
            *OPtr++ = '0';
    }
    template<class Int>
    inline void PutI(Int val) noexcept {
        if (val < 0) {
            *OPtr++ = '-';
            XPutDig(-val);
        }
        else if (val > 0)
            XPutDig(val);
        else
            *OPtr++ = '0';
    }
    inline void Eol() noexcept {
        *OPtr++ = '\n';
    }
    inline void PutC(char ch) noexcept {
        *OPtr++ = ch;
    }
    inline void PutS(const char* str) noexcept {
        while (*str)
            *OPtr++ = *str;
    }
    inline void PutS(const char* str, ::std::size_t len) noexcept {
        ::std::copy(str, str + len, OPtr);
        OPtr += len;
    }
    template<::std::size_t len>
    inline void PutS(const char (&str)[len]) noexcept {
        ::std::copy(str, str + len - 1, OPtr);
        OPtr += len - 1;
    }
    template<class F, class Elem>
    inline void PutList(F&& f, Elem* A, ::std::size_t N) noexcept {
        f(A[0]);
        for (::std::size_t i = 1; i < N; ++i) {
            PutC(' ');
            f(A[i]);
        }
    }
}
