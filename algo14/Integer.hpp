// Integer Stuff
#pragma once

#include <type_traits>

template<class Int>
struct IntTraits {
  using SInt = ::std::make_signed_t<Int>;
  using UInt = ::std::make_unsigned_t<Int>;
};

template<class Int> using MakeSInt = IntTraits<Int>::typename SInt;
template<class Int> using MakeUInt = IntTraits<Int>::typename UInt;
