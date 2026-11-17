#include "app/leaderboard.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace leaderboard;
using namespace std;
namespace {

Leaderboard load_csv(const string& path) {
    ifstream file(path);
    if (!file) throw runtime_error("cannot open input file: " + path);
    string line;
    if (!getline(file, line)) throw invalid_argument("empty CSV file: " + path);
    if (line.compare(0, 3, "\xEF\xBB\xBF") == 0) line.erase(0, 3);
    if (!line.empty() && line.back() == '\r') line.pop_back();
    if (line != "player_id,score")
        throw invalid_argument(path + ": CSV header must be player_id,score");
    vector<pair<ll, ll>> entries;
    vector<ll> scores;
    PlayerHash seen;
    size_t line_number = 1;
    while (getline(file, line)) {
        ++line_number;
        if (!line.empty() && line.back() == '\r') line.pop_back();
        try {
            auto comma = line.find(',');
            if (comma == string::npos || line.find(',', comma + 1) != string::npos)
                throw invalid_argument("expected player_id,score");
            string id_text = line.substr(0, comma), score_text = line.substr(comma + 1);
            size_t used = 0;
            ll id = stoll(id_text, &used);
            if (id_text.find_first_not_of(" \t", used) != string::npos || id < 0)
                throw invalid_argument("invalid player ID");
            ll score = stoll(score_text, &used);
            if (score_text.find_first_not_of(" \t", used) != string::npos)
                throw invalid_argument("invalid score");
            if (seen.contains(id)) throw invalid_argument("duplicate player ID");
            seen.put(id, score);
            entries.push_back({id, score});
            scores.push_back(score);
        } catch (const exception& e) {
            throw invalid_argument(path + " line " + to_string(line_number) + ": " + e.what());
        }
    }
    if (!file.eof()) throw runtime_error("failed to read input file: " + path);
    Leaderboard board(scores);
    for (const auto& [id, score] : entries) board.revise(id, score);
    return board;
}

vector<Entry> all_entries(const Leaderboard& board) {
    return board.size() ? board.positions(1, board.size()) : vector<Entry>{};
}

void accept_scores(Leaderboard& board, const vector<ll>& added_scores) {
    bool rebuild = false;
    for (ll score : added_scores) if (!board.fenwick().accepts(score)) {
        rebuild = true;
        break;
    }
    if (!rebuild) return;
    // shortcut: unseen scores rebuild all coordinates, pre-scan scores for large update streams.
    auto entries = all_entries(board);
    vector<ll> scores = added_scores;
    for (const auto& e : entries) scores.push_back(e.score);
    Leaderboard expanded(scores);
    for (const auto& e : entries) expanded.revise(e.player_id, e.score);
    board = move(expanded);
}

void save_csv(const Leaderboard& board, const string& path) {
    ofstream file(path);
    if (!file) throw runtime_error("cannot open output file: " + path);
    file << "player_id,score\n";
    for (const auto& e : all_entries(board))
        file << e.player_id << ',' << e.score << '\n';
    file.flush();
    if (!file) throw runtime_error("failed to write output file: " + path);
}

void run_queries(Leaderboard& board, const string& path, const string& output_path) {
    ifstream file(path);
    if (!file) throw runtime_error("cannot open query file: " + path);
    string line;
    size_t line_number = 0;
    while (getline(file, line)) {
        ++line_number;
        if (line_number == 1 && line.compare(0, 3, "\xEF\xBB\xBF") == 0)
            line.erase(0, 3);
        if (!line.empty() && line.back() == '\r') line.pop_back();
        istringstream in(line);
        in >> ws;
        if (in.eof() || in.peek() == '#') continue;
        try {
            int type;
            if (!(in >> type)) throw invalid_argument("expected query type");
            ll a, b;
            if (type == 1) {
                string other_path;
                getline(in >> ws, other_path);
                other_path.erase(other_path.find_last_not_of(" \t") + 1);
                if (other_path.empty()) throw invalid_argument("expected CSV path");
                if (filesystem::weakly_canonical(other_path) ==
                    filesystem::weakly_canonical(output_path))
                    throw invalid_argument("output file must differ from merged CSV");
                auto other = load_csv(other_path);
                vector<ll> scores;
                for (const auto& e : all_entries(other)) scores.push_back(e.score);
                accept_scores(board, scores);
                board.merge(other);
                cout << "1 merged " << scores.size() << " players; total " << board.size() << '\n';
                continue;
            }
            if (type == 0 || type == 3 || type == 4 || type == 6) {
                if (!(in >> a)) throw invalid_argument("expected one integer argument");
            } else if (type == 2 || type == 5) {
                if (!(in >> a >> b)) throw invalid_argument("expected two integer arguments");
            } else if (type != 7) {
                throw invalid_argument("unknown query type");
            }
            in >> ws;
            if (!in.eof()) throw invalid_argument("extra query arguments");
            switch (type) {
            case 0: {
                if (a < 1) throw invalid_argument("position must be positive");
                auto e = board.select(static_cast<size_t>(a));
                cout << "0 " << e.player_id << ' ' << e.score << '\n';
                break;
            }
            case 2:
                accept_scores(board, {b});
                board.revise(a, b);
                cout << "2 " << a << ' ' << b << '\n';
                break;
            case 3:
                board.remove(a);
                cout << "3 removed " << a << '\n';
                break;
            case 4:
                cout << "4 " << board.rank(a) << '\n';
                break;
            case 5: {
                if (a < 1 || b < a) throw invalid_argument("invalid position range");
                size_t position = static_cast<size_t>(a);
                for (const auto& e : board.positions(position, static_cast<size_t>(b)))
                    cout << "5 " << position++ << ' ' << e.player_id << ' ' << e.score << '\n';
                break;
            }
            case 6: {
                auto [high, low] = board.split(a);
                cout << "6 above " << high.size() << " at_or_below " << low.size() << '\n';
                high.merge(low);
                board = move(high);
                break;
            }
            case 7:
                cout << "7 players " << board.audit() << '\n';
                break;
            }
        } catch (const exception& e) {
            throw runtime_error(path + " line " + to_string(line_number) + ": " + e.what());
        }
    }
    if (!file.eof()) throw runtime_error("failed to read query file: " + path);
}
}

