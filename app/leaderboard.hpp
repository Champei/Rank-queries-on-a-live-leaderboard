#pragma once
#include "app/player_hash.hpp"
#include "src/structA/avl.hpp"
#include "src/structB/fenwick.hpp"
#include <utility>
#include <vector>

namespace leaderboard {
using std::pair;
using std::size_t;
using std::vector;

class Leaderboard {
public:
    explicit Leaderboard(vector<ll> score_universe);
    void revise(ll player_id, ll score);
    void remove(ll player_id);
    size_t rank(ll player_id) const;
    Entry select(size_t position) const;
    vector<Entry> positions(size_t first, size_t last) const;
    size_t size() const { return players_.size(); }
    size_t audit() const;
    pair<Leaderboard, Leaderboard> split(ll threshold);
    void merge(Leaderboard& other); // consumes other; IDs must be disjoint
    const Avl& avl() const { return avl_; }
    const Fenwick& fenwick() const { return fenwick_; }
private:
    vector<ll> universe_;
    PlayerHash players_;
    Avl avl_;
    Fenwick fenwick_;
    static Leaderboard from_tree(const vector<ll>& universe,
                                  Avl&& tree);
};
}
