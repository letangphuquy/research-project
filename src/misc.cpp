#include "template.hpp"

int main() {

    Real avg_time_iter = 0;
    Real avg_time_default = 0;
    Real avg_size = 0;

    const int T = 200;
    const int MOD = 1e9 + 7;

    for (int _ = 0; _ < T; _++) {
        const int N = 2e5;
        Gene gene(random_int(1,N));
        if (random_int(0,1)) gene.resize(4950);
        if (!_) {
            for (auto it = begin(gene); it != end(gene); it++) {
                cout << (*it.base()) << ' ' << it.position() << '\n';
            }
            break;
        }
        bit::fill(all_of(gene), bit::bit0);
        for (int i = 0; i < gene.size(); i++)
            possibly(0.5, [&] {gene[i].flip();} );
        avg_size += bit::count(all_of(gene), bit::bit1);
        // std::shuffle(all_of(gene), rng);
        int ans_iter = 1, ans_default = 1;
        avg_time_iter += benchmark(
            [&] {
                ans_iter = 1;
                {
                    WordType msk; 
                    int block_idx = 0, idx, size = gene.size();
                    for (auto it = begin(gene); block_idx < size; it += 64, block_idx += 64) {
                        for (msk = *it.base(); msk > 0; msk ^= msk&-msk) {
                            idx = block_idx + __builtin_ctzll(msk);
                            gene[idx];
                            ans_iter = (1LL * ans_iter * (idx + 1)) % MOD;
                        }
                    }
                }
                /*
                {
                    int idx = 1;
                    for (auto it = begin(gene); it != end(gene); it++, idx++) {
                        if (*it) answer = (1LL * answer * idx) % MOD;
                    }
                }
                */
               /*
                iterate(gene) 
                    if (!bit0) answer = (1LL * answer * (idx + 1)) % MOD;
                }}
               */
            }
        );
        avg_time_default += benchmark(
            [&] {
                ans_default = 1;
                for (int i = 0; i < gene.size(); i++)
                    if (gene[i]) ans_default = (1LL * ans_default * (i+1)) % MOD;
            }
        );
        assert(ans_iter == ans_default);
    }
    avg_time_iter /= T;
    avg_time_default /= T;
    avg_size /= T;

    cout << std::fixed << std::setprecision(8);
    PRINTLN(cout, avg_time_default);
    PRINTLN(cout, avg_time_iter);
    PRINT(cout, avg_size);
}