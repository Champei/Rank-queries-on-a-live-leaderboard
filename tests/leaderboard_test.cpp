#include "include/leaderboard.hpp"
#include <cassert>
#include <stdexcept>

int main() {
    leaderboard::Leaderboard board({70, 80, 90});
    board.revise(7, 90);
    board.revise(9, 80);
    board.revise(2, 80);
    board.revise(4, 70);
    assert(board.audit() == 4);
    assert(board.rank(2) == 2 && board.rank(9) == 2);
    assert(board.select(2).player_id == 2 && board.select(3).player_id == 9);
    assert(board.positions(2, 3).size() == 2);

    board.revise(9, 90);
    assert(board.rank(9) == 1 && board.rank(2) == 3);
    assert(board.audit() == 4);
    try { board.revise(9, 85); assert(false); }
    catch (const std::invalid_argument&) {}
    assert(board.audit() == 4 && board.select(1).player_id == 7);

    auto [high, low] = board.split(80);
    assert(board.size() == 0 && high.audit() == 2 && low.audit() == 2);
    high.merge(low);
    assert(high.audit() == 4 && low.size() == 0);

    leaderboard::Leaderboard overlap({70, 80, 90});
    overlap.revise(7, 70);
    try { high.merge(overlap); assert(false); }
    catch (const std::invalid_argument&) {}
    assert(high.audit() == 4 && overlap.audit() == 1);

    leaderboard::Leaderboard incompatible({70, 80, 95});
    incompatible.revise(11, 95);
    try { high.merge(incompatible); assert(false); }
    catch (const std::invalid_argument&) {}
    assert(high.audit() == 4 && incompatible.audit() == 1);
    high.remove(4);
    assert(high.audit() == 3);
}
