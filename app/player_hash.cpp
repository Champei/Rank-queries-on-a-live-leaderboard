#include "app/player_hash.hpp"
#include <stdexcept>

namespace leaderboard {
PlayerHash::PlayerHash() : cells_(8) {}
std::pair<std::size_t, bool> PlayerHash::slot(std::int64_t id) const {
    auto mask = cells_.size() - 1;
    auto at = (static_cast<std::uint64_t>(id) * 11400714819323198485ull) & mask;
    std::size_t tomb = cells_.size();
    while (cells_[at].state != 0) {
        if (cells_[at].state == 1 && cells_[at].id == id) return {at, true};
        if (cells_[at].state == 2 && tomb == cells_.size()) tomb = at;
        at = (at + 1) & mask;
    }
    return {tomb == cells_.size() ? at : tomb, false};
}
void PlayerHash::resize(std::size_t capacity) {
    auto old = std::move(cells_);
    cells_.assign(capacity, {});
    live_ = occupied_ = 0;
    for (const auto& cell : old) if (cell.state == 1) put(cell.id, cell.score);
}
bool PlayerHash::contains(std::int64_t id) const { return slot(id).second; }
std::int64_t PlayerHash::get(std::int64_t id) const {
    auto [at, found] = slot(id);
    if (!found) throw std::out_of_range("unknown player ID");
    return cells_[at].score;
}
void PlayerHash::put(std::int64_t id, std::int64_t score) {
    if (id < 0) throw std::invalid_argument("negative player ID");
    if ((occupied_ + 1) * 10 >= cells_.size() * 7) resize(cells_.size() * 2);
    auto [at, found] = slot(id);
    if (!found) {
        if (cells_[at].state == 0) ++occupied_;
        ++live_;
    }
    cells_[at] = {id, score, 1};
}
void PlayerHash::erase(std::int64_t id) {
    auto [at, found] = slot(id);
    if (!found) throw std::out_of_range("unknown player ID");
    cells_[at].state = 2; --live_;
    if (cells_.size() > 8 && live_ * 4 < cells_.size()) resize(cells_.size() / 2);
    else if (occupied_ > 2 * live_ + 8) resize(cells_.size());
}
std::vector<std::pair<std::int64_t, std::int64_t>> PlayerHash::entries() const {
    std::vector<std::pair<std::int64_t, std::int64_t>> out;
    out.reserve(live_);
    for (const auto& c : cells_) if (c.state == 1) out.emplace_back(c.id, c.score);
    return out;
}
std::size_t PlayerHash::audit() const {
    std::size_t found = 0;
    for (const auto& c : cells_) if (c.state == 1) {
        if (get(c.id) != c.score) throw std::logic_error("hash lookup");
        ++found;
    }
    if (found != live_) throw std::logic_error("hash size");
    return found;
}
}
