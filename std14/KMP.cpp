// LGOJ3375
#include <cstddef>
#include <cstdio>
#include <cstring>

// Fail[i] = Argmax_{j} {Pat_{0 ~ (j - 1)} = Pat_{(i - j + 1) ~ i}}
template<class Elem>
constexpr void KmpFail(size_t* Fail, Elem* Pat, size_t PLen) noexcept {
    Fail[0] = 0;
    size_t j = 0;
    for (size_t i = 1; i < PLen; ++i) {
        while (j && Pat[i] != Pat[j])
            j = Fail[j - 1];
        if (Pat[i] == Pat[j])
            ++j;
        Fail[i] = j;
    }
}

template<class Elem>
constexpr size_t KmpMatch(size_t* Res, size_t* Fail, Elem* Pat, size_t PLen, Elem* Txt, size_t TLen) noexcept {
    size_t cnt = 0;
    size_t j = 0;
    for (size_t i = 0; i < TLen; ++i) {
        while (j && Txt[i] != Pat[j])
            j = Fail[j - 1];
        if (Txt[i] == Pat[j])
            ++j;
        if (j == PLen) {
            Res[cnt++] = i - PLen + 1;
            j = Fail[j - 1];
        }
    }
    return cnt;
}

char A[1000001], B[1000001];
size_t F[1000001], G[1000001];

int main() {
    scanf("%s%s", A, B);
    auto la = strlen(A);
    auto lb = strlen(B);
    KmpFail(F, B, lb);
    auto res = KmpMatch(G, F, B, lb, A, la);
    for (size_t i = 0; i < res; ++i)
        printf("%zu\n", G[i] + 1);
    for (size_t i = 0; i < lb - 1; ++i)
        printf("%zu ", F[i]);
    printf("%zu\n", F[lb - 1]);
    return 0;
}
