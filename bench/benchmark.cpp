#include "src/structA/avl.hpp"
#include "src/structB/fenwick.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

using namespace leaderboard;
using Clock = std::chrono::steady_clock;
static volatile std::uint64_t sink = 0;
static double us_per_op(Clock::time_point start, std::size_t ops) {
    return std::chrono::duration<double, std::micro>(Clock::now() - start).count() / ops;
}
static void row(std::ofstream& out, std::string label, std::size_t n,
                const std::vector<double>& trials, std::uint64_t seed) {
    double mean = std::accumulate(trials.begin(), trials.end(), 0.0) / trials.size();
    double sumsq = 0;
    for (double x : trials) sumsq += (x - mean) * (x - mean);
    double sd = std::sqrt(sumsq / (trials.size() - 1));
    out << label << ',' << n << ',' << seed << ',' << trials.size()
        << ',' << mean << ',' << sd;
    for (double x : trials) out << ',' << x;
    out << '\n';
    std::cout << label << " n=" << n << " mean=" << mean
              << " us/op sd=" << sd << '\n';
}
int main(int argc, char** argv) {
    try {
        bool full = argc > 1 && std::string(argv[1]) == "--full";
        std::string path = argc > 2 ? argv[2] : "results/raw.csv";
        constexpr std::uint64_t seed = 253;
        std::ofstream out(path);
        if (!out) throw std::runtime_error("cannot open results CSV");
        out << "operation,n,seed,trials,mean_us_per_op,sd_us_per_op,trial1,trial2,trial3,trial4,trial5\n";
        auto sizes = full ? std::vector<std::size_t>{1000, 10000, 100000, 1000000}
                          : std::vector<std::size_t>{1000, 10000};
        for (auto n : sizes) {
            std::vector<std::int64_t> universe;
            universe.reserve(n);
            for (std::size_t i = 0; i < n; ++i) universe.push_back(i);
            std::mt19937_64 rng(seed + n);
            std::vector<std::size_t> order(n);
            std::iota(order.begin(), order.end(), 0);
            std::shuffle(order.begin(), order.end(), rng);
            Avl avl; Fenwick fenwick(universe);
            for (auto i : order) {
                avl.insert({static_cast<std::int64_t>(i), static_cast<std::int64_t>(i)});
                fenwick.update(i, 1);
            }
            std::vector<double> arank, brank, aselect, bselect;
            std::vector<double> asplit, brebuild, ajoin, bjoin;
            std::size_t queries = full ? 100000 : 10000;
            for (int trial = 0; trial < 5; ++trial) {
                rng.seed(seed + n + trial);
                auto start = Clock::now();
                for (std::size_t i = 0; i < queries; ++i)
                    sink = sink + avl.rank(static_cast<std::int64_t>(rng() % n));
                arank.push_back(us_per_op(start, queries));
                rng.seed(seed + n + trial);
                start = Clock::now();
                for (std::size_t i = 0; i < queries; ++i)
                    sink = sink + fenwick.rank(static_cast<std::int64_t>(rng() % n));
                brank.push_back(us_per_op(start, queries));
                rng.seed(seed + n + trial);
                start = Clock::now();
                for (std::size_t i = 0; i < queries; ++i)
                    sink = sink + avl.select(1 + rng() % n).player_id;
                aselect.push_back(us_per_op(start, queries));
                rng.seed(seed + n + trial);
                start = Clock::now();
                for (std::size_t i = 0; i < queries; ++i)
                    sink = sink + fenwick.select_score(1 + rng() % n);
                bselect.push_back(us_per_op(start, queries));
                Avl temp;
                for (auto i : order) temp.insert({static_cast<std::int64_t>(i),
                                                   static_cast<std::int64_t>(i)});
                double split_us = 0, join_us = 0;
                constexpr std::size_t repetitions = 1000;
                for (std::size_t repeat = 0; repeat < repetitions; ++repeat) {
                    start = Clock::now();
                    auto [high, low] = temp.split_score(static_cast<std::int64_t>(n / 2));
                    split_us += std::chrono::duration<double, std::micro>(Clock::now() - start).count();
                    auto [unused, pivot, below] =
                        low.split({static_cast<std::int64_t>(n / 2),
                                   static_cast<std::int64_t>(n / 2)});
                    if (!pivot || unused.size()) throw std::logic_error("split pivot");
                    start = Clock::now();
                    temp = Avl::join(std::move(high), *pivot, std::move(below));
                    join_us += std::chrono::duration<double, std::micro>(Clock::now() - start).count();
                }
                asplit.push_back(split_us / repetitions);
                ajoin.push_back(join_us / repetitions);
                std::vector<std::int64_t> left, right;
                left.reserve(n); right.reserve(n);
                for (auto score : universe)
                    (score > static_cast<std::int64_t>(n / 2) ? left : right).push_back(score);
                start = Clock::now();
                auto f1 = Fenwick::rebuild(universe, left);
                auto f2 = Fenwick::rebuild(universe, right);
                brebuild.push_back(us_per_op(start, 1));
                sink = sink + temp.size() + f1.size() + f2.size();
                start = Clock::now();
                auto rebuilt = Fenwick::rebuild(universe, universe);
                bjoin.push_back(us_per_op(start, 1));
                sink = sink + rebuilt.size();
            }
            row(out, "avl_rank", n, arank, seed);
            row(out, "fenwick_rank", n, brank, seed);
            row(out, "avl_select", n, aselect, seed);
            row(out, "fenwick_select_score", n, bselect, seed);
            row(out, "avl_split_score", n, asplit, seed);
            row(out, "fenwick_split_rebuild", n, brebuild, seed);
            row(out, "avl_ordered_join", n, ajoin, seed);
            row(out, "fenwick_join_rebuild", n, bjoin, seed);
        }
        std::cout << "wrote " << path << "; sink=" << sink << '\n';
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "bench error: " << e.what() << '\n'; return 1;
    }
}
