#include "template.hpp"

#define iterate(vec) { \
    int idx = 0, d2n = 0; \
    for (auto it = begin(vec); it != end(vec); it += std::max(1,d2n), idx += std::max(1,d2n)) { \
        int d2n =  __builtin_ctzll(*(it.base()) >> it.position());\
                \


int main()
{
    bit::bit_vector<uint16_t> vec("0000101110101101110101011");
    cout << vec.debug_string() << '\n';
    bit::bit_iterator it = begin(vec);
    for (; it != end(vec); it++) {
        cout << int(*it.base()) << " " << ((int) it.mask()) << " " << it.position() << " " << (*it).address() << '\n';
    }
    it = begin(vec);
    for (int i = 0; it != end(vec); ) {
        int val = *(it.base()) >> it.position();
        int dist = __builtin_ctzll(val);
        cout << "next bit 1 is " << dist << " away\n";
        umax(dist, 1);
        i += dist;
        it += dist;
    }
    iterate(vec) 
        if (!d2n) cout << idx << " bit is on\n";
        cout.flush();
    }}
}