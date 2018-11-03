#include <algorithm>
#include <cstdio>
#include <cstring>
#include <utility>

using namespace std;

typedef int I32;
typedef unsigned int U32;
typedef long long I64;
typedef unsigned long long U64;

const U32 BufSize = 200010;
const U32 StkSize = BufSize * 4;

U32 Rank[StkSize];
U32 Suff[StkSize];
U32 Heit[BufSize];

void RadixSort(U32 *dst, U32 *src, U32 *rank, U32 len, U32 chset) {
    static U32 RdxCnt[BufSize];
    memset(RdxCnt, 0, sizeof(U32) * chset);
    for (U32 i = 0; i < len; ++i)
        ++RdxCnt[rank[src[i]]];
    for (U32 i = 1; i < chset; ++i)
        RdxCnt[i] += RdxCnt[i - 1];
    for (U32 i = len - 1; i < len; --i)
        dst[--RdxCnt[rank[src[i]]]] = src[i];
}

void SkewAlgo(U32 *suff, U32 *rank, U32 len, U32 chset) {
    rank[len] = rank[len + 1] = rank[len + 2] = 0;
    U32 *suff12 = suff + len + 3;
    U32 *rank12 = rank + len + 3;
    // in order to append a dummy (0, 0, 0) when len % 3 == 1
    U32 len_ = len % 3 == 1 ? len + 1 : len;
    U32 len12 = 0;
    for (U32 i = 0; i < len_; ++i)
        if (i % 3)
            rank12[len12++] = i;
    RadixSort(suff12, rank12, rank + 2, len12, chset);
    RadixSort(rank12, suff12, rank + 1, len12, chset);
    RadixSort(suff12, rank12, rank, len12, chset);
    // len0 is equivalent to len1
    U32 len1 = (len_ + 1) / 3;
    U32 chset12 = 0;
    U64 hprv = 0xffffffffffffffffULL;
    for (U32 i = 0; i < len12; ++i) {
        U32 idx = suff12[i];
        U64 hcur = (U64) rank[idx] << 42 | (U64) rank[idx + 1] << 21 | rank[idx + 2];
        if (hcur != hprv)
            ++chset12;
        rank12[idx % 3 == 1 ? idx / 3 : idx / 3 + len1] = chset12;
        hprv = hcur;
    }
    // recurse only if chset12 != len12
    if (chset12 != len12)
        SkewAlgo(suff12, rank12, len12, chset12 + 1);
    else
        for (U32 i = 0; i < len12; ++i)
            suff12[rank12[i] - 1] = i;
    static U32 Suff0[BufSize], Rank0[BufSize];
    for (U32 i = 0, j = 0; i < len12; ++i)
        if (suff12[i] < len1)
            Rank0[j++] = suff12[i] * 3;
    RadixSort(Suff0, Rank0, rank, len1, chset);
    U32 ranks = 0;
    U32 i0 = 0;
    // if a dummy (0, 0, 0) presents, which must be at 0, simply ignore it
    // equivalent to len % 3 == 1 ? 1 : 0
    for (U32 i12 = len_ - len; i12 < len12; ++i12) {
        if (suff12[i12] < len1) {
            U32 idx1 = suff12[i12] * 3 + 1;
            U64 hash1 = (U64) rank[idx1] << 21 | rank12[suff12[i12] + len1];
            while (i0 < len1) {
                U32 idx0 = Suff0[i0];
                U64 hash0 = (U64) rank[idx0] << 21 | rank12[idx0 / 3];
                if (hash1 < hash0)
                    break;
                suff[ranks++] = idx0;
                ++i0;
            }
            suff[ranks++] = idx1;
        }
        else {
            U32 idx2 = (suff12[i12] - len1) * 3 + 2;
            U64 hash2 = (U64) rank[idx2] << 42 |
                (U64) rank[idx2 + 1] << 21 | rank12[suff12[i12] - len1 + 1];
            while (i0 < len1) {
                U32 idx0 = Suff0[i0];
                U64 hash0 = (U64) rank[idx0] << 42 |
                    (U64) rank[idx0 + 1] << 21 | rank12[idx0 / 3 + len1];
                if (hash2 < hash0)
                    break;
                suff[ranks++] = idx0;
                ++i0;
            }
            suff[ranks++] = idx2;
        }
    }
    while (i0 < len1)
        suff[ranks++] = Suff0[i0++];
    for (U32 i = 0; i < len; ++i)
        rank[suff[i]] = i + 1;
}

void SuffixArray(char *str, U32 len, U32 chset) {
    for (U32 i = 0; i < len; ++i)
        Rank[i] = (U32) (unsigned char) str[i];
    SkewAlgo(Suff, Rank, len, chset);
    U32 curh = 0;
    for (U32 i = 0; i < len; ++i) {
        if (curh)
            --curh;
        if (Rank[i] > 1)
            for (U32 j = Suff[Rank[i] - 2]; str[i + curh] == str[j + curh]; ++curh);
        Heit[Rank[i] - 1] = curh;
    }
}

int main() {
    static char Str[BufSize];
    scanf("%s", Str);
    U32 len = (U32) strlen(Str);
    SuffixArray(Str, len, 128);
    for (U32 i = 0; i < len; ++i)
        printf("%u%c", Suff[i] + 1, " \n"[i == len - 1]);
    for (U32 i = 0; i < len; ++i)
        printf("%u%c", Heit[i], " \n"[i == len - 1]);
    return 0;
}
