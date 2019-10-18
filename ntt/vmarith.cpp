#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <random>

#include <immintrin.h>

using namespace std;

#ifdef _MSC_VER
#define restrict_ __restrict
#define MFence(...) _ReadWriteBarrier()
#else
#define restrict_ __restrict__
#define MFence(...) __sync_synchronize()
#endif

constexpr int MGetF(uint32_t Mod) {
  int F = 0;
  while (F < 32 && (1u << (F + 1)) <= Mod)
    ++F;
  return F;
}

constexpr int MGetG(uint32_t Mod, int F) {
  auto M2 = (uint64_t) (Mod - 1) * (Mod - 1);
  int G = 0;
  while (F + G < 64 && (1ull << (F + G)) < M2)
    ++G;
  return G;
}

constexpr uint32_t MGetH(uint32_t Mod, int F, int G) {
  return (uint32_t) ((uint64_t{1} << (F + G)) / Mod);
}

constexpr uint32_t Mod = 2147483647;
constexpr int F = MGetF(Mod);
constexpr int G = MGetG(Mod, F);
constexpr uint32_t H = MGetH(Mod, F, G);

constexpr uint32_t Min(uint32_t A, uint32_t B, uint32_t C) {
    return A < B ? (A < C ? A : C) : (B < C ? B : C);
}

constexpr uint32_t MulMod(uint32_t A, uint32_t B) {
    auto C = (uint64_t) A * B;
    auto D = (uint32_t) (C >> F);
    auto R = (uint32_t) (((uint64_t) H * D) >> G);
    auto X = (uint32_t) C - R * Mod;
    return Min(X, X - Mod, X - Mod - Mod);
}

void VMul0(uint32_t* restrict_ D,
    const uint32_t* restrict_ A,
    const uint32_t* restrict_ B)
{
    D[0] = (uint32_t) ((uint64_t) A[0] * B[0] % Mod);
    D[1] = (uint32_t) ((uint64_t) A[1] * B[1] % Mod);
    D[2] = (uint32_t) ((uint64_t) A[2] * B[2] % Mod);
    D[3] = (uint32_t) ((uint64_t) A[3] * B[3] % Mod);
    D[4] = (uint32_t) ((uint64_t) A[4] * B[4] % Mod);
    D[5] = (uint32_t) ((uint64_t) A[5] * B[5] % Mod);
    D[6] = (uint32_t) ((uint64_t) A[6] * B[6] % Mod);
    D[7] = (uint32_t) ((uint64_t) A[7] * B[7] % Mod);
}

void VMul1(uint32_t* restrict_ D,
    const uint32_t* restrict_ A,
    const uint32_t* restrict_ B)
{
    D[0] = MulMod(A[0], B[0]);
    D[1] = MulMod(A[1], B[1]);
    D[2] = MulMod(A[2], B[2]);
    D[3] = MulMod(A[3], B[3]);
    D[4] = MulMod(A[4], B[4]);
    D[5] = MulMod(A[5], B[5]);
    D[6] = MulMod(A[6], B[6]);
    D[7] = MulMod(A[7], B[7]);
}

