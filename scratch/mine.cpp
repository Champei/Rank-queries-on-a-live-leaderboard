#include <bits/stdc++.h>
#include <iostream>
#include <map>
#include <set>
#include <bits/stdc++.h>
using namespace std;
using ll = long long;

// Data
vector<ll> scores;   // sorted, unique list of allowed scores   (0-based)
vector<ll> cnt;      // cnt[i]  = players having scores[i]      (0-based)
vector<ll> bit;      // Fenwick array of counts                 (1-based, bit[0] unused)
ll U = 0;            // number of allowed scores
ll total = 0;        // total players

// Build empty structure
void init(vector<ll> universe) {
    sort(universe.begin(), universe.end());
    universe.erase(unique(universe.begin(), universe.end()), universe.end());
    scores = universe;
    U = scores.size();
    cnt.assign(U, 0);
    bit.assign(U+1, 0);
    total = 0;
}

// score -> 0-based position, or -1 if the score is not in the universe
ll get_index(ll score) {
    ll i = lower_bound(scores.begin(), scores.end(), score) - scores.begin();
    if (i == U || scores[i] != score) return -1;
    return i;
}

// add (delta += 1) or remove (delta -= 1) one player having this score
bool update(ll score, ll delta) {
    ll i = get_index(score);
    if (i == -1) return false;               // score not allowed
    if (cnt[i] + delta < 0) return false;    // nobody to remove
    cnt[i] += delta;
    total += delta;
    for (ll x = i+1; x<=U; x += x & (-x)) {  // go UP through blocks
        bit[x] += delta;
    }
    return true;
}

// number of players among the first x slots (1-based)
ll prefix(ll x) {
    ll s = 0;
    for (ll i=x; i>0; i -= i & (-i)) {             // go DOWN through blocks
        s += bit[i];
    }
    return s;
}

// competition rank = 1 + (number of players with strictly higher score)
ll rank_of(ll score) {
    ll i = get_index(score);
    return 1 + total - prefix(i+1);
}

// score of the player at position pos from the top (1 = best)
ll select_score(ll pos) {
    ll target = total - pos + 1;
    ll at = 0;
    ll LOG = 0;
    while ((1LL << (LOG+1)) <= U) LOG++;
    for (ll i=LOG; i>=0; i--) {
        ll step = 1LL << i;
        if (at+step <= U && bit[at+step] < target) {
            at += step;
            target -= bit[at];
        }
    }
    return scores[at];                       // at = number of slots fully skipped
}

// self-check: bit must agree with plain counts
bool audit() {
    ll running = 0;
    for (ll i=0; i<U; i++) {
        running += cnt[i];
        if (prefix(i + 1) != running) return false;
    }
    return running == total;
}

int main() {
    init({50, 10, 30, 10, 40, 20});
    for (ll s : {10, 30, 40, 40, 10, 40}) update(s, 1);

    cout << "bit: ";
    for (ll i = 1; i <= U; i++) cout << bit[i] << " ";
    cout << "\n";                                   // expect 2 2 1 6 0

    cout << "ranks 40,30,10,20: ";
    for (ll s : {40, 30, 10, 20}) cout << rank_of(s) << " ";
    cout << "\n";                                   // expect 1 4 5 5

    cout << "select 1..6: ";
    for (ll p = 1; p <= 6; p++) cout << select_score(p) << " ";
    cout << "\n";                                   // expect 40 40 40 30 10 10

    cout << "audit: " << audit() << "\n";           // expect 1
    return 0;
}