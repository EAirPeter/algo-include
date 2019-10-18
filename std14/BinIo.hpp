// Binary Input & Output
// Require: IBufLen and OBufLen defined
// Binary Input & Output
// IBufLen and OBufLen should be defined
// Only one translation unit may include this file
#pragma once

#include "Common.hpp"

#include <cctype>
#include <cstdio>

namespace ImplBinIo {
  char IBuf[IBufLen];
  char OBuf[OBufLen];
  const char* IPtr = IBuf;
  char* OPtr = OBuf;

  struct Init {
    Init() { ::std::fread(IBuf, 1, IBufLen, stdin); }
    ~Init() { ::std::fwrite(OBuf, 1, (Uz) (OPtr - OBuf), stdout); }
  } TheInit;

  inline void XSkip() { while (!::std::isgraph(*IPtr)) ++IPtr; }

  template<class Int>
  inline void XGetDig(Int& I) {
    I = (Int) (*IPtr++ & 0x0f);
    while (::std::isdigit(*IPtr))
      I = I * 10 + (Int) (*IPtr++ & 0x0f);
  }

  template<class UInt>
  inline void GetU(UInt& I) {
    XSkip();
    XGetDig(I);
  }

  template<class SInt>
  inline void GetI(SInt& I) {
    XSkip();
    if (*IPtr != '-')
      XGetDig(I);
    else {
      ++IPtr;
      XGetDig(I);
      I = -I;
    }
  }

  inline void GetC(char& C) {
    XSkip();
    C = *IPtr++;
  }

  inline void GetS(char* S) {
    XSkip();
    while (::std::isgraph(*IPtr))
      *S++ = *IPtr++;
    *S = '\0';
  }

  template<class Int>
  inline void XPutDig(Int I) {
    int cnt = 0;
    while (I) {
      OPtr[cnt++] = (char) ((I % 10) | '0');
      I /= 10;
    }
    ::std::reverse(OPtr, OPtr + cnt);
    OPtr += cnt;
  }

  template<class UInt>
  inline void PutU(UInt I) {
    if (I)
      XPutDig(I);
    else
      *OPtr++ = '0';
  }

  template<class SInt>
  inline void PutI(SInt I) {
    if (I < 0) {
      *OPtr++ = '-';
      XPutDig(-I);
    }
    else if (I > 0)
      XPutDig(I);
    else
      *OPtr++ = '0';
  }

  inline void PutC(char C) { *OPtr++ = C; }
  inline void PutS(const char* S) { while (*S) *OPtr++ = *S++; }

  inline void PutS(const char* S, Uz N) {
    ::std::copy(S, S + N, OPtr);
    OPtr += N;
  }

  template<Uz N>
  inline void PutS(const char (&S)[N]) {
    ::std::copy(S, S + N - 1, OPtr);
    OPtr += N - 1;
  }

  struct Get {
    Get operator ,(signed   int      & I) const { GetI(I); return {}; }
    Get operator ,(signed   long     & I) const { GetI(I); return {}; }
    Get operator ,(signed   long long& I) const { GetI(I); return {}; }
    Get operator ,(unsigned int      & I) const { GetU(I); return {}; }
    Get operator ,(unsigned long     & I) const { GetU(I); return {}; }
    Get operator ,(unsigned long long& I) const { GetU(I); return {}; }
    Get operator ,(char& C) const { GetC(C); return {}; }
    Get operator ,(char* S) const { GetS(S); return {}; }
  };

  struct Put {
    Put operator ,(signed   int       I) const { PutI(I); return {}; }
    Put operator ,(signed   long      I) const { PutI(I); return {}; }
    Put operator ,(signed   long long I) const { PutI(I); return {}; }
    Put operator ,(unsigned int       I) const { PutU(I); return {}; }
    Put operator ,(unsigned long      I) const { PutU(I); return {}; }
    Put operator ,(unsigned long long I) const { PutU(I); return {}; }
    Put operator ,(char C) const { PutC(C); return {}; }
    template<Uz N>
    Put operator ,(const char (&S)[N]) const { PutS<N>(S); return {}; }
    Put operator ,(const char* S) const { PutS(S); return {}; }
  };
} // namespace ImplBinIo

#define IoGet(...) ((void) (::ImplBinIo::Get{}, __VA_ARGS__))
#define IoPut(...) ((void) (::ImplBinIo::Put{}, __VA_ARGS__))