void VMul2(uint32_t* restrict_ D,
    const uint32_t* restrict_ A,
    const uint32_t* restrict_ B)
{
    auto va0 = _mm256_cvtepu32_epi64(_mm_load_si128((const __m128i* restrict_) (A + 0)));
    auto va1 = _mm256_cvtepu32_epi64(_mm_load_si128((const __m128i* restrict_) (A + 4)));
    auto vb0 = _mm256_cvtepu32_epi64(_mm_load_si128((const __m128i* restrict_) (B + 0)));
    auto vb1 = _mm256_cvtepu32_epi64(_mm_load_si128((const __m128i* restrict_) (B + 4)));
    auto vc0 = _mm256_mul_epu32(va0, vb0);
    auto vc1 = _mm256_mul_epu32(va1, vb1);
    auto vd0 = _mm256_srli_epi64(vc0, F);
    auto vd1 = _mm256_srli_epi64(vc1, F);
    const auto vh = _mm256_set1_epi64x(H);
    auto vr0 = _mm256_srli_epi64(_mm256_mul_epu32(vd0, vh), G);
    auto vr1 = _mm256_srli_epi64(_mm256_mul_epu32(vd1, vh), G);
    const auto vm_ = _mm256_set1_epi64x(Mod);
    const auto vp0 = _mm256_set_epi64x(0, 0, 0x0000000600000004, 0x0000000200000000);
    const auto vp1 = _mm256_set_epi64x(0x0000000600000004, 0x0000000200000000, 0, 0);
    auto vs0 = _mm256_mul_epu32(vr0, vm_);
    auto vs1 = _mm256_mul_epu32(vr1, vm_);
    auto vc = _mm256_blend_epi32(_mm256_permutevar8x32_epi32(vc0, vp0), _mm256_permutevar8x32_epi32(vc1, vp1), 0xf0);
    auto vs = _mm256_blend_epi32(_mm256_permutevar8x32_epi32(vs0, vp0), _mm256_permutevar8x32_epi32(vs1, vp1), 0xf0);
    auto vx = _mm256_sub_epi32(vc, vs);
    const auto vm = _mm256_set1_epi32(Mod);
    const auto vm2 = _mm256_set1_epi32(Mod + Mod);
    auto vxm = _mm256_sub_epi32(vx, vm);
    auto vxm2 = _mm256_sub_epi32(vx, vm2);
    auto vres = _mm256_min_epu32(_mm256_min_epu32(vx, vxm), vxm2);
    _mm256_store_si256((__m256i* restrict_) D, vres);
}

void VMul3(uint32_t* restrict_ D,
    const uint32_t* restrict_ A,
    const uint32_t* restrict_ B)
{
    auto va0 = _mm256_load_si256((const __m256i* restrict_) A);
    auto vb0 = _mm256_load_si256((const __m256i* restrict_) B);
    auto va1 = _mm256_shuffle_epi32(va0, 0xb1);
    auto vb1 = _mm256_shuffle_epi32(vb0, 0xb1);
    auto vc0 = _mm256_mul_epu32(va0, vb0);
    auto vc1 = _mm256_mul_epu32(va1, vb1);
    auto vd0 = _mm256_srli_epi64(vc0, F);
    auto vd1 = _mm256_srli_epi64(vc1, F);
    const auto vh = _mm256_set1_epi64x(H);
    auto vr0 = _mm256_srli_epi64(_mm256_mul_epu32(vd0, vh), G);
    auto vr1 = _mm256_srli_epi64(_mm256_mul_epu32(vd1, vh), G);
    const auto vm_ = _mm256_set1_epi64x(Mod);
    auto vs0 = _mm256_mul_epu32(vr0, vm_);
    auto vs1 = _mm256_mul_epu32(vr1, vm_);
    auto vc = _mm256_blend_epi32(vc0, _mm256_shuffle_epi32(vc1, 0xb1), 0xaa);
    auto vs = _mm256_blend_epi32(vs0, _mm256_shuffle_epi32(vs1, 0xb1), 0xaa);
    auto vx = _mm256_sub_epi32(vc, vs);
    const auto vm = _mm256_set1_epi32(Mod);
    const auto vm2 = _mm256_set1_epi32(Mod + Mod);
    auto vxm = _mm256_sub_epi32(vx, vm);
    auto vxm2 = _mm256_sub_epi32(vx, vm2);
    auto vres = _mm256_min_epu32(_mm256_min_epu32(vx, vxm), vxm2);
    _mm256_store_si256((__m256i* restrict_) D, vres);
}

