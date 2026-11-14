#include "adversary/generators.hpp"

namespace leaderboard::adversary {
std::vector<Entry> rotation_inserts(std::size_t n) {
    std::vector<Entry> out; out.reserve(n);
    // Increasing scores are reverse-sorted keys; AVL must rebalance repeatedly.
    for (std::size_t i = 0; i < n; ++i)
        out.push_back({static_cast<std::int64_t>(i), static_cast<std::int64_t>(i)});
    return out;
}
// The key at tree position p (0 = first key = highest score) after
// rotation_inserts(n) is entry n-1-p.
static Entry at_position(std::size_t n, std::size_t p) {
    auto v = static_cast<std::int64_t>(n - 1 - p);
    return {v, v};
}
std::vector<Entry> rotation_deletes(std::size_t n) {
    // Found by a greedy search that tried every possible next delete (see
    // scratch/scratch-e). Opening, by n % 4:
    //   0: nothing;  2: positions 0, 1;  3: positions 0, 1, 2;
    //   1: positions 3, 0, 1, 2, 4 (a block of five; for n = 1, just position 0).
    // Then the remaining positions in blocks of four b..b+3 from the left end,
    // deleting the third, first, second and fourth key of each block.
    // Measured: equals the greedy search for every n up to 200, and at least
    // n - bitlen(n) - 1 rotations for every n up to 3000, about one per delete.
    // Deleting from one end gives about 0.5.
    std::vector<Entry> out; out.reserve(n);
    std::size_t start = 0;
    if (n % 4 == 1 && n >= 5) {
        for (std::size_t p : {3u, 0u, 1u, 2u, 4u}) out.push_back(at_position(n, p));
        start = 5;
    } else {
        start = n % 4;
        for (std::size_t p = 0; p < start; ++p) out.push_back(at_position(n, p));
    }
    for (std::size_t b = start; b + 3 < n; b += 4)
        for (std::size_t offset : {2u, 0u, 1u, 3u}) out.push_back(at_position(n, b + offset));
    return out;
}
std::vector<Entry> tied_inserts(std::size_t n) {
    // Every key has score 0, so the order comes from the ID tie-break alone. IDs
    // fall, so each new key is the new first key, as in rotation_inserts.
    std::vector<Entry> out; out.reserve(n);
    for (std::size_t i = 0; i < n; ++i)
        out.push_back({0, static_cast<std::int64_t>(n - 1 - i)});
    return out;
}
std::vector<Entry> rotation_deletes_one_end(std::size_t n) {
    // The starter schedule, kept for comparison: score 0 first, which is always the
    // current last key.
    return rotation_inserts(n);
}
std::pair<std::vector<Entry>, std::vector<Entry>> unequal_join(std::size_t n) {
    std::vector<Entry> tall, short_tree;
    tall.reserve(n);
    for (std::size_t i = 0; i < n; ++i)
        tall.push_back({static_cast<std::int64_t>(2 * n - i),
                        static_cast<std::int64_t>(i)});
    short_tree.push_back({0, static_cast<std::int64_t>(n + 1)});
    return {std::move(tall), std::move(short_tree)};
}
std::tuple<std::vector<Entry>, Entry, std::vector<Entry>>
ordered_join_case(std::size_t left_n, std::size_t right_n) {
    // Higher scores come first, so the left tree takes the top scores.
    // Right: scores 0..right_n-1, pivot: right_n, left: right_n+1..right_n+left_n.
    std::vector<Entry> left, right;
    left.reserve(left_n); right.reserve(right_n);
    for (std::size_t i = 0; i < right_n; ++i)
        right.push_back({static_cast<std::int64_t>(i), static_cast<std::int64_t>(i)});
    Entry pivot{static_cast<std::int64_t>(right_n), static_cast<std::int64_t>(right_n)};
    for (std::size_t i = 1; i <= left_n; ++i)
        left.push_back({static_cast<std::int64_t>(right_n + i),
                        static_cast<std::int64_t>(right_n + i)});
    return {std::move(left), pivot, std::move(right)};
}
}
