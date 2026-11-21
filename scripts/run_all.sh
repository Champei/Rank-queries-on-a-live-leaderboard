#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
bash tests/run_tests.sh
exe=''
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) exe=.exe ;; esac
"./build/leaderboard_app$exe" 1000 5000 > build/demo.txt
"./build/leaderboard_app$exe" 1000000 1000000 > build/full_replay.txt
mkdir -p results/raw results/figures
"./build/benchmark$exe" --full results/raw/full.csv
"./build/plot_results$exe" results/raw/full.csv results/figures/full