int main(int argc, char** argv) {
    try {
        if (argc > 1 && string(argv[1]) == "--file") {
            if (argc != 3) throw invalid_argument("usage: leaderboard_app.exe --file players.csv");
            auto board = load_csv(argv[2]);
            cout << "position,player_id,score,rank\n";
            size_t position = 0;
            for (const auto& e : all_entries(board))
                cout << ++position << ',' << e.player_id << ',' << e.score
                     << ',' << board.rank(e.player_id) << '\n';
            return 0;
        }
        if (argc > 1 && string(argv[1]) == "--queries") {
            if (argc != 5)
                throw invalid_argument("usage: leaderboard_app.exe --queries players.csv queries.txt updated.csv");
            if (filesystem::weakly_canonical(argv[4]) == filesystem::weakly_canonical(argv[2]) ||
                filesystem::weakly_canonical(argv[4]) == filesystem::weakly_canonical(argv[3]))
                throw invalid_argument("output file must differ from input and query files");
            auto board = load_csv(argv[2]);
            run_queries(board, argv[3], argv[4]);
            save_csv(board, argv[4]);
            cout << "saved " << board.size() << " players to " << argv[4] << '\n';
            return 0;
        }
        size_t players = 1000, revisions = 5000;
        if (argc > 1) players = stoull(argv[1]);
        if (argc > 2) revisions = stoull(argv[2]);
        if (players == 0) throw invalid_argument("players must be positive");
        vector<ll> scores;
        for (int s = 0; s <= 10000; ++s) scores.push_back(s);
        Leaderboard board(scores);
        mt19937_64 rng(253);
        for (size_t i = 0; i < players; ++i)
            board.revise(static_cast<ll>(i), rng() % 10001);
        for (size_t i = 0; i < revisions; ++i) {
            auto id = static_cast<ll>(rng() % players);
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
