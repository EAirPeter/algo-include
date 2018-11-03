#pragma once

#include "Common.hpp"

#include <algorithm>
#include <cctype>
#include <cstdio>

namespace { namespace BinIo {
    char IBuf[IBufLen];
    char OBuf[OBufLen];
    const char* IPtr = IBuf;
    char* OPtr = OBuf;
    struct IoInit {
        inline IoInit() {
            ::std::fread(IBuf, 1, IBufLen, stdin);
        }
        inline ~IoInit() {
            ::std::fwrite(OBuf, 1, (Usz) (OPtr - OBuf), stdout);
        }
    } TheIoInit;
    inline void XSkip() {
        while (!::std::isgraph(*IPtr))
            ++IPtr;
    }
    template<class Int>
    inline void XGetDig(Int& val) {
        val = (Int) (*IPtr++ & 0x0f);
        while (::std::isdigit(*IPtr))
            val = val * 10 + (Int) (*IPtr++ & 0x0f);
    }
    template<class Int>
    inline void GetU(Int& val) {
        XSkip();
        XGetDig(val);
    }
    template<class Int>
    inline void GetI(Int& val) {
        XSkip();
        if (*IPtr != '-')
            XGetDig(val);
        else {
            ++IPtr;
            XGetDig(val);
            val = -val;
        }
    }
    inline void GetC(char& ch) {
        XSkip();
        ch = *IPtr++;
    }
    inline void GetS(char* str) {
        XSkip();
        while (::std::isgraph(*IPtr))
            *str++ = *IPtr++;
        *str = '\0';
    }
    template<class Int>
    inline void XPutDig(Int val) {
        int cnt = 0;
        while (val) {
            OPtr[cnt++] = (char) ((val % 10) | '0');
            val /= 10;
        }
        ::std::reverse(OPtr, OPtr + cnt);
        OPtr += cnt;
    }
    template<class Int>
    inline void PutU(Int val) {
        if (val)
            XPutDig(val);
        else
            *OPtr++ = '0';
    }
    template<class Int>
    inline void PutI(Int val) {
        if (val < 0) {
            *OPtr++ = '-';
            XPutDig(-val);
        }
        else if (val > 0)
            XPutDig(val);
        else
            *OPtr++ = '0';
    }
    inline void PutC(char ch) {
        *OPtr++ = ch;
    }
    inline void PutS(const char* str) {
        while (*str)
            *OPtr++ = *str++;
    }
    inline void PutS(const char* str, Usz len) {
        ::std::copy(str, str + len, OPtr);
        OPtr += len;
    }
    template<Usz len>
    inline void PutS(const char (&str)[len]) {
        ::std::copy(str, str + len - 1, OPtr);
        OPtr += len - 1;
    }
    struct Get {
        inline Get operator ,(signed   int      & val) const { GetI(val); return Get(); }
        inline Get operator ,(signed   long     & val) const { GetI(val); return Get(); }
        inline Get operator ,(signed   long long& val) const { GetI(val); return Get(); }
        inline Get operator ,(unsigned int      & val) const { GetU(val); return Get(); }
        inline Get operator ,(unsigned long     & val) const { GetU(val); return Get(); }
        inline Get operator ,(unsigned long long& val) const { GetU(val); return Get(); }
        inline Get operator ,(char& ch) const { GetC(ch); return Get(); }
        inline Get operator ,(char* str) const { GetS(str); return Get(); }
    };
    struct Put {
        inline Put operator ,(signed   int       val) const { PutI(val); return Put(); }
        inline Put operator ,(signed   long      val) const { PutI(val); return Put(); }
        inline Put operator ,(signed   long long val) const { PutI(val); return Put(); }
        inline Put operator ,(unsigned int       val) const { PutU(val); return Put(); }
        inline Put operator ,(unsigned long      val) const { PutU(val); return Put(); }
        inline Put operator ,(unsigned long long val) const { PutU(val); return Put(); }
        inline Put operator ,(char ch) const { PutC(ch); return Put(); }
        template<Usz len>
        inline Put operator ,(const char (&str)[len]) const { PutS<len>(str); return Put(); }
        inline Put operator ,(const char* str) const { PutS(str); return Put(); }
    };
}}

#define IoGet(...) ((void) (::BinIo::Get(), __VA_ARGS__))
#define IoPut(...) ((void) (::BinIo::Put(), __VA_ARGS__))
