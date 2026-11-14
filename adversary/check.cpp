#include "adversary/generators.hpp"
#include "src/structA/avl.hpp"
#include <algorithm>
#include <cstdint>
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
// Full audit; on failure, say which phase and step broke the tree.
static std::size_t audit_at(const Avl& tree, const char* phase, std::size_t step) {
    try {
        return tree.audit();
    } catch (const std::exception& e) {
        throw std::logic_error(std::string(phase) + " step " + std::to_string(step) + ": " + e.what());
    }
}
static std::size_t audit_at(const Avl& tree, const std::string& phase) {
    try {
        return tree.audit();
    } catch (const std::exception& e) {
        throw std::logic_error(phase + ": " + e.what());
    }
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
// Insert in a shuffled order so the tree is not perfectly shaped along its spines.
static void insert_shuffled(Avl& tree, std::vector<Entry> keys, std::mt19937_64& rng) {
    std::shuffle(keys.begin(), keys.end(), rng);
    for (auto e : keys) tree.insert(e);
}

// Ordered join with chosen sizes, then split at the pivot and rejoin.
// Returns the number of rotations performed inside the join itself.
static std::uint64_t check_join(std::size_t left_n, std::size_t right_n, std::mt19937_64& rng) {
    auto [left_keys, pivot, right_keys] = adversary::ordered_join_case(left_n, right_n);
    std::string name = "join " + std::to_string(left_n) + "+" + std::to_string(right_n);
    Avl left, right;
    insert_shuffled(left, left_keys, rng);
    insert_shuffled(right, right_keys, rng);
    std::vector<Entry> expected = sorted_keys(left_keys);
    expected.push_back(pivot);
    auto right_sorted = sorted_keys(right_keys);
    expected.insert(expected.end(), right_sorted.begin(), right_sorted.end());

    auto rotations_before = left.rotations() + right.rotations();
    auto joined = Avl::join(std::move(left), pivot, std::move(right));
    auto join_rotations = joined.rotations() - rotations_before;
    require(audit_at(joined, name) == left_n + right_n + 1, name + ": size");
    require_partition(expected, joined, nullptr, Avl{}, name);
    auto [high, found, low] = joined.split(pivot);
    require(found && same(*found, pivot), name + ": pivot not found by split");
    audit_at(high, name + " split, first part");
    audit_at(low, name + " split, second part");
    require_partition(expected, high, &*found, low, name + " split");
    auto rejoined = Avl::join(std::move(high), *found, std::move(low));
    require(audit_at(rejoined, name + " rejoin") == expected.size(), name + ": rejoin size");
    require_partition(expected, rejoined, nullptr, Avl{}, name + " rejoin");
    return join_rotations;
}

// Join must refuse keys on the wrong side of the pivot and leave both inputs intact,
// and must accept tied scores that the ID tie-break puts on the correct side.
static void check_join_boundaries() {
    auto build = [](const std::vector<Entry>& keys) {
        Avl tree;
        for (auto e : keys) tree.insert(e);
        return tree;
    };
    auto expect_rejected = [&](std::vector<Entry> left_keys, Entry pivot,
                               std::vector<Entry> right_keys, const std::string& what) {
        Avl left = build(left_keys), right = build(right_keys);
        bool rejected = false;
        try { Avl::join(std::move(left), pivot, std::move(right)); }
        catch (const std::invalid_argument&) { rejected = true; }
        require(rejected, what + ": join accepted it");
        require(audit_at(left, what + ", left input after rejection") == left_keys.size() &&
                audit_at(right, what + ", right input after rejection") == right_keys.size(),
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
    // Tied scores: the lower ID comes first, so (5,3) belongs after pivot (5,2).
    expect_rejected({{5, 3}}, {5, 2}, {}, "tied left key with a higher ID than the pivot");
    expect_rejected({}, {5, 2}, {{5, 1}}, "tied right key with a lower ID than the pivot");

    // A valid join where every score ties: (5,1) < (5,2) < (5,3) by ID.
    Avl joined;
    try {
        joined = Avl::join(build({{5, 1}}), {5, 2}, build({{5, 3}}));
    } catch (const std::exception& e) {
        throw std::logic_error(std::string("tied join: a valid join was rejected (") + e.what() + ")");
    }
    require(audit_at(joined, "tied join") == 3, "tied join: size");
    auto keys = joined.entries();
    require(keys[0].player_id == 1 && keys[1].player_id == 2 && keys[2].player_id == 3,
            "tied join: wrong key order");
}

static void run(std::size_t n) {
    // 1. Rotation stress: audit the whole tree after every insert and every delete.
    Avl tree;
    auto inserts = adversary::rotation_inserts(n);
    for (std::size_t i = 0; i < n; ++i) { tree.insert(inserts[i]); audit_at(tree, "rotation_inserts", i); }
    auto insert_rotations = tree.rotations();
    auto deletes = adversary::rotation_deletes(n);
    for (std::size_t i = 0; i < n; ++i) { tree.erase(deletes[i]); audit_at(tree, "rotation_deletes", i); }
    require(tree.size() == 0, "rotation_deletes did not empty the tree");
    auto delete_rotations = tree.rotations() - insert_rotations;

    // Same tree, starter delete order, for comparison.
    Avl baseline;
    for (auto e : inserts) baseline.insert(e);
    auto baseline_start = baseline.rotations();
    auto one_end = adversary::rotation_deletes_one_end(n);
    for (std::size_t i = 0; i < n; ++i) { baseline.erase(one_end[i]); audit_at(baseline, "one-end deletes", i); }
    require(baseline.size() == 0, "one-end deletes did not empty the tree");
    auto one_end_rotations = baseline.rotations() - baseline_start;

    // Both orders above delete near the edges, where a node rarely has two children.
    // These two delete from the middle, so erase has to fetch a successor from deep in
    // the right subtree (pop_first) and rebalance on the way back.
    {
        Avl middle;
        for (auto e : inserts) middle.insert(e);
        for (std::size_t i = 0; middle.size(); ++i) {
            middle.erase(middle.select(middle.size() / 2 + 1));
            audit_at(middle, "middle-key deletes", i);
        }
        Avl shuffled;
        for (auto e : inserts) shuffled.insert(e);
        auto order = inserts;
        std::mt19937_64 shuffle_rng(253);
        std::shuffle(order.begin(), order.end(), shuffle_rng);
        for (std::size_t i = 0; i < n; ++i) { shuffled.erase(order[i]); audit_at(shuffled, "random-order deletes", i); }
        require(shuffled.size() == 0, "random-order deletes did not empty the tree");
    }

    // 2. Tied scores: every key has score 0, so only the ID tie-break orders them.
    {
        auto tied = adversary::tied_inserts(n);
        Avl ties;
        for (std::size_t i = 0; i < n; ++i) { ties.insert(tied[i]); audit_at(ties, "tied inserts", i); }
        require(ties.rotations() == n - static_cast<std::size_t>(bit_length(n)),
                "tied inserts: rotation count differs from the untied family");
        auto ordered_ties = sorted_keys(tied);
        require_partition(ordered_ties, ties, nullptr, Avl{}, "tied tree order");
        auto [none_above, all_below] = ties.split_score(0);  // scores > 0 | scores <= 0
        require(none_above.size() == 0, "tied score split at 0: keys above");
        require_partition(ordered_ties, none_above, nullptr, all_below, "tied score split at 0");
        Avl again;
        for (auto e : tied) again.insert(e);
        auto [all_above, none_below] = again.split_score(-1);  // every score 0 is > -1
        require(none_below.size() == 0, "tied score split at -1: keys below");
        require_partition(ordered_ties, all_above, nullptr, none_below, "tied score split at -1");
        std::mt19937_64 tie_rng(253);
        std::shuffle(tied.begin(), tied.end(), tie_rng);
        for (std::size_t i = 0; i < n; ++i) { all_above.erase(tied[i]); audit_at(all_above, "tied deletes", i); }
        require(all_above.size() == 0, "tied deletes did not empty the tree");
    }

    // 3. Original unequal-height join: tall n-node tree, one-node tree.
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
    require(audit_at(joined, "unequal join") == n + 2, "unequal join: size");
    require_partition(all, joined, nullptr, Avl{}, "unequal join");
    auto [high, exact, low] = joined.split(pivot);
    require(exact && audit_at(high, "unequal split") == n && audit_at(low, "unequal split") == 1,
            "unequal split: sizes or pivot");
    require_partition(all, high, &*exact, low, "unequal split");
    auto rejoined = Avl::join(std::move(high), *exact, std::move(low));
    require(audit_at(rejoined, "unequal rejoin") == n + 2, "unequal rejoin: size");
    auto [above, below] = rejoined.split_score(static_cast<std::int64_t>(n));
    require(audit_at(above, "score split") + audit_at(below, "score split") == n + 2, "score split: sizes");
    require_partition(all, above, nullptr, below, "score split");
    for (auto e : above.entries()) require(e.score > static_cast<std::int64_t>(n), "score split: low key above");
    for (auto e : below.entries()) require(e.score <= static_cast<std::int64_t>(n), "score split: high key below");

    // 4. Joins at several height gaps, with the taller tree on either side. The input
    // trees are built in shuffled order so the joins also have to rotate.
    std::size_t quarter = std::max<std::size_t>(1, n / 4), small = std::max<std::size_t>(1, n / 16);
    std::vector<std::pair<std::size_t, std::size_t>> shapes = {
        {n, 1}, {1, n}, {n, quarter}, {quarter, n}, {n, small}, {small, n},
        {n, n}, {n, 0}, {0, n}, {0, 0}};
    std::mt19937_64 join_rng(253);
    std::uint64_t join_rotations = 0;
    for (auto [l, r] : shapes) join_rotations += check_join(l, r, join_rng);
    // 100 more joins with random sizes, so join's rebalancing is exercised on both sides.
    std::uint64_t larger_left = 0, larger_right = 0;
    for (int trial = 0; trial < 100; ++trial) {
        std::size_t l = join_rng() % (n + 1), r = join_rng() % (n + 1);
        (l > r ? larger_left : larger_right) += check_join(l, r, join_rng);
    }

    // 5. Random pivot splits and random score splits on the sorted-insert tree.
    auto ordered = sorted_keys(inserts);
    std::mt19937_64 rng(253);
    for (int trial = 0; trial < 100; ++trial) {
        Avl sample;
        for (auto e : inserts) sample.insert(e);
        auto key = inserts[rng() % n];
        auto [left_part, found, right_part] = sample.split(key);
        require(found && audit_at(left_part, "random pivot split") + audit_at(right_part, "random pivot split") + 1 == n,
                "random pivot split: sizes or pivot");
        require_partition(ordered, left_part, &*found, right_part, "random pivot split");
        auto restored = Avl::join(std::move(left_part), *found, std::move(right_part));
        require(audit_at(restored, "random rejoin") == n, "random rejoin: size");
        require_partition(ordered, restored, nullptr, Avl{}, "random rejoin");

        auto threshold = static_cast<std::int64_t>(rng() % (n + 1)) - 1;  // -1..n-1
        auto [hi, lo] = restored.split_score(threshold);
        audit_at(hi, "random score split"); audit_at(lo, "random score split");
        require_partition(ordered, hi, nullptr, lo, "random score split");
        require(hi.size() == static_cast<std::size_t>(static_cast<std::int64_t>(n) - 1 - threshold),
                "random score split count");
    }

    // D5 Theorem 1: sorted inserts of n keys rotate exactly n - bit_length(n) times
    // (insert k rotates unless k is a power of two). A different count means the rotation
    // counter or the balancing rule no longer matches the proof.
    auto predicted = n - static_cast<std::size_t>(bit_length(n));
    require(insert_rotations == predicted,
            "insert rotations " + std::to_string(insert_rotations) + " differ from n - bitlen(n) = "
            + std::to_string(predicted) + " (D5 Theorem 1)");
    std::printf("PASS n=%zu  insert rotations %llu (%.4f per insert, = n - bitlen(n))"
                "  delete rotations %llu (%.4f per delete; one-end order %llu, %.4f)"
                "  tied keys ok  join shapes %zu (%llu rotations inside joins)"
                "  random joins 100 (rotations inside: larger-left %llu, larger-right %llu)\n",
                n, static_cast<unsigned long long>(insert_rotations),
                static_cast<double>(insert_rotations) / static_cast<double>(n),
                static_cast<unsigned long long>(delete_rotations),
                static_cast<double>(delete_rotations) / static_cast<double>(n),
                static_cast<unsigned long long>(one_end_rotations),
                static_cast<double>(one_end_rotations) / static_cast<double>(n),
                shapes.size(), static_cast<unsigned long long>(join_rotations),
                static_cast<unsigned long long>(larger_left),
                static_cast<unsigned long long>(larger_right));
}

int main(int argc, char** argv) {
    try {
        // Usage: adversarial [n ...]; default 500. Each size audits after every
        // operation, so the cost grows as n^2: sizes above ~20000 are slow.
        std::vector<std::size_t> sizes;
        for (int i = 1; i < argc; ++i) sizes.push_back(std::stoull(argv[i]));
        if (sizes.empty()) sizes.push_back(500);
        check_join_boundaries();
        std::printf("PASS join rejects keys on the wrong side (including tied scores), "
                    "accepts a valid tied join, and leaves rejected inputs intact\n");
        for (auto n : sizes) {
            if (n == 0) throw std::invalid_argument("n must be positive");
            run(n);
        }
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "adversary error: " << e.what() << '\n'; return 1;
    }
}
