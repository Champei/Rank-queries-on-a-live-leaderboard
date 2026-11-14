#include "app/leaderboard.hpp"
#include <cstdint>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

using namespace leaderboard;
int main(int argc, char** argv) {
    try {
        std::size_t players = 1000, revisions = 5000;
        if (argc > 1) players = std::stoull(argv[1]);
        if (argc > 2) revisions = std::stoull(argv[2]);
        if (players == 0) throw std::invalid_argument("players must be positive");
        std::vector<std::int64_t> scores;
        for (int s = 0; s <= 10000; ++s) scores.push_back(s);
        Leaderboard board(scores);
        std::mt19937_64 rng(253);
        for (std::size_t i = 0; i < players; ++i)
            board.revise(static_cast<std::int64_t>(i), rng() % 10001);
        for (std::size_t i = 0; i < revisions; ++i) {
            auto id = static_cast<std::int64_t>(rng() % players);
            board.revise(id, rng() % 10001);
            if (i % 10000 == 0) {
                auto rank = board.rank(id);
                auto e = board.select(1 + rng() % board.size());
                std::cout << "revision " << i << ": player " << id
                          << " competition rank " << rank << ", sampled "
                          << e.player_id << " score " << e.score << '\n';
            }
        }
        auto range = board.positions(40, 50);
        std::cout << "positions 40..50 (id:score):";
        for (const auto& e : range) std::cout << ' ' << e.player_id << ':' << e.score;
        std::cout << '\n';
        auto [high, low] = board.split(5000);
        std::cout << "season split >5000: " << high.size()
                  << ", <=5000: " << low.size() << '\n';
        high.merge(low);
        std::cout << "merged: " << high.audit() << " players\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "app error: " << e.what() << '\n'; return 1;
    }
}
