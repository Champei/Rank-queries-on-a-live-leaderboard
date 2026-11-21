#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
bash scripts/build.sh
exe=''
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) exe=.exe ;; esac

"./build/player_hash_test$exe"
"./build/leaderboard_test$exe"
"./build/differential$exe" 1000000 1000 253
"./build/adversarial$exe" 100 1000 10000
"./build/leaderboard_app$exe" --file tests/players_sample.csv > build/file_actual.txt
cat > build/file_expected.txt <<'EOF'
position,player_id,score,rank
1,7,90,1
2,2,80,2
3,9,80,2
4,4,70,4
EOF
diff -u build/file_expected.txt <(sed 's/\r$//' build/file_actual.txt)
"./build/leaderboard_app$exe" --queries tests/players_sample.csv tests/queries_sample.txt build/players_updated_test.csv > build/queries_actual.txt
cat > build/queries_expected.txt <<'EOF'
0 2 80
4 2
2 9 100
3 removed 4
1 merged 2 players; total 5
5 1 9 100
5 2 11 95
5 3 7 90
6 above 3 at_or_below 2
7 players 5
saved 5 players to build/players_updated_test.csv
EOF
diff -u build/queries_expected.txt <(sed 's/\r$//' build/queries_actual.txt)
cat > build/players_expected.csv <<'EOF'
player_id,score
9,100
11,95
7,90
2,80
12,60
EOF
diff -u build/players_expected.csv <(sed 's/\r$//' build/players_updated_test.csv)
echo 'PASS unit, differential, adversarial, and CSV checks'
