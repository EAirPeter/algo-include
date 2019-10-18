#include <algorithm>
#include <cstdio>
#include <cstring>
#include <utility>
#include <vector>

using namespace std;

const unsigned MaxE = 2000;
const unsigned MaxV = 400;

unsigned EAdj[MaxE], ECap[MaxE], ENxt[MaxE], ECnt;
unsigned Adj[MaxV];

const unsigned Inf = 0x7f7f7f7f;

unsigned Isap(unsigned ntot, unsigned src, unsigned snk) {
    static unsigned dis[MaxV], cnt[MaxV + 1], que[MaxV];
    memset(dis, 0xff, sizeof(unsigned) * ntot);
    memset(cnt, 0x00, sizeof(unsigned) * ntot);
    dis[snk] = 0;
    que[0] = snk;
    for (unsigned qh = 0, qt = 1; qh != qt; ++qh) {
        unsigned u = que[qh];
        ++cnt[dis[u]];
        for (unsigned e = Adj[u]; ~e; e = ENxt[e])
            if (!~dis[EAdj[e]]) {
                dis[EAdj[e]] = dis[u] + 1;
                que[qt++] = EAdj[e];
            }
    }
    static unsigned cur[MaxV], prv[MaxV];
    memcpy(cur, Adj, sizeof(unsigned) * ntot);
    unsigned u = src;
    unsigned fmax = 0;
    prv[src] = src;
    while (dis[src] < ntot) {
        if (u == snk) {
            unsigned flow = Inf, back;
            for (unsigned v = src; v != snk; v = EAdj[cur[v]])
                if (ECap[cur[v]] < flow) {
                    flow = ECap[cur[v]];
                    back = v;
                }
            for (unsigned v = src; v != snk; v = EAdj[cur[v]]) {
                ECap[cur[v]] -= flow;
                ECap[cur[v] ^ 1] += flow;
            }
            fmax += flow;
            u = back;
        }
        unsigned e = cur[u];
        while (~e && !(ECap[e] && dis[u] == dis[EAdj[e]] + 1))
            e = ENxt[e];
        if (~e) {
            prv[EAdj[e]] = u;
            cur[u] = e;
            u = EAdj[e];
        }
        else {
            if (!--cnt[dis[u]])
                break;
            unsigned dmin = ntot - 1;
            for (unsigned e = Adj[u]; ~e; e = ENxt[e])
                if (ECap[e] && dis[EAdj[e]] < dmin) {
                    dmin = dis[EAdj[e]];
                    cur[u] = e;
                }
            ++cnt[dis[u] = dmin + 1];
            cur[u] = Adj[u];
            u = prv[u];
        }
    }
    return fmax;
}

void AddEdge(unsigned u, unsigned v, unsigned cap) {
    EAdj[ECnt] = v;
    ECap[ECnt] = cap;
    ENxt[ECnt] = Adj[u];
    Adj[u] = ECnt++;
    EAdj[ECnt] = u;
    ECap[ECnt] = 0;
    ENxt[ECnt] = Adj[v];
    Adj[v] = ECnt++;
}

int main() {
    unsigned n, m;
    while (scanf("%u%u", &m, &n) == 2) {
        ECnt = 0;
        memset(Adj, 0xff, sizeof(unsigned) * n);
        while (m--) {
            unsigned u, v, c;
            scanf("%u%u%u", &u, &v, &c);
            AddEdge(u - 1, v - 1, c);
        }
        printf("%u\n", Isap(n, 0, n - 1));
    }
    return 0;
}
