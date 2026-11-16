#pragma once
#include <cstddef>
#include <utility>
#include <vector>

namespace leaderboard {
using ll = long long;
using std::pair;
using std::size_t;
using std::vector;

// Open addressing with linear probing. No unordered_map in production.
class PlayerHash {
public:
    PlayerHash();
    size_t size() const { return live_; }
    bool contains(ll id) const;
    ll get(ll id) const;
    void put(ll id, ll score);
    void erase(ll id);
    vector<pair<ll, ll>> entries() const;
    size_t audit() const;
private:
    struct Cell { ll id = 0, score = 0; unsigned char state = 0; };
    vector<Cell> cells_;
    size_t live_ = 0, occupied_ = 0;
    pair<size_t, bool> slot(ll id) const;
    void resize(size_t capacity);
};
}