void VMul4(uint32_t* restrict_ D,
    const uint32_t* restrict_ A,
    const uint32_t* restrict_ B)
{
    auto va = _mm256_load_si256((const __m256i* restrict_) A);
    auto vb = _mm256_load_si256((const __m256i* restrict_) B);
    const auto vp0 = _mm256_set_epi64x(0x111111111b1a1918, 0x1111111113121110, 0x111111110b0a0908, 0x1111111103020100);
    const auto vp1 = _mm256_set_epi64x(0x111111111f1e1d1c, 0x1111111117161514, 0x111111110f0e0d0c, 0x1111111107060504);
    auto va0 = _mm256_shuffle_epi8(va, vp0);
    auto va1 = _mm256_shuffle_epi8(va, vp1);
    auto vb0 = _mm256_shuffle_epi8(vb, vp0);
    auto vb1 = _mm256_shuffle_epi8(vb, vp1);
    auto vc0 = _mm256_mul_epu32(va0, vb0);
    auto vc1 = _mm256_mul_epu32(va1, vb1);
    auto vd0 = _mm256_srli_epi64(vc0, F);
    auto vd1 = _mm256_srli_epi64(vc1, F);
    const auto vh = _mm256_set1_epi64x(H);
    auto vr0 = _mm256_srli_epi64(_mm256_mul_epu32(vd0, vh), G);
    auto vr1 = _mm256_srli_epi64(_mm256_mul_epu32(vd1, vh), G);
    const auto vm_ = _mm256_set1_epi64x(Mod);
    auto vs0 = _mm256_mul_epu32(vr0, vm_);
    auto vs1 = _mm256_mul_epu32(vr1, vm_);
    auto vc = _mm256_blend_epi32(vc0, _mm256_shuffle_epi32(vc1, 0xb1), 0xaa);
    auto vs = _mm256_blend_epi32(vs0, _mm256_shuffle_epi32(vs1, 0xb1), 0xaa);
    auto vx = _mm256_sub_epi32(vc, vs);
    const auto vm = _mm256_set1_epi32(Mod);
    const auto vm2 = _mm256_set1_epi32(Mod + Mod);
    auto vxm = _mm256_sub_epi32(vx, vm);
    auto vxm2 = _mm256_sub_epi32(vx, vm2);
    auto vres = _mm256_min_epu32(_mm256_min_epu32(vx, vxm), vxm2);
    _mm256_store_si256((__m256i* restrict_) D, vres);
}

constexpr size_t K = 5;
constexpr size_t N = 1000000;
constexpr size_t P = N * 8;

constexpr int WU = 40;
constexpr int RU = 80;

uint32_t* Alloc() {
  return (uint32_t*)
#ifdef _WIN32
    _aligned_malloc(sizeof(uint32_t) * P, 32);
#else
    aligned_alloc(32, sizeof(uint32_t) * P);
#endif
}

uint32_t* A;
uint32_t* B;
uint32_t* C[K];

using namespace chrono;

template<class F>
auto Test(F&& VMul) {
  for (int i = 0; i < WU; ++i) {
    for (size_t j = 0; j < N; ++j)
      VMul(C[0] + j * 8, A + j * 8, B + j * 8);
    MFence();
  }
  auto T0 = high_resolution_clock::now();
  for (int i = 0; i < RU; ++i) {
    for (size_t j = 0; j < N; ++j)
      VMul(C[0] + j * 8, A + j * 8, B + j * 8);
    MFence();
  }
  auto T1 = high_resolution_clock::now();
  return T1 - T0;
}

int main() {
  A = Alloc();
  B = Alloc();
  for (size_t i = 0; i < K; ++i)
    C[i] = Alloc();
  mt19937 MT{random_device{}()};
  uniform_int_distribution<uint32_t> U{0, Mod - 1};
  for (size_t i = 0; i < P; ++i)
    A[i] = U(MT);
  for (size_t i = 0; i < P; ++i)
    B[i] = U(MT);
  if (P < 100) {
    printf("A:");
    for (size_t i = 0; i < P; ++i)
      printf(" %u", A[i]);
    printf("\nB:");
    for (size_t i = 0; i < P; ++i)
      printf(" %u", B[i]);
    printf("\n");
  }
  for (size_t i = 0; i < N; ++i) {
    VMul0(C[0] + i * 8, A + i * 8, B + i * 8);
    VMul1(C[1] + i * 8, A + i * 8, B + i * 8);
    VMul2(C[2] + i * 8, A + i * 8, B + i * 8);
    VMul3(C[3] + i * 8, A + i * 8, B + i * 8);
    VMul4(C[4] + i * 8, A + i * 8, B + i * 8);
  }
  for (size_t i = 1; i < K; ++i)
    if (memcmp(C[0], C[i], sizeof(uint32_t) * P))
      printf("Fuck %zu\n", i);
  printf("AllCorrect\n");
  printf("VMul0: %lld us\n", (long long) duration_cast<microseconds>(Test(VMul0)).count());
  printf("VMul1: %lld us\n", (long long) duration_cast<microseconds>(Test(VMul1)).count());
  printf("VMul2: %lld us\n", (long long) duration_cast<microseconds>(Test(VMul2)).count());
  printf("VMul3: %lld us\n", (long long) duration_cast<microseconds>(Test(VMul3)).count());
  printf("VMul4: %lld us\n", (long long) duration_cast<microseconds>(Test(VMul4)).count());
  return 0;
}
