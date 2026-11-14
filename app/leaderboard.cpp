#include "app/leaderboard.hpp"
#include <stdexcept>

namespace leaderboard {
Leaderboard::Leaderboard(std::vector<std::int64_t> universe)
    : universe_(std::move(universe)),
      fenwick_(std::vector<ll>(universe_.begin(), universe_.end())) {}
void Leaderboard::revise(std::int64_t id, std::int64_t score) {
    if (id < 0) throw std::invalid_argument("negative player ID");
    // Validate that Fenwick can represent the new score before any mutation.
    if (!fenwick_.accepts(score))
        throw std::invalid_argument("score absent from offline universe");
    if (players_.contains(id)) {
        auto old = players_.get(id);
        if (old == score) return;
        avl_.erase({old, id}); fenwick_.update(old, -1);
    }
    players_.put(id, score);
    avl_.insert({score, id}); fenwick_.update(score, 1);
}
void Leaderboard::remove(std::int64_t id) {
    auto old = players_.get(id);
    avl_.erase({old, id}); fenwick_.update(old, -1); players_.erase(id);
}
std::size_t Leaderboard::rank(std::int64_t id) const {
    auto score = players_.get(id);
    auto a = avl_.rank(score), b = fenwick_.rank(score);
    if (a != b) throw std::logic_error("structures disagree on rank");
    return a;
}
Entry Leaderboard::select(std::size_t pos) const {
    auto e = avl_.select(pos);
    if (fenwick_.select_score(pos) != e.score)
        throw std::logic_error("structures disagree on select score");
    return e;
}
std::vector<Entry> Leaderboard::positions(std::size_t first, std::size_t last) const {
    return avl_.rank_range(first, last);
}
std::size_t Leaderboard::audit() const {
    auto n = players_.audit();
    if (n != avl_.audit() || n != fenwick_.audit())
        throw std::logic_error("structure sizes disagree");
    for (const auto& e : avl_.entries()) {
        if (players_.get(e.player_id) != e.score ||
            avl_.rank(e.score) != fenwick_.rank(e.score))
            throw std::logic_error("structure entries disagree");
    }
    return n;
}
Leaderboard Leaderboard::from_tree(const std::vector<std::int64_t>& universe, Avl&& tree) {
    Leaderboard out(universe);
    out.avl_ = std::move(tree);
    for (const auto& e : out.avl_.entries()) {
        out.players_.put(e.player_id, e.score);
        out.fenwick_.update(e.score, 1);
    }
    return out;
}
std::pair<Leaderboard, Leaderboard> Leaderboard::split(std::int64_t threshold) {
    auto [high, low] = avl_.split_score(threshold);
    auto a = from_tree(universe_, std::move(high));
    auto b = from_tree(universe_, std::move(low));
    players_ = PlayerHash();
    fenwick_ = Fenwick(std::vector<ll>(universe_.begin(), universe_.end()));
    return {std::move(a), std::move(b)};
}
void Leaderboard::merge(Leaderboard& other) {
    for (const auto& [id, score] : other.players_.entries()) {
        if (players_.contains(id)) throw std::invalid_argument("overlapping player ID");
        if (!fenwick_.accepts(score)) throw std::invalid_argument("incompatible score universe");
    }
    for (const auto& [id, score] : other.players_.entries()) {
        revise(id, score);
    }
    other.players_ = PlayerHash();
    other.avl_ = Avl();
    other.fenwick_ = Fenwick(std::vector<ll>(other.universe_.begin(),
                                             other.universe_.end()));
}
}
