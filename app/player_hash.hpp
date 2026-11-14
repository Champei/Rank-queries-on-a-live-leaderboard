#pragma once
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

namespace leaderboard {
using std::int64_t;
using std::pair;
using std::size_t;
using std::vector;

// Open addressing with linear probing. No unordered_map in production.
class PlayerHash {
public:
    PlayerHash();
    size_t size() const { return live_; }
    bool contains(int64_t id) const;
    int64_t get(int64_t id) const;
    void put(int64_t id, int64_t score);
    void erase(int64_t id);
    vector<pair<int64_t, int64_t>> entries() const;
    size_t audit() const;
private:
    struct Cell { int64_t id = 0, score = 0; unsigned char state = 0; };
    vector<Cell> cells_;
    size_t live_ = 0, occupied_ = 0;
    pair<size_t, bool> slot(int64_t id) const;
    void resize(size_t capacity);
};
}
