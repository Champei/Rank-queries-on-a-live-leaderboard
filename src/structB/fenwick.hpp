// Author: Teerth (250001027) - Structure B
#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>

namespace leaderboard {
using ll = long long;
using std::vector;

class Fenwick {
public:
    explicit Fenwick(vector<ll> score_universe);
    void update(ll score, int delta);
    // Match the AVL rank return type used by the application.
    std::size_t rank(ll score) const; // competition rank
    ll select_score(ll position) const; // descending, one based
    ll size() const { return total; }
    ll universe_size() const { return scores.size(); }
    bool accepts(ll score) const;
    std::uint64_t steps() const { return step_count; }
    std::size_t audit() const;
    static Fenwick rebuild(const vector<ll>& universe,
                           const vector<ll>& player_scores);
private:
    vector<ll> scores;
    vector<ll> bit;
    vector<ll> cnt;
    ll total = 0;
    mutable std::uint64_t step_count = 0;
    ll index(ll score) const;
    ll prefix(ll end) const;
};
}
