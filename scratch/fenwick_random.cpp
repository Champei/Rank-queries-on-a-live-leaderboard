// Run from the project root; see members/02-structure-b/README.md.
// Reuse the practice functions, giving their example main another name.
#define main practice_example_main
#include "mine.cpp"
#undef main
#include "src/structB/fenwick.hpp"
#include <random>
#include <stdexcept>

void check(bool ok, string message) {
    if (!ok) throw runtime_error(message);
}

int main() {
    mt19937 rng(253);
    ll operations = 0;
    try {
        for(ll test=0; test<300; test++) {
            vector<ll> universe;
            ll n = 1 + rng()%20;
            for(ll i=0; i<n; i++) universe.push_back((ll)(rng()%41)-20);
            init(universe);
            leaderboard::Fenwick f(universe);
            vector<ll> players; // Plain sorted-array oracle, with duplicates.
            for(ll op=0; op<400; op++) {
                if (players.empty() || rng()%2 == 0) {
                    ll score = universe[rng()%universe.size()];
                    check(update(score, 1), "practice insertion");
                    f.update(score, 1);
                    players.push_back(score);
                } else {
                    ll i = rng()%players.size();
                    ll score = players[i];
                    check(update(score, -1), "practice deletion");
                    f.update(score, -1);
                    players.erase(players.begin()+i);
                }
                sort(players.begin(), players.end(), greater<ll>());
                check(f.size() == (ll)players.size(), "class size");
                check(total == (ll)players.size(), "practice size");
                check(audit(), "practice audit");
                check(f.audit() == players.size(), "class audit");
                for(ll i=0; i<(ll)scores.size(); i++) {
                    ll expected = 1;
                    for(ll j=0; j<(ll)players.size(); j++) {
                        if (players[j] > scores[i]) expected++;
                    }
                    check(rank_of(scores[i]) == expected, "practice rank");
                    check(f.rank(scores[i]) == (size_t)expected, "class rank");
                }
                for(ll p=1; p<=(ll)players.size(); p++) {
                    check(select_score(p) == players[p-1], "practice select");
                    check(f.select_score(p) == players[p-1], "class select");
                }
                operations++;
            }
        }
        leaderboard::Fenwick empty({});
        check(empty.audit() == 0, "empty universe audit");
        check(!empty.accepts(10), "empty universe accepts");
        bool rejected = false;
        try { empty.select_score(1); }
        catch (const out_of_range&) { rejected = true; }
        check(rejected, "empty select must fail");
        auto f = leaderboard::Fenwick::rebuild({50,10,30,10,40,20},
                                              {10,30,40,40,10,40});
        check(f.audit() == 6 && f.rank(30) == 4 && f.select_score(3) == 40,
              "rebuild example");
        check(f.rank(35) == 4 && !f.accepts(35), "unseen query score");
        for(ll p=-1; p<=7; p++) {
            if (p>=1 && p<=6) continue;
            rejected = false;
            try { f.select_score(p); }
            catch (const out_of_range&) { rejected = true; }
            check(rejected, "invalid position must fail");
        }
        rejected = false;
        try { f.update(35, 1); }
        catch (const out_of_range&) { rejected = true; }
        check(rejected && f.audit() == 6, "unseen update must fail safely");
        rejected = false;
        try { f.update(20, -1); }
        catch (const out_of_range&) { rejected = true; }
        check(rejected && f.audit() == 6, "negative count must fail safely");
        cout << "PASS " << operations
             << " random updates (300 universes x 400), seed 253; edge cases passed\n";
    } catch (const exception& e) {
        cerr << "FAIL after " << operations << " random updates: " << e.what() << '\n';
        return 1;
    }
}
