// Standard Input & Output
// Only one translation unit may include this file
#pragma once

#include "Common.hpp"

#include <iostream>

namespace StdIo {
  struct Init {
    Init() {
      ::std::ios_base::sync_with_stdio(false);
      ::std::cin.tie(nullptr);
      ::std::cout.tie(nullptr);
    }
  } TheInit;

  struct Get {
    Get operator ,(signed   int      & I) const { ::std::cin >> I; return {}; }
    Get operator ,(signed   long     & I) const { ::std::cin >> I; return {}; }
    Get operator ,(signed   long long& I) const { ::std::cin >> I; return {}; }
    Get operator ,(unsigned int      & I) const { ::std::cin >> I; return {}; }
    Get operator ,(unsigned long     & I) const { ::std::cin >> I; return {}; }
    Get operator ,(unsigned long long& I) const { ::std::cin >> I; return {}; }
    Get operator ,(char& C) const { ::std::cin >> C; return {}; }
    Get operator ,(char* S) const { ::std::cin >> S; return {}; }
  };

  struct Put {
    Put operator ,(signed   int       I) const { ::std::cout << I; return {}; }
    Put operator ,(signed   long      I) const { ::std::cout << I; return {}; }
    Put operator ,(signed   long long I) const { ::std::cout << I; return {}; }
    Put operator ,(unsigned int       I) const { ::std::cout << I; return {}; }
    Put operator ,(unsigned long      I) const { ::std::cout << I; return {}; }
    Put operator ,(unsigned long long I) const { ::std::cout << I; return {}; }
    Put operator ,(char C) const { ::std::cout << C; return {}; }
    template<Uz N>
    Put operator ,(const char (&S)[N]) const { ::std::cout << S; return {}; }
    Put operator ,(const char* S) const { ::std::cout << S; return {}; }
  };
} // namespace StdIo

#define IoGet(...) ((void) (::StdIo::Get{}, __VA_ARGS__))
#define IoPut(...) ((void) (::StdIo::Put{}, __VA_ARGS__))
