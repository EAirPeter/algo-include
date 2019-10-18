#pragma once

#include <cstddef>

namespace {
    typedef ::std::ptrdiff_t    Isz;
    typedef ::std::size_t       Usz;
}

#if __cplusplus >= 201402
#   define CVMF constexpr
#else // __cplusplus < 201402
#   define CVMF inline
#endif

#if __cplusplus >= 201103
#   define CEXF constexpr
#   define CEXV constexpr
#   define NOEX noexcept
#   include <cstdint>
namespace {
    typedef ::std::int8_t       I8;
    typedef ::std::int16_t      I16;
    typedef ::std::int32_t      I32;
    typedef ::std::int64_t      I64;
    typedef ::std::intmax_t     Imx;
    typedef ::std::uint8_t      U8;
    typedef ::std::uint16_t     U16;
    typedef ::std::uint32_t     U32;
    typedef ::std::uint64_t     U64;
    typedef ::std::uintmax_t    Umx;
}
#else // __cplusplus < 201103
#   define CEXF inline
#   define CEXV const
#   define NOEX throw()
#   define nullptr 0
namespace {
    typedef signed   char       I8;
    typedef signed   short      I16;
    typedef signed   int        I32;
    typedef signed   long long  I64;
    typedef signed   long long  Imx;
    typedef unsigned char       U8;
    typedef unsigned short      U16;
    typedef unsigned int        U32;
    typedef unsigned long long  U64;
    typedef unsigned long long  Umx;
}
#endif // __cplusplus
