#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <random>

using namespace std;

constexpr uint32_t Thresh = 10000;
constexpr int NCase = 10000;

int main() {
  uint32_t M;
  scanf("%u", &M);
  int F = 0;
  while (F < 32 && (1u << (F + 1)) <= M)
    ++F;
  uint64_t M2 = (uint64_t) (M - 1) * (M - 1);
  int G = 0;
  while (F + G < 64 && (1ull << (F + G)) < M2)
    ++G;
  auto H = (uint32_t) ((uint64_t{1} << (F + G)) / M);
  printf("F=%u G=%u H=%u\n", F, G, H);
  auto Check = [M, F, G, H](uint32_t A, uint32_t B) {
    auto C = (uint64_t) A * B;
    auto Res = (uint32_t) (C % M);
    auto D = (uint32_t) (C >> F);
    //auto E = (uint32_t) C & ((1u << F) - 1);
    auto R_ = (uint32_t) (((uint64_t) H * D) >> G);
    auto Res_ = (uint32_t) C - R_ * M;
    Res_ = min(Res_, min(Res_ - M, Res_ - M - M));
    return Res_ == Res;
  };
  if (M > Thresh) {
    mt19937 MT{random_device{}()};
    for (int i = 0; i < NCase; ++i) {
      auto A = uniform_int_distribution<uint32_t>{0, M - 10001}(MT);
      auto B = uniform_int_distribution<uint32_t>{A, M - 10001}(MT);
      if (!Check(A, B))
        printf("Fuck: %u*%u\n", A, B);
    }
  }
  for (uint32_t A = M < Thresh ? 0 : M - Thresh; A < M; ++A)
    for (uint32_t B = A; B < M; ++B)
      if (!Check(A, B))
        printf("Fuck: %u*%u\n", A, B);
  printf("Done\n");
  return 0;
}
