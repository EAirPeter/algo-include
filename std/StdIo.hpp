#pragma once

#include "Common.hpp"

#include <iostream>

namespace { namespace StdIo {
    struct IoInit {
        inline IoInit() {
            ::std::ios_base::sync_with_stdio(false);
            ::std::cin.tie(nullptr);
            ::std::cout.tie(nullptr);
        }
    } TheIoInit;
    struct Get {
        inline Get operator ,(signed   int      & val) const { ::std::cin >> val; return Get(); }
        inline Get operator ,(signed   long     & val) const { ::std::cin >> val; return Get(); }
        inline Get operator ,(signed   long long& val) const { ::std::cin >> val; return Get(); }
        inline Get operator ,(unsigned int      & val) const { ::std::cin >> val; return Get(); }
        inline Get operator ,(unsigned long     & val) const { ::std::cin >> val; return Get(); }
        inline Get operator ,(unsigned long long& val) const { ::std::cin >> val; return Get(); }
        inline Get operator ,(char& ch) const { ::std::cin >> ch; return Get(); }
        inline Get operator ,(char* str) const { ::std::cin >> str; return Get(); }
    };
    struct Put {
        inline Put operator ,(signed   int       val) const { ::std::cout << val; return Put(); }
        inline Put operator ,(signed   long      val) const { ::std::cout << val; return Put(); }
        inline Put operator ,(signed   long long val) const { ::std::cout << val; return Put(); }
        inline Put operator ,(unsigned int       val) const { ::std::cout << val; return Put(); }
        inline Put operator ,(unsigned long      val) const { ::std::cout << val; return Put(); }
        inline Put operator ,(unsigned long long val) const { ::std::cout << val; return Put(); }
        inline Put operator ,(char ch) const { ::std::cout << ch; return Put(); }
        template<Usz len>
        inline Put operator ,(const char (&str)[len]) const { ::std::cout << str; return Put(); }
        inline Put operator ,(const char* str) const { ::std::cout << str; return Put(); }
    };
}}

#define IoGet(...) ((void) (::StdIo::Get(), __VA_ARGS__))
#define IoPut(...) ((void) (::StdIo::Put(), __VA_ARGS__))
