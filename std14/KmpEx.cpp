// LGOJ5410
#include <cstddef>
#include <cstdio>
#include <cstring>

// Invariant: Pat[0, k - j) = Pat[j, k)
// Next[i] = Argmax_{j} {Pat[0, j) = Pat[i, i + j)}
template<class Elem>
constexpr void KmpExNext(size_t* Next, const Elem* Pat, size_t PLen) noexcept {
  Next[0] = PLen;
  size_t j = 0;
  size_t k = 0;
  for (size_t i = 1; i < PLen; ++i) {
    if (i >= k || i + Next[i - j] >= k) {
      if (i >= k)
        k = i;
      while (k < PLen && Pat[k] == Pat[k - i])
        ++k;
      Next[i] = k - i;
      j = i;
    }
    else
      Next[i] = Next[i - j];
  }
}

// Invariant: Pat[0, k - j) = Txt[j, k)
// Extend[i] = Argmax_{j} {Pat[0, j) = Txt[i, i + j)}
template<class Elem>
constexpr void KmpExtend(size_t* Extend, size_t* Next,
  const Elem* Pat, size_t PLen, const Elem* Txt, size_t TLen) noexcept
{
  size_t j = 0;
  size_t k = 0;
  for (size_t i = 0; i < TLen; ++i) {
    if (i >= k || i + Next[i - j] >= k) {
      if (i >= k)
        k = i;
      while (k < TLen && k - i < PLen && Txt[k] == Pat[k - i])
        ++k;
      Extend[i] = k - i;
      j = i;
    }
    else
      Extend[i] = Next[i - j];
  }
}

char A[100001], B[100001];
size_t F[100001], G[100001];

int main() {
  scanf("%s%s", A, B);
  auto la = strlen(A);
  auto lb = strlen(B);
  KmpExNext(F, B, lb);
  KmpExtend(G, F, B, lb, A, la);
  for (size_t i = 0; i < lb - 1; ++i)
      printf("%zu ", F[i]);
  printf("%zu\n", F[lb - 1]);
  for (size_t i = 0; i < la - 1; ++i)
      printf("%zu ", G[i]);
  printf("%zu\n", G[la - 1]);
  return 0;
}
