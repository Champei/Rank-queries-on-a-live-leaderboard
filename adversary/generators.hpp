#pragma once
#include "src/structA/avl.hpp"
#include <cstddef>
#include <cstdint>
#include <tuple>
#include <vector>

namespace leaderboard::adversary {
std::vector<Entry> rotation_inserts(std::size_t n);
// Strongest delete order found for the rotation_inserts(n) tree.
std::vector<Entry> rotation_deletes(std::size_t n);
// Starter order (always delete the current last key), kept for comparison.
std::vector<Entry> rotation_deletes_one_end(std::size_t n);
std::pair<std::vector<Entry>, std::vector<Entry>> unequal_join(std::size_t n);
// Keys for an ordered join with chosen sizes: every left key precedes the pivot,
// and the pivot precedes every right key. Either side may be empty.
std::tuple<std::vector<Entry>, Entry, std::vector<Entry>>
ordered_join_case(std::size_t left_n, std::size_t right_n);
}
