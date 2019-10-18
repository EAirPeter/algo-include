#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <random>

using namespace std;

constexpr uint64_t kMod = 9223372036854775807;

constexpr uint64_t Slow(uint64_t a, uint64_t b, uint64_t c) noexcept {
  uint64_t res = 0;
  while (b) {
    if (b & 1)
      res = (res + a) % c;
    a = (a + a) % c;
    b >>= 1;
  }
  return res;
}

constexpr uint64_t Fast(uint64_t a, uint64_t b, uint64_t c) noexcept {
  auto res = a * b - (uint64_t) ((long double) a * b / c) * c;
  return min(res, min(res - c, res + c));
}

inline uint64_t MottoFast(uint64_t a, uint64_t b, uint64_t c) noexcept {
  uint64_t quo, rem;
  asm (
    "mulq %3\n\t"
    "divq %4"
    : "=a"(quo), "=&d"(rem)
    : "%a"(a), "rm"(b), "rm"(c)
    : "cc"
  );
  return rem;
}

constexpr uint64_t WtfClang(uint64_t a, uint64_t b, uint64_t c) noexcept {
  return (uint64_t) ((unsigned __int128) a * b % c);
}

int main() {
  mt19937_64 rng((uint64_t) chrono::system_clock::now()
    .time_since_epoch().count());
  for (;;) {
    // try to calculate a * b % kMod
    auto a = rng() % kMod;
    auto b = rng() % kMod;
    auto s = Slow(a, b, kMod);
    auto f = Fast(a, b, kMod);
    auto m = MottoFast(a, b, kMod);
    auto w = WtfClang(a, b, kMod);
    if (s != f || s != m || s != w)
      cout << "fuck: " << s << ' ' << f << ' ' << m << ' ' << w << endl;
  }
  return 0;
}
