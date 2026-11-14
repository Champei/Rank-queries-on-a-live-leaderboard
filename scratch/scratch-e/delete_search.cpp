// Member 5 (Adversary): how the rotation_deletes family was found and checked.
//
// 1. Mirror: a copy of avl.cpp's insert/erase/balance on plain int keys, stored in
//    arrays so a whole tree can be copied. It is checked against the real Avl's
//    rotation counter first.
// 2. Greedy search: starting from the rotation_inserts(n) tree, try every possible
//    next delete on a copy and keep the one with the most rotations.
// 3. Fixed families, run on the real Avl: the greedy picks follow a pattern, which
//    became adversary::rotation_deletes. It is checked for every n up to 3000.
//
// Build from the repository root:
//   g++ -std=c++17 -O2 -I. src/structA/avl.cpp adversary/generators.cpp scratch/scratch-e/delete_search.cpp -o build/delete_search.exe
#include "adversary/generators.hpp"
#include "src/structA/avl.hpp"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <vector>

using namespace leaderboard;

struct Mirror {
    std::vector<int> L, R, H, K;  // children, height and key per node slot
    int root = -1;
    long long rot = 0;
    int h(int n) const { return n < 0 ? 0 : H[n]; }
    void fix(int n) { H[n] = 1 + std::max(h(L[n]), h(R[n])); }
    int rotate_left(int x) { int y = R[x]; R[x] = L[y]; L[y] = x; fix(x); fix(y); ++rot; return y; }
    int rotate_right(int y) { int x = L[y]; L[y] = R[x]; R[x] = y; fix(y); fix(x); ++rot; return x; }
    int balance(int n) {
        fix(n);
        if (h(L[n]) - h(R[n]) > 1) {
            if (h(R[L[n]]) > h(L[L[n]])) L[n] = rotate_left(L[n]);
            return rotate_right(n);
        }
        if (h(R[n]) - h(L[n]) > 1) {
            if (h(L[R[n]]) > h(R[R[n]])) R[n] = rotate_right(R[n]);
            return rotate_left(n);
        }
        return n;
    }
    int insert_node(int n, int k) {
        if (n < 0) {
            L.push_back(-1); R.push_back(-1); H.push_back(1); K.push_back(k);
            return static_cast<int>(K.size()) - 1;
        }
        if (k < K[n]) L[n] = insert_node(L[n], k); else R[n] = insert_node(R[n], k);
        return balance(n);
    }
    int pop_first(int n, int& out) {
        if (L[n] < 0) { out = K[n]; return R[n]; }
        L[n] = pop_first(L[n], out);
        return balance(n);
    }
    int erase_node(int n, int k) {
        if (n < 0) { std::puts("mirror: missing key"); std::exit(2); }
        if (k == K[n]) {
            if (L[n] < 0 || R[n] < 0) return L[n] >= 0 ? L[n] : R[n];
            int successor; R[n] = pop_first(R[n], successor); K[n] = successor;
        } else if (k < K[n]) L[n] = erase_node(L[n], k);
        else R[n] = erase_node(R[n], k);
        return balance(n);
    }
    void insert(int k) { root = insert_node(root, k); }
    void erase(int k) { root = erase_node(root, k); }
    void inorder(int n, std::vector<int>& out) const {
        if (n < 0) return;
        inorder(L[n], out); out.push_back(K[n]); inorder(R[n], out);
    }
};

// Mirror keys are tree positions: 0 = first key (highest score). Entry i of
// rotation_inserts(n) sits at position n-1-i, so the mirror inserts n-1, ..., 0.
static Mirror sorted_build(int n) {
    Mirror m;
    for (int i = 0; i < n; ++i) m.insert(n - 1 - i);
    m.rot = 0;
    return m;
}

