# Rank-queries-on-a-live-leaderboard

This C++17 project compares an order-statistic AVL tree with a coordinate-compressed Fenwick tree on a live leaderboard. Members 1, 2, 3, and 4's modules are present. The adversary and benchmark modules are still needed for the complete project.

## Clone and run the included CSV data (Windows PowerShell)

Install a C++17 compiler such as MinGW-w64 `g++`. The repository includes `players_synthetic.csv`, `players_merge.csv`, and `queries_synthetic.txt`, so run these commands in PowerShell:

```powershell
git clone https://github.com/Champei/Rank-queries-on-a-live-leaderboard.git
cd Rank-queries-on-a-live-leaderboard
g++ -std=c++17 -O2 -I. src/structA/avl.cpp src/structB/fenwick.cpp app/player_hash.cpp app/leaderboard.cpp app/main.cpp -o leaderboard_app.exe
.\leaderboard_app.exe --queries .\players_synthetic.csv .\queries_synthetic.txt .\players_updated.csv | Tee-Object -FilePath .\query_results.txt
```

## Run with your own player scores

Create a CSV file such as `players.csv` in the project root. The first row must be `player_id,score`; each following row has one unique, nonnegative numeric player ID and one integer score:

```csv
player_id,score
7,90
9,80
2,80
4,70
```

Run `./leaderboard_app.exe --file players.csv` (or `./leaderboard_app.exe --file "C:\path\to\players.csv"`). The app prints `position,player_id,score,rank` as CSV. Higher scores come first; equal scores share a competition rank, while lower player IDs come first within a tie. To save the result, run `./leaderboard_app.exe --file players.csv > results.csv`. The original numeric-argument mode, such as `./leaderboard_app.exe 1000 5000`, still generates demo data instead of reading a file.

## Run numbered queries and save the updated leaderboard

`queries_synthetic.txt` shows commented examples for every query type. Add your own active queries under `# Queries here:`, save the file, then rerun the last command above.

Query answers go to `query_results.txt`; the final player IDs and scores go to `players_updated.csv`. The input CSV stays unchanged. Each nonempty line of the query file has one numbered query; lines starting with `#` are comments. Positions and competition ranks start at 1.

| Query | Meaning |
| --- | --- |
| `0 position` | Print the player ID and score at that position. |
| `1 other.csv` | Merge another leaderboard CSV with distinct player IDs. Paths are relative to the project root. |
| `2 player_id score` | Add a player or replace their score. |
| `3 player_id` | Remove a player. |
| `4 player_id` | Print that player's competition rank. |
| `5 first last` | Print players in the inclusive position range. |
| `6 threshold` | Print counts above and at or below the score threshold; keep the leaderboard intact. |
| `7` | Audit the structures and print the player count. |

The program checks each query and saves the output CSV only after every query succeeds. The output path must differ from the input CSV, query file, and any merged CSV.

Member 3's [interface](INTERFACE.md) defines competition ranks, ID tie order for positions, and season operations. The [architect guide](members/03-architect/README.md) has the hash and application test commands. The rank proof is in [D3](docs/derivations/D3.md). Member 1 and 2 guides are in [members/01-structure-a/README.md](members/01-structure-a/README.md) and [members/02-structure-b/README.md](members/02-structure-b/README.md).

Member 4's [oracle guide](members/04-oracle/README.md) describes the sorted-array reference, seeded differential checks, and targeted edge cases. The rotation metadata and selection proof is in [D4](docs/derivations/D4.md).
