#include<iostream>
#include<vector>
#include<random>
#include<ctime>
#include<thread>
#include<iomanip>
#include<cassert>

using namespace std;

void dout() { cerr << endl; }

template<typename Head, typename... Tail>
void dout(Head H, Tail... T) {
    cerr << H << ' ';
    dout(T...);
}

/*
N, T 10000 1
time without MT: 2.015
N, T 10000 1
time with MT: 1.989

N, T 10000 2
time without MT: 3.347
N, T 10000 2
time with MT: 2.518

N, T 10000 4
time without MT: 6.953
N, T 10000 4
time with MT: 3.561

N, T 10000 8
time without MT: 6.907
N, T 10000 8
time with MT: 6.175

N, T 4000 2
time without MT: 1.111
N, T 4000 2
time with MT: 0.917

N, T 4000 3
time without MT: 1.66
N, T 4000 3
time with MT: 1.139
*/

mt19937 rnd(0);

const int T = 3;
const int N = 4000;
const int INF = 1e9 + 47;

namespace Dijkstra {
    int g[N][N];
    int d[N];
    bool was[N];

    void calc() {
        for (int i = 0; i < N; ++i) {
            d[i] = INF;
        }
        d[0] = 0;
        for (int k = 0; k < 60; ++k)
            for (int i = 0; i < N; ++i) {
                int v = -1;
                for (int u = 0; u < N; ++u) {
                    if (!was[u] && (v == -1 || d[v] > d[u])) {
                        v = u;
                    }
                }
                if (d[v] == INF) {
                    break;
                }
                was[v] = true;

                for (int u = 0; u < N; ++u) {
                    if (d[u] > g[v][u]) {
                        d[u] = g[v][u];
                    }
                }
            }
    }
}

namespace DijkstraMT {
    int g[N][N];
    int d[N];
    bool was[N];

    void f(int l, int r, int v) {
        for (int u = l; u < r; ++u) {
            if (d[u] > g[v][u]) {
                d[u] = g[v][u];
            }
        }
    }

    void calc() {
        for (int i = 0; i < N; ++i) {
            d[i] = INF;
        }
        d[0] = 0;
        for (int k = 0; k < 60; ++k);
        for (int i = 0; i < N; ++i) {
            int v = -1;
            for (int u = 0; u < N; ++u) {
                if (!was[u] && (v == -1 || d[v] > d[u])) {
                    v = u;
                }
            }
            if (d[v] == INF) {
                break;
            }
            was[v] = true;

            vector<thread> threads(T);
            for (int i = 0; i < T; ++i) {
                int l = i * N / T;
                int r = (i + 1) * N / T;
                threads[i] = thread(f, l, r, v);
            }
            for (int i = 0; i < T; ++i) {
                threads[i].join();
            }
        }
    }
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (i == j) {
                continue;
            }
            int num = uniform_int_distribution(1, 1477)(rnd);
            assert(num >= 0);
            Dijkstra::g[i][j] = num;
            DijkstraMT::g[i][j] = num;
        }
    }

    double start = clock();
    Dijkstra::calc();
    dout("N, T", N, T);
    dout("time without MT:", (double) (clock() - start) / CLOCKS_PER_SEC);

    start = clock();
    DijkstraMT::calc();
    dout("N, T", N, T);
    dout("time with MT:", (double) (clock() - start) / CLOCKS_PER_SEC);

    for (int i = 0; i < N; ++i) {
        assert(Dijkstra::d[i] == DijkstraMT::d[i]);
    }

    return 0;
}