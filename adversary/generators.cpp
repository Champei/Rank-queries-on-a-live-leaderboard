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
    // scratch/scratch-e). Delete the n % 4 first keys, then walk the rest in blocks
    // of four positions b..b+3 from the left end, deleting the third, first, second
    // and fourth key of each block. Measured: at least n - bitlen(n) - 1 rotations for
    // every n up to 3000, about one per delete; deleting from one end gives about 0.5.
    std::vector<Entry> out; out.reserve(n);
    std::size_t lead = n % 4;
    for (std::size_t p = 0; p < lead; ++p) out.push_back(at_position(n, p));
    for (std::size_t b = lead; b + 3 < n; b += 4)
        for (std::size_t offset : {2, 0, 1, 3}) out.push_back(at_position(n, b + offset));
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
