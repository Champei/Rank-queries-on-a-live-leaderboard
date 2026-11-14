#pragma once
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

namespace leaderboard {
// Open addressing with linear probing. No std::unordered_map in production.
class PlayerHash {
public:
    PlayerHash();
    std::size_t size() const { return live_; }
    bool contains(std::int64_t id) const;
    std::int64_t get(std::int64_t id) const;
    void put(std::int64_t id, std::int64_t score);
    void erase(std::int64_t id);
    std::vector<std::pair<std::int64_t, std::int64_t>> entries() const;
    std::size_t audit() const;
private:
    struct Cell { std::int64_t id = 0, score = 0; unsigned char state = 0; };
    std::vector<Cell> cells_;
    std::size_t live_ = 0, occupied_ = 0;
    std::pair<std::size_t, bool> slot(std::int64_t id) const;
    void resize(std::size_t capacity);
};
}
