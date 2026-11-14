// A small vector-backed AVL reference model for practicing order-statistic queries.
// The production tree remains in src/structA/avl.cpp; this intentionally favors clarity.
#include "src/structA/avl.hpp"
#include <algorithm>
#include <iostream>
#include <stdexcept>

class PracticeAvl {
public:
    void insert(leaderboard::Entry entry) {
        auto it = std::lower_bound(entries_.begin(), entries_.end(), entry, before);
        if (it != entries_.end() && same(*it, entry))
            throw std::invalid_argument("duplicate pair key");
        entries_.insert(it, entry);
    }

    void erase(leaderboard::Entry entry) {
        auto it = std::lower_bound(entries_.begin(), entries_.end(), entry, before);
        if (it == entries_.end() || !same(*it, entry))
            throw std::out_of_range("missing pair key");
        entries_.erase(it);
    }

    std::size_t size() const { return entries_.size(); }

    std::size_t rank(std::int64_t score) const {
        return 1 + static_cast<std::size_t>(std::count_if(
            entries_.begin(), entries_.end(), [score](const auto& e) { return e.score > score; }));
    }

    leaderboard::Entry select(std::size_t position) const {
        if (position == 0 || position > entries_.size())
            throw std::out_of_range("selection position");
        return entries_[position - 1];
    }

    std::vector<leaderboard::Entry> rank_range(std::size_t first, std::size_t last) const {
        if (first == 0 || last < first || last > entries_.size())
            throw std::out_of_range("rank range");
        return {entries_.begin() + static_cast<std::ptrdiff_t>(first - 1),
                entries_.begin() + static_cast<std::ptrdiff_t>(last)};
    }

    const std::vector<leaderboard::Entry>& entries() const { return entries_; }

    std::size_t audit() const {
        for (std::size_t i = 1; i < entries_.size(); ++i)
            if (!before(entries_[i - 1], entries_[i]))
                throw std::logic_error("reference order or uniqueness failure");
        return entries_.size();
    }

private:
    static bool before(const leaderboard::Entry& a, const leaderboard::Entry& b) {
        return a.score != b.score ? a.score > b.score : a.player_id < b.player_id;
    }
    static bool same(const leaderboard::Entry& a, const leaderboard::Entry& b) {
        return a.score == b.score && a.player_id == b.player_id;
    }
    std::vector<leaderboard::Entry> entries_;
};

int main() {
    PracticeAvl practice;
    practice.insert({30, 2});
    practice.insert({40, 1});
    practice.insert({30, 1});
    practice.insert({10, 7});
    std::cout << "practice entries: " << practice.size()
              << "; competition rank(30): " << practice.rank(30)
              << "; select(2): score " << practice.select(2).score
              << ", player " << practice.select(2).player_id << '\n';
    return 0;
}
