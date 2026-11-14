// Deterministic randomized checks for the Structure A AVL module.
// The historical filename is retained to match the requested scratch layout.
#define main practice_example_main
#include "scratch/mine.cpp"
#undef main
#include "src/structA/avl.hpp"
#include <algorithm>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>

using leaderboard::Avl;
using leaderboard::Entry;

static void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}
static bool same_entry(const Entry& a, const Entry& b) {
    return a.score == b.score && a.player_id == b.player_id;
}
static bool same_entries(const std::vector<Entry>& a, const std::vector<Entry>& b) {
    if (a.size() != b.size()) return false;
    for (std::size_t i = 0; i < a.size(); ++i)
        if (!same_entry(a[i], b[i])) return false;
    return true;
}
static void compare(const Avl& tree, const PracticeAvl& model) {
    require(tree.size() == model.size(), "size differs from reference");
    require(tree.audit() == model.audit(), "audit count");
    require(same_entries(tree.entries(), model.entries()), "in-order listing");
    for (int score = -22; score <= 22; score += 2)
        require(tree.rank(score) == model.rank(score), "competition rank");
    for (std::size_t pos = 1; pos <= model.size(); ++pos)
        require(same_entry(tree.select(pos), model.select(pos)), "select");
    if (model.size() >= 3)
        require(same_entries(tree.rank_range(2, model.size() - 1),
                             model.rank_range(2, model.size() - 1)), "rank range");
}
static void check_split_and_join(Avl& tree, const PracticeAvl& model) {
    if (model.size() < 3) return;
    const Entry pivot = model.select(model.size() / 2);
    auto [left, found, right] = tree.split(pivot);
    require(found && same_entry(*found, pivot), "split pivot");
    require(left.audit() + right.audit() + 1 == model.size(), "split sizes");
    Avl rebuilt = Avl::join(std::move(left), pivot, std::move(right));
    require(rebuilt.audit() == model.size(), "join after split");
    require(same_entries(rebuilt.entries(), model.entries()), "split/join order");
    auto [high, low] = rebuilt.split_score(0);
    std::vector<Entry> expected_high, expected_low;
    for (const auto& e : model.entries())
        (e.score > 0 ? expected_high : expected_low).push_back(e);
    require(same_entries(high.entries(), expected_high), "score split high side");
    require(same_entries(low.entries(), expected_low), "score split low side");
}
static void check_rejected_join() {
    Avl left, right;
    left.insert({5, 1}); right.insert({6, 2});
    bool rejected = false;
    try { auto bad = Avl::join(std::move(left), {4, 3}, std::move(right)); (void)bad; }
    catch (const std::invalid_argument&) { rejected = true; }
    require(rejected, "invalid join must be rejected");
    require(left.size() == 1 && right.size() == 1, "rejected join preserves inputs");
}
int main() {
    try {
        constexpr int rounds = 80;
        constexpr int operations_per_round = 500;
        constexpr int max_live = 120;
        std::mt19937_64 rng(253);
        std::uint64_t operation_count = 0;
        std::int64_t next_id = 0;
        for (int round = 0; round < rounds; ++round) {
            Avl tree;
            PracticeAvl model;
            std::vector<Entry> live;
            for (int op = 0; op < operations_per_round; ++op) {
                const bool add = live.empty() ||
                    (live.size() < max_live && (rng() % 100 < 57));
                if (add) {
                    Entry e{static_cast<std::int64_t>(rng() % 45) - 22, next_id++};
                    tree.insert(e); model.insert(e); live.push_back(e);
                } else {
                    const std::size_t i = static_cast<std::size_t>(rng() % live.size());
                    const Entry e = live[i];
                    tree.erase(e); model.erase(e);
                    live.erase(live.begin() + static_cast<std::ptrdiff_t>(i));
                }
                compare(tree, model);
                ++operation_count;
            }
            check_split_and_join(tree, model);
        }
        check_rejected_join();

        // Exercise duplicate/missing-key rejection and invalid one-based positions.
        Avl edge; edge.insert({7, 99});
        bool duplicate = false, missing = false, bad_select = false;
        try { edge.insert({7, 99}); } catch (const std::invalid_argument&) { duplicate = true; }
        try { edge.erase({7, 100}); } catch (const std::out_of_range&) { missing = true; }
        try { (void)edge.select(0); } catch (const std::out_of_range&) { bad_select = true; }
        require(duplicate && missing && bad_select, "invalid operation checks");
        edge.erase({7, 99});
        require(edge.audit() == 0, "erase to empty");

        std::cout << "PASS " << operation_count << " randomized updates (" << rounds
                  << " rounds x " << operations_per_round << "), seed 253; rank/select/range, "
                  << "audit, split/join, score split and edge cases passed\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "FAIL: " << e.what() << '\n';
        return 1;
    }
}
