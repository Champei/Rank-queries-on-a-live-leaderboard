#pragma once
#include "app/player_hash.hpp"
#include "src/structA/avl.hpp"
#include "src/structB/fenwick.hpp"
#include <cstdint>
#include <utility>
#include <vector>

namespace leaderboard {
class Leaderboard {
public:
    explicit Leaderboard(std::vector<std::int64_t> score_universe);
    void revise(std::int64_t player_id, std::int64_t score);
    void remove(std::int64_t player_id);
    std::size_t rank(std::int64_t player_id) const;
    Entry select(std::size_t position) const;
    std::vector<Entry> positions(std::size_t first, std::size_t last) const;
    std::size_t size() const { return players_.size(); }
    std::size_t audit() const;
    std::pair<Leaderboard, Leaderboard> split(std::int64_t threshold);
    void merge(Leaderboard& other); // consumes other; IDs must be disjoint
    const Avl& avl() const { return avl_; }
    const Fenwick& fenwick() const { return fenwick_; }
private:
    std::vector<std::int64_t> universe_;
    PlayerHash players_;
    Avl avl_;
    Fenwick fenwick_;
    static Leaderboard from_tree(const std::vector<std::int64_t>& universe,
                                  Avl&& tree);
};
}
