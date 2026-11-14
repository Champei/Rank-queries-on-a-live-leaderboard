#include "adversary/generators.hpp"
#include "src/structA/avl.hpp"
#include <algorithm>
#include <cstdio>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

using namespace leaderboard;

static void require(bool ok, const std::string& what) {
    if (!ok) throw std::logic_error(what);
}
static int bit_length(std::size_t n) {
    int bits = 0;
    while (n) { ++bits; n >>= 1; }
    return bits;
}
// Every key must land on the correct side of a split, in order, with none lost.
// Checking the in-order concatenation catches misplaced keys that a size check misses.
static void require_partition(const std::vector<Entry>& original, const Avl& first,
                              const Entry* middle, const Avl& second,
                              const std::string& what) {
    auto a = first.entries(), b = second.entries();
    std::vector<Entry> joined = a;
    if (middle) joined.push_back(*middle);
    joined.insert(joined.end(), b.begin(), b.end());
    require(joined.size() == original.size(), what + ": key count");
    for (std::size_t i = 0; i < joined.size(); ++i)
        require(same(joined[i], original[i]), what + ": key order or side");
}
static std::vector<Entry> sorted_keys(std::vector<Entry> keys) {
    std::sort(keys.begin(), keys.end(), before);
    return keys;
}

// Ordered join with chosen sizes, then split at the pivot and rejoin.
static void check_join(std::size_t left_n, std::size_t right_n) {
    auto [left_keys, pivot, right_keys] = adversary::ordered_join_case(left_n, right_n);
    std::string name = "join " + std::to_string(left_n) + "+" + std::to_string(right_n);
    Avl left, right;
    for (auto e : left_keys) left.insert(e);
    for (auto e : right_keys) right.insert(e);
    std::vector<Entry> expected = sorted_keys(left_keys);
    expected.push_back(pivot);
    auto right_sorted = sorted_keys(right_keys);
    expected.insert(expected.end(), right_sorted.begin(), right_sorted.end());

    auto joined = Avl::join(std::move(left), pivot, std::move(right));
    require(joined.audit() == left_n + right_n + 1, name + ": size");
    require_partition(expected, joined, nullptr, Avl{}, name);
    auto [high, found, low] = joined.split(pivot);
    require(found && same(*found, pivot), name + ": pivot not found by split");
    high.audit(); low.audit();
    require_partition(expected, high, &*found, low, name + " split");
    auto rejoined = Avl::join(std::move(high), *found, std::move(low));
    require(rejoined.audit() == expected.size(), name + ": rejoin size");
    require_partition(expected, rejoined, nullptr, Avl{}, name + " rejoin");
}

// Join must refuse keys on the wrong side of the pivot and leave both inputs intact.
static void check_join_rejects_overlap() {
    auto expect_rejected = [](std::vector<Entry> left_keys, Entry pivot,
                              std::vector<Entry> right_keys, const std::string& what) {
        Avl left, right;
        for (auto e : left_keys) left.insert(e);
        for (auto e : right_keys) right.insert(e);
        bool rejected = false;
        try { Avl::join(std::move(left), pivot, std::move(right)); }
        catch (const std::invalid_argument&) { rejected = true; }
        require(rejected, what + ": join accepted it");
        require(left.audit() == left_keys.size() && right.audit() == right_keys.size(),
                what + ": rejected join changed its inputs");
    };
    // Higher score comes first. (3,2) belongs after pivot (4,3), yet it is in the left tree.
    expect_rejected({{5, 1}, {3, 2}}, {4, 3}, {{2, 4}}, "left key after pivot");
    // (6,5) belongs before pivot (4,3), yet it is in the right tree.
    expect_rejected({{5, 1}}, {4, 3}, {{6, 5}, {2, 4}}, "right key before pivot");
    // Pivot equal to an existing key would duplicate it.
    expect_rejected({{5, 1}}, {5, 1}, {{2, 4}}, "pivot duplicates a key");
    // Negative IDs are reserved for split_score's sentinel.
    expect_rejected({{5, 1}}, {4, -1}, {{2, 4}}, "negative pivot ID");
}

