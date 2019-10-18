// GCC builtin wrapper
#pragma once

namespace BI {

constexpr int Clz(unsigned int       I) noexcept { return __builtin_clz(I); }
constexpr int Clz(unsigned long      I) noexcept { return __builtin_clzl(I); }
constexpr int Clz(unsigned long long I) noexcept { return __builtin_clzll(I); }

}

} // namespace BI
