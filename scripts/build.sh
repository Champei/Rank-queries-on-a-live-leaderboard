#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p build
CXX=${CXX:-g++}
flags=(-std=c++17 -O2 -Wall -Wextra -Wpedantic -I.)
exe=''
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) exe=.exe ;; esac

"$CXX" "${flags[@]}" src/structA/avl.cpp src/structB/fenwick.cpp app/player_hash.cpp app/leaderboard.cpp app/main.cpp -o "build/leaderboard_app$exe"
"$CXX" "${flags[@]}" src/structA/avl.cpp src/structB/fenwick.cpp app/player_hash.cpp app/leaderboard.cpp oracle/reference.cpp oracle/differential.cpp -o "build/differential$exe"
"$CXX" "${flags[@]}" src/structA/avl.cpp adversary/generators.cpp adversary/check.cpp -o "build/adversarial$exe"
"$CXX" "${flags[@]}" app/player_hash.cpp tests/player_hash_test.cpp -o "build/player_hash_test$exe"
"$CXX" "${flags[@]}" src/structA/avl.cpp src/structB/fenwick.cpp app/player_hash.cpp app/leaderboard.cpp tests/leaderboard_test.cpp -o "build/leaderboard_test$exe"
"$CXX" "${flags[@]}" src/structA/avl.cpp src/structB/fenwick.cpp bench/benchmark.cpp -o "build/benchmark$exe"
"$CXX" "${flags[@]}" bench/plot.cpp -o "build/plot_results$exe"