static void run(std::size_t n) {
    // 1. Rotation stress: audit the whole tree after every insert and delete.
    Avl tree;
    auto inserts = adversary::rotation_inserts(n);
    for (auto e : inserts) { tree.insert(e); tree.audit(); }
    auto insert_rotations = tree.rotations();
    for (auto e : adversary::rotation_deletes(n)) { tree.erase(e); tree.audit(); }
    require(tree.size() == 0, "delete schedule did not empty tree");
    auto delete_rotations = tree.rotations() - insert_rotations;
    // Same tree, starter delete order, for comparison.
    Avl baseline;
    for (auto e : inserts) baseline.insert(e);
    auto baseline_start = baseline.rotations();
    for (auto e : adversary::rotation_deletes_one_end(n)) { baseline.erase(e); baseline.audit(); }
    require(baseline.size() == 0, "one-end delete schedule did not empty tree");
    auto one_end_rotations = baseline.rotations() - baseline_start;

    // 2. Original unequal-height join: tall n-node tree, one-node tree.
    auto [tall_keys, short_keys] = adversary::unequal_join(n);
    Avl left, right;
    for (auto e : tall_keys) left.insert(e);
    for (auto e : short_keys) right.insert(e);
    Entry pivot{1, static_cast<std::int64_t>(n)};
    std::vector<Entry> all = tall_keys;
    all.push_back(pivot);
    all.insert(all.end(), short_keys.begin(), short_keys.end());
    all = sorted_keys(all);
    auto joined = Avl::join(std::move(left), pivot, std::move(right));
    require(joined.audit() == n + 2, "join size");
    require_partition(all, joined, nullptr, Avl{}, "unequal join");
    auto [high, exact, low] = joined.split(pivot);
    require(exact && high.audit() == n && low.audit() == 1, "split failure");
    require_partition(all, high, &*exact, low, "unequal split");
    auto rejoined = Avl::join(std::move(high), *exact, std::move(low));
    require(rejoined.audit() == n + 2, "rejoin failure");
    auto [above, below] = rejoined.split_score(static_cast<std::int64_t>(n));
    require(above.audit() + below.audit() == n + 2, "score split failure");
    require_partition(all, above, nullptr, below, "score split");
    for (auto e : above.entries()) require(e.score > static_cast<std::int64_t>(n), "score split: low key above");
    for (auto e : below.entries()) require(e.score <= static_cast<std::int64_t>(n), "score split: high key below");

    // 3. Joins at several height gaps, with the taller tree on either side.
    std::size_t half = n / 2, small = std::max<std::size_t>(1, n / 16);
    std::vector<std::pair<std::size_t, std::size_t>> shapes = {
        {n, 1}, {1, n}, {n, half}, {half, n}, {n, small}, {small, n},
        {n, n}, {n, 0}, {0, n}, {0, 0}};
    for (auto [l, r] : shapes) check_join(l, r);

    // 4. Random pivot splits and random score splits on the sorted-insert tree.
    auto ordered = sorted_keys(inserts);
    std::mt19937_64 rng(253);
    for (int trial = 0; trial < 100; ++trial) {
        Avl sample;
        for (auto e : inserts) sample.insert(e);
        auto key = inserts[rng() % n];
        auto [left_part, found, right_part] = sample.split(key);
        require(found && left_part.audit() + right_part.audit() + 1 == n,
                "random pivot split failure");
        require_partition(ordered, left_part, &*found, right_part, "random pivot split");
        auto restored = Avl::join(std::move(left_part), *found, std::move(right_part));
        require(restored.audit() == n, "random rejoin failure");
        require_partition(ordered, restored, nullptr, Avl{}, "random rejoin");

        auto threshold = static_cast<std::int64_t>(rng() % (n + 1)) - 1;  // -1..n-1
        auto [hi, lo] = restored.split_score(threshold);
        hi.audit(); lo.audit();
        require_partition(ordered, hi, nullptr, lo, "random score split");
        require(hi.size() == static_cast<std::size_t>(static_cast<std::int64_t>(n) - 1 - threshold),
                "random score split count");
    }

    // Sorted inserts of n keys should rotate n - bit_length(n) times (insert k rotates
    // unless k is a power of two). Reported, not enforced: it is the D5 claim to prove.
    auto predicted = n - static_cast<std::size_t>(bit_length(n));
    std::printf("PASS n=%zu  insert rotations %llu (%.4f per insert, predicted %zu: %s)"
                "  delete rotations %llu (%.4f per delete; one-end order %llu, %.4f)"
                "  joins %zu shapes\n",
                n, static_cast<unsigned long long>(insert_rotations),
                static_cast<double>(insert_rotations) / n, predicted,
                insert_rotations == predicted ? "match" : "DIFFERS",
                static_cast<unsigned long long>(delete_rotations),
                static_cast<double>(delete_rotations) / n,
                static_cast<unsigned long long>(one_end_rotations),
                static_cast<double>(one_end_rotations) / n, shapes.size());
}

int main(int argc, char** argv) {
    try {
        // Usage: adversarial [n ...]; default 500. Each size audits after every
        // operation, so the cost grows as n^2: sizes above ~20000 are slow.
        std::vector<std::size_t> sizes;
        for (int i = 1; i < argc; ++i) sizes.push_back(std::stoull(argv[i]));
        if (sizes.empty()) sizes.push_back(500);
        check_join_rejects_overlap();
        std::printf("PASS join rejects overlapping keys and leaves its inputs intact\n");
        for (auto n : sizes) {
            if (n == 0) throw std::invalid_argument("n must be positive");
            run(n);
        }
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "adversary error: " << e.what() << '\n'; return 1;
    }
}
