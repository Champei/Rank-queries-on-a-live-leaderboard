#include "app/player_hash.hpp"
#include <cstdint>
#include <stdexcept>

using namespace std;

namespace leaderboard {
PlayerHash::PlayerHash() : cells_(8) {}
pair<size_t, bool> PlayerHash::slot(ll id) const {
    auto mask = cells_.size() - 1;
    auto at = (static_cast<uint64_t>(id) * 11400714819323198485ull) & mask;
    size_t tomb = cells_.size();
    while (cells_[at].state != 0) {
        if (cells_[at].state == 1 && cells_[at].id == id) return {at, true};
        if (cells_[at].state == 2 && tomb == cells_.size()) tomb = at;
        at = (at + 1) & mask;
    }
    return {tomb == cells_.size() ? at : tomb, false};
}
void PlayerHash::resize(size_t capacity) {
    auto old = move(cells_);
    cells_.assign(capacity, {});
    live_ = occupied_ = 0;
    for (const auto& cell : old) if (cell.state == 1) put(cell.id, cell.score);
}
bool PlayerHash::contains(ll id) const { return slot(id).second; }
ll PlayerHash::get(ll id) const {
    auto [at, found] = slot(id);
    if (!found) throw out_of_range("unknown player ID");
    return cells_[at].score;
}
void PlayerHash::put(ll id, ll score) {
    if (id < 0) throw invalid_argument("negative player ID");
    if ((occupied_ + 1) * 10 >= cells_.size() * 7) resize(cells_.size() * 2);
    auto [at, found] = slot(id);
    if (!found) {
        if (cells_[at].state == 0) ++occupied_;
        ++live_;
    }
    cells_[at] = {id, score, 1};
}
void PlayerHash::erase(ll id) {
    auto [at, found] = slot(id);
    if (!found) throw out_of_range("unknown player ID");
    cells_[at].state = 2; --live_;
    if (cells_.size() > 8 && live_ * 4 < cells_.size()) resize(cells_.size() / 2);
    else if (occupied_ > 2 * live_ + 8) resize(cells_.size());
}
vector<pair<ll, ll>> PlayerHash::entries() const {
    vector<pair<ll, ll>> out;
    out.reserve(live_);
    for (const auto& c : cells_) if (c.state == 1) out.emplace_back(c.id, c.score);
    return out;
}
size_t PlayerHash::audit() const {
    size_t found = 0;
    for (const auto& c : cells_) if (c.state == 1) {
        if (get(c.id) != c.score) throw logic_error("hash lookup");
        ++found;
    }
    if (found != live_) throw logic_error("hash size");
    return found;
}
}
