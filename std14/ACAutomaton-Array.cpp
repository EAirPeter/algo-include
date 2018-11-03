// LGOJ3808
#include <cstddef>
#include <cstdio>
#include <cstring>

using namespace std;

constexpr size_t MaxTxt = 1000001;
constexpr size_t MaxPat = 1000001;

using Key = char;

struct AcaNode {
    AcaNode* child[26];
    AcaNode* fail;
    AcaNode* nexw; // Next word when traversing along fail chain
    size_t words;
} AcaBuf[MaxPat];
size_t AcaCnt;

inline AcaNode* AcaNew() noexcept {
    auto res = AcaBuf + AcaCnt++;
    return res;
}

inline void AcaInsert(AcaNode* Root, const Key* S) noexcept {
    while (*S) {
        auto key = *S - 'a';
        if (!Root->child[key])
            Root->child[key] = AcaNew();
        Root = Root->child[key];
        ++S;
    }
    ++Root->words;
}

void AcaBuild(AcaNode* Root) noexcept {
    static AcaNode* Q[MaxPat];
    Root->fail = Root->nexw = Root;
    size_t qend = 0;
    for (auto key = 0; key < 26; ++key)
        if (Root->child[key]) {
            auto cur = Root->child[key];
            cur->fail = cur->nexw = Root;
            Q[qend++] = cur;
        }
    for (size_t qbeg = 0; qbeg != qend; ++qbeg)
        for (auto key = 0; key < 26; ++key)
            if (Q[qbeg]->child[key]) {
                auto cur = Q[qbeg]->child[key];
                auto fail = Q[qbeg]->fail;
                while (fail != Root && !fail->child[key])
                    fail = fail->fail;
                if (auto nxt = fail->child[key]) {
                    cur->fail = nxt;
                    cur->nexw = nxt->words ? nxt : nxt->nexw;
                }
                else
                    cur->fail = cur->nexw = Root;
                Q[qend++] = cur;
            }
}

size_t AcaMatch(AcaNode* Root, const Key* S) noexcept {
    size_t res = 0;
    for (auto fail = Root; *S; ++S) {
        auto key = *S - 'a';
        while (fail != Root && !fail->child[key])
            fail = fail->fail;
        if (auto nxt = fail->child[key]) {
            fail = nxt;
            // for (auto cur = fail; cur != Root; cur = cur->nexw)
            //     res += cur->words;
            for (auto cur = fail; ~cur->words; cur = cur->nexw) {
                res += cur->words;
                cur->words = ~size_t{};
            }
        }
    }
    return res;
}

int main() {
    size_t N;
    scanf("%zu", &N);
    auto Root = AcaNew();
    Root->words = ~size_t{};
    static char S[MaxTxt];
    for (size_t i = 0; i < N; ++i) {
        scanf("%s", S);
        AcaInsert(Root, S);
    }
    AcaBuild(Root);
    scanf("%s", S);
    printf("%zu\n", AcaMatch(Root, S));
}
