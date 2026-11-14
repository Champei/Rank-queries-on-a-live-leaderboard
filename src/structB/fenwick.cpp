#include "src/structB/fenwick.hpp"
#include <algorithm>
#include <stdexcept>
#include <utility>

using namespace std;

namespace leaderboard {
Fenwick::Fenwick(vector<ll> score_universe)
    : scores(std::move(score_universe)) {
    sort(scores.begin(), scores.end());
    scores.erase(unique(scores.begin(), scores.end()), scores.end());
    bit.assign(scores.size() + 1, 0);
    cnt.assign(scores.size(), 0);
}
ll Fenwick::index(ll score) const {
    auto it = lower_bound(scores.begin(), scores.end(), score);
    if (it == scores.end() || *it != score)
        throw out_of_range("score outside Fenwick universe");
    return it - scores.begin();
}
bool Fenwick::accepts(ll score) const {
    return binary_search(scores.begin(), scores.end(), score);
}
ll Fenwick::prefix(ll end) const {
    ll sum = 0;
    for(ll i=end; i>0; i-=i & -i) {
        sum += bit[i];
        ++step_count;
    }
    return sum;
}
void Fenwick::update(ll score, int delta) {
    auto i = index(score);
    if (cnt[i] + delta < 0) throw out_of_range("negative score count");
    cnt[i] += delta;
    total += delta;
    for(ll x=i+1; x<(ll)bit.size(); x+=x & -x) {
        bit[x] += delta; ++step_count;
    }
}
std::size_t Fenwick::rank(ll score) const {
    ll end = upper_bound(scores.begin(), scores.end(), score) - scores.begin();
    return 1 + total - prefix(end);
}
ll Fenwick::select_score(ll position) const {
    if (position <= 0 || position > total) throw out_of_range("invalid position");
    ll target = total - position + 1;
    ll at = 0;
    int LOG = 0;
    while ((1LL << (LOG + 1)) <= (ll)scores.size()) LOG++;
    for(ll i=LOG; i>=0; i--) {
        ll step = 1LL << i;
        ll next = at + step;
        ++step_count;
        if (next <= (ll)scores.size() && bit[next] < target) {
            target -= bit[next];
            at = next;
        }
    }
    return scores[at];
}
std::size_t Fenwick::audit() const {
    ll running = 0;
    for(ll i=0; i<(ll)cnt.size(); i++) {
        if (cnt[i] < 0) throw logic_error("negative Fenwick count");
        running += cnt[i];
        if (prefix(i + 1) != running) throw logic_error("Fenwick prefix mismatch");
    }
    if (running != total)
        throw logic_error("Fenwick total mismatch");
    return total;
}
Fenwick Fenwick::rebuild(const vector<ll>& universe,
                         const vector<ll>& player_scores) {
    Fenwick result(universe);
    for(ll i=0; i<(ll)player_scores.size(); i++) {
        result.update(player_scores[i], 1);
    }
    return result;
}
}
