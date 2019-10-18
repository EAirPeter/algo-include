// Common Definition
#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <type_traits>
#include <utility>

namespace {

using I8    = ::std::int8_t;
using I16   = ::std::int16_t;
using I32   = ::std::int32_t;
using I64   = ::std::int64_t;
using Iz    = ::std::ptrdiff_t;
using Iptr  = ::std::intptr_t;
using Imax  = ::std::intmax_t;

using U8    = ::std::uint8_t;
using U16   = ::std::uint16_t;
using U32   = ::std::uint32_t;
using U64   = ::std::uint64_t;
using Uz    = ::std::size_t;
using Uptr  = ::std::uintptr_t;
using Umax  = ::std::uintmax_t;

}
