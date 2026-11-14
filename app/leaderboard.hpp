#pragma once
#include "app/player_hash.hpp"
#include "src/structA/avl.hpp"
#include "src/structB/fenwick.hpp"
#include <cstdint>
#include <utility>
#include <vector>

namespace leaderboard {
using std::int64_t;
using std::pair;
using std::size_t;
using std::vector;

class Leaderboard {
public:
    explicit Leaderboard(vector<int64_t> score_universe);
    void revise(int64_t player_id, int64_t score);
    void remove(int64_t player_id);
    size_t rank(int64_t player_id) const;
    Entry select(size_t position) const;
    vector<Entry> positions(size_t first, size_t last) const;
    size_t size() const { return players_.size(); }
    size_t audit() const;
    pair<Leaderboard, Leaderboard> split(int64_t threshold);
    void merge(Leaderboard& other); // consumes other; IDs must be disjoint
    const Avl& avl() const { return avl_; }
    const Fenwick& fenwick() const { return fenwick_; }
private:
    vector<int64_t> universe_;
    PlayerHash players_;
    Avl avl_;
    Fenwick fenwick_;
    static Leaderboard from_tree(const vector<int64_t>& universe,
                                  Avl&& tree);
};
}