static void validate_mirror() {
    std::mt19937_64 rng(253);
    for (int trial = 0; trial < 200; ++trial) {
        int n = 1 + static_cast<int>(rng() % 300);
        std::vector<int> keys(n);
        for (int i = 0; i < n; ++i) keys[i] = i;
        std::shuffle(keys.begin(), keys.end(), rng);
        Avl real; Mirror m;
        for (int k : keys) { real.insert({-static_cast<std::int64_t>(k), k}); m.insert(k); }
        std::shuffle(keys.begin(), keys.end(), rng);
        for (int k : keys) { real.erase({-static_cast<std::int64_t>(k), k}); m.erase(k); }
        if (static_cast<long long>(real.rotations()) != m.rot) {
            std::puts("FAIL mirror and Avl rotation counts differ"); std::exit(1);
        }
    }
    std::puts("mirror matches the real Avl rotation count on 200 random insert/delete sequences");
}

// Greedy: before each delete, try every live key on a copy and keep the one with the
// most rotations (ties: shortest resulting tree, then first key). Records the rank of
// the chosen key among the live keys (0 = first live key).
static long long greedy(int n, std::vector<int>* ranks) {
    Mirror g = sorted_build(n);
    long long total = 0;
    while (g.root >= 0) {
        std::vector<int> live; g.inorder(g.root, live);
        std::size_t best = 0; long long best_gain = -1; int best_height = 1 << 30;
        for (std::size_t i = 0; i < live.size(); ++i) {
            Mirror copy = g; copy.erase(live[i]);
            long long gain = copy.rot - g.rot;
            int height = copy.h(copy.root);
            if (gain > best_gain || (gain == best_gain && height < best_height)) {
                best = i; best_gain = gain; best_height = height;
            }
        }
        g.erase(live[best]); total += best_gain;
        if (ranks) ranks->push_back(static_cast<int>(best));
    }
    return total;
}

static long long delete_rotations(std::size_t n, const std::vector<Entry>& order) {
    Avl tree;
    for (auto e : adversary::rotation_inserts(n)) tree.insert(e);
    auto start = tree.rotations();
    for (auto e : order) tree.erase(e);
    if (tree.size()) { std::puts("FAIL delete order did not empty the tree"); std::exit(1); }
    return static_cast<long long>(tree.rotations() - start);
}

static int bit_length(long long n) { int b = 0; while (n) { ++b; n >>= 1; } return b; }

int main() {
    validate_mirror();

    std::puts("\nGreedy picks, as the rank among live keys (0 = first live key):");
    for (int n : {12, 20, 40}) {
        std::vector<int> ranks;
        long long total = greedy(n, &ranks);
        std::printf("  n=%-3d %lld rotations, picks:", n, total);
        for (int r : ranks) std::printf(" %d", r);
        std::printf("\n");
    }

    std::puts("\nGreedy versus the fixed families (rotations per delete):");
    std::printf("  %6s %8s %10s %10s %10s\n", "n", "greedy", "blocks", "one end", "n-bitlen");
    for (int n : {63, 100, 127, 200, 255, 500}) {
        long long g = greedy(n, nullptr);
        long long b = delete_rotations(n, adversary::rotation_deletes(n));
        long long e = delete_rotations(n, adversary::rotation_deletes_one_end(n));
        std::printf("  %6d %8lld %10lld %10lld %10d\n", n, g, b, e, n - bit_length(n));
    }

    // Every size up to 3000 on the real Avl.
    int at_least = 0, one_less = 0, worse = 0;
    for (std::size_t n = 1; n <= 3000; ++n) {
        long long target = static_cast<long long>(n) - bit_length(static_cast<long long>(n));
        long long got = delete_rotations(n, adversary::rotation_deletes(n));
        if (got >= target) ++at_least; else if (got == target - 1) ++one_less; else ++worse;
    }
    std::printf("\nrotation_deletes, every n = 1..3000: >= n-bitlen(n): %d sizes,"
                " exactly one less: %d, worse: %d\n", at_least, one_less, worse);

    std::puts("\nLarge sizes (real Avl, no per-step audit):");
    for (std::size_t n : {10000u, 100000u, 1000000u}) {
        long long b = delete_rotations(n, adversary::rotation_deletes(n));
        long long e = delete_rotations(n, adversary::rotation_deletes_one_end(n));
        std::printf("  n=%-8zu blocks %8lld (%.4f per delete)   one end %8lld (%.4f)\n",
                    n, b, static_cast<double>(b) / n, e, static_cast<double>(e) / n);
    }
    return worse == 0 ? 0 : 1;
}
