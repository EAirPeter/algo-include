// LGOJ3796
#include <cstddef>
#include <cstdio>
#include <cstring>

using namespace std;

constexpr size_t MaxTxt = 1000001;
constexpr size_t MaxPat = 1000001;

using Key = char;

struct AcaNode {
    AcaNode* child;
    AcaNode* next;
    AcaNode* fail;
    AcaNode* nexw; // Next word when traversing along fail chain
    size_t words;
    Key key;
    constexpr AcaNode* Lookup(Key k) noexcept {
        auto res = child;
        while (res && res->key != k)
            res = res->next;
        return res;
    }
} AcaBuf[MaxPat];
size_t AcaCnt;

inline AcaNode* AcaNew(Key key) noexcept {
    auto res = AcaBuf + AcaCnt++;
    res->key = key;
    return res;
}

inline void AcaInsert(AcaNode* Root, const Key* S) noexcept {
    while (*S) {
        auto cur = Root->Lookup(*S);
        if (!cur) {
            cur = AcaNew(*S);
            cur->next = Root->child;
            Root->child = cur;
        }
        Root = cur;
        ++S;
    }
    ++Root->words;
}

void AcaBuild(AcaNode* Root) noexcept {
    static AcaNode* Q[MaxPat];
    Root->fail = Root->nexw = Root;
    size_t qend = 0;
    for (auto cur = Root->child; cur; cur = cur->next) {
        cur->fail = cur->nexw = Root;
        Q[qend++] = cur;
    }
    for (size_t qbeg = 0; qbeg != qend; ++qbeg)
        for (auto cur = Q[qbeg]->child; cur; cur = cur->next) {
            auto fail = Q[qbeg]->fail;
            while (fail != Root && !fail->Lookup(cur->key))
                fail = fail->fail;
            if (auto nxt = fail->Lookup(cur->key)) {
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
        while (fail != Root && !fail->Lookup(*S))
            fail = fail->fail;
        if (auto nxt = fail->Lookup(*S)) {
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
    auto Root = AcaNew('\0');
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
