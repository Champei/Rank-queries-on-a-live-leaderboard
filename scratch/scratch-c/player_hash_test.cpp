#include "app/player_hash.hpp"
#include <cassert>
#include <cstdint>
#include <random>
#include <stdexcept>
#include <unordered_map>

int main() {
    leaderboard::PlayerHash hash;
    std::unordered_map<std::int64_t, std::int64_t> expected;
    std::mt19937_64 rng(253);
    for (int i = 0; i < 100000; ++i) {
        auto id = static_cast<std::int64_t>(rng() % 2000);
        if (rng() % 3 == 0 && expected.count(id)) {
            hash.erase(id);
            expected.erase(id);
        } else {
            auto score = static_cast<std::int64_t>(rng() % 10001);
            hash.put(id, score);
            expected[id] = score;
        }
        assert(hash.size() == expected.size());
        assert(hash.contains(id) == (expected.count(id) != 0));
        if (expected.count(id)) assert(hash.get(id) == expected.at(id));
        if (i % 1000 == 0) assert(hash.audit() == expected.size());
    }
    for (const auto& [id, score] : expected) assert(hash.get(id) == score);
    assert(hash.audit() == expected.size());
    bool missing = false;
    try { hash.get(-1); } catch (const std::out_of_range&) { missing = true; }
    assert(missing);
    bool invalid = false;
    try { hash.put(-1, 0); } catch (const std::invalid_argument&) { invalid = true; }
    assert(invalid);
}
