#define NTT_USE_AVX2
#include "Ntt.hpp"

#include <cstdint>
#include <cstring>
#include <iostream>
#include <limits>
#include <random>

using namespace std;

constexpr int LogLen = 14;
constexpr size_t Len = size_t{1} << LogLen;
constexpr size_t Len2 = Len >> 1;
constexpr uint32_t Mod = 998244353;
constexpr uint32_t G = 3;

uint32_t A[Len]{};
uint32_t B[Len]{};
uint32_t C[Len]{};

template<class Int>
constexpr Int ISqrt(Int val) {
  Int res = 0;
  Int bit = Int{1} << (numeric_limits<Int>::digits - 2);
  while (bit > val)
    bit >>= 2;
  while (bit) {
    if (val >= res + bit) {
      val -= res + bit;
      res += bit << 1;
    }
    res >>= 1;
    bit >>= 2;
  }
  return res;
}

template<size_t N>
void Print(const uint32_t* A, const char* S = "") {
  if constexpr (N < 40) {
    cout << S << ":";
    for (uint32_t i = 0; i < N; ++i)
      cout << ' ' << A[i];
    cout << endl;
  }
}

int main() {
  mt19937 MT{random_device{}()};
  uniform_int_distribution<uint32_t> U{0, ISqrt((uint64_t) Mod * Mod / Len)};
  using Ntt = ntt::Ntt<Mod, G>;
  for (uint32_t i = 0; i < Len2; ++i)
    A[i] = U(MT);
  Print<Len2>(A, "A");
  for (uint32_t i = 0; i < Len2; ++i)
    B[i] = U(MT);
  Print<Len2>(B, "B");
  for (uint32_t i = 0; i < Len2; ++i)
    for (uint32_t j = 0; j < Len2; ++j)
      C[i + j] = Ntt::Add(C[i + j], Ntt::Mul(A[i], B[j]));
  Print<Len>(C, "AB");
  Ntt::FwdDif<LogLen>(A);
  Print<Len>(A, "N(A)");
  Ntt::FwdDif<LogLen>(B);
  Print<Len>(B, "N(B)");
  Ntt::VMul<LogLen>(A, B);
  Print<Len>(A, "N(A)N(B)");
  Ntt::InvDit<LogLen>(A);
  Print<Len>(A, "IN(N(A)N(B))");
  if (memcmp(A, C, sizeof(uint32_t) * Len))
    cout << "Fuck" << endl;
  else
    cout << "Done" << endl;
  return 0;
}
