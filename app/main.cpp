#include "app/leaderboard.hpp"
#include <cstdint>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

using namespace leaderboard;
using namespace std;
int main(int argc, char** argv) {
    try {
        size_t players = 1000, revisions = 5000;
        if (argc > 1) players = stoull(argv[1]);
        if (argc > 2) revisions = stoull(argv[2]);
        if (players == 0) throw invalid_argument("players must be positive");
        vector<int64_t> scores;
        for (int s = 0; s <= 10000; ++s) scores.push_back(s);
        Leaderboard board(scores);
        mt19937_64 rng(253);
        for (size_t i = 0; i < players; ++i)
            board.revise(static_cast<int64_t>(i), rng() % 10001);
        for (size_t i = 0; i < revisions; ++i) {
            auto id = static_cast<int64_t>(rng() % players);
            board.revise(id, rng() % 10001);
            if (i % 10000 == 0) {
                auto rank = board.rank(id);
                auto e = board.select(1 + rng() % board.size());
                cout << "revision " << i << ": player " << id
                          << " competition rank " << rank << ", sampled "
                          << e.player_id << " score " << e.score << '\n';
            }
        }
        auto range = board.positions(40, 50);
        cout << "positions 40..50 (id:score):";
        for (const auto& e : range) cout << ' ' << e.player_id << ':' << e.score;
        cout << '\n';
        auto [high, low] = board.split(5000);
        cout << "season split >5000: " << high.size()
                  << ", <=5000: " << low.size() << '\n';
        high.merge(low);
        cout << "merged: " << high.audit() << " players\n";
        return 0;
    } catch (const exception& e) {
        cerr << "app error: " << e.what() << '\n'; return 1;
    }
}
