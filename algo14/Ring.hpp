// Ring of Integer Modulo mod
#pragma once

#include <algorithm>

template<class UInt_, UInt_ kMod_, class Traits_ = IntTraits<UInt_>>
struct NormTraits {
  using UInt = UInt_;
  using SInt = Traits_::typename SInt;
  static constexpr kMod = kMod_;
  template<class Ty>
  static constexpr UInt NormU(Ty&& val) noexcept {
    return (UInt) (val % kMod);
  }
  template<class Ty>
  static constexpr UInt NormS(Ty&& val) noexcept {
    auto res = val % kMod;
    return val < 0 ? (UInt) res + kMod : (UInt) res;
  }
};

template<class UInt, UInt kMod, class Tmp = UInt>
struct MulTrivial {
  static constexpr UInt Mul(const UInt& lhs, const UInt& rhs) noexcept {
    return (UInt) ((Tmp) lhs * (Tmp) rhs % (Tmp) kMod);
  }
};

template<class UInt, UInt kMod>
struct MulLongDbl {
  static constexpr UInt Mul(const UInt& lhs, const UInt& rhs) noexcept {
    auto res = lhs * rhs - (UInt) ((long double) lhs * rhs / kMod) * kMod;
    return min(min(res, res - kMod), res + kMod);
  }
};

template<
  class UInt_, class UInt_ kMod_,
  class MulHelper = MulTriv<UInt_, kMod_>
>
class Ring {
public:
  using UInt = UInt_;
  static constexpr UInt kMod = kMod_;
private:
  UInt val;
};
