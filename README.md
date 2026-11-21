# Project 2: Rank Queries on a Live Leaderboard

Team Big_O(6) - CS 253 / MA 253, semester 3.

## Team and roles

| Name | Roll no. | GitHub | Role | Module folder | Derivation |
|---|---|---|---|---|---|
| Chaitanya Sharma | 250001020 | @Champei | Structure A owner | `src/structA/` | D1 |
| Teerth Dudhagara | 250001027 | @coder-TD-24 | Structure B owner | `src/structB/` | D2 |
| Prathamesh Hingol | 250001056 | @Prathamesh-Hingol | Architect | `include/`, `app/` | D3 |
| Herin Patel | 250001033 | @herinishere | Oracle owner | `oracle/` | D4 |
| Jay Gupta | 250001035 | @Jay-Gupta0607 | Adversary owner | `adversary/` | D5 |
| Sarthak Pandey | 250001063 | @spsbest7625-max | Analyst | `bench/`, `results/` | D6 |

## Summary

The service maintains a live leaderboard through score revisions, rank and position queries, and season split and league merge operations. Structure A is an order-statistic AVL tree over `(score, player_id)` pairs; Structure B is a coordinate-compressed Fenwick tree of score counts. An open-addressing hash table maps player IDs to scores, and a sorted-array oracle checks the integrated result. In our five-trial run at one million players, Fenwick rank and score selection were faster, while isolated AVL split and ordered join were much faster than rebuilding Fenwick counts.

## Requirements

- OS: Windows with Git Bash and MSYS2 MinGW-w64, macOS, or Linux.
- Language: C++17. Verified here with MSYS2 MinGW-w64 GCC 16.2.0 and `-O2 -Wall -Wextra -Wpedantic`; an earlier contributor used Apple clang 17.
- Tools: Bash, a C++17 `g++` or `clang++`, `diff`, and `sed`. Set `CXX` to the compiler path if needed. No external runtime library is required. The oracle and tests may use standard-library containers; the AVL, Fenwick tree, and player hash table are implemented here.

## Build

From the repository root, compile the application, tests, benchmark, and plotter with one command:

```bash
bash scripts/build.sh
```

On Windows, run in Git Bash with the MSYS2 compiler on `PATH`, or set `CXX=/c/msys64/mingw64/bin/g++.exe` before this command. Binaries appear in `build/` (with `.exe` on Windows).

## Quick start

After building, run `./build/leaderboard_app --file tests/players_sample.csv` (add `.exe` on Windows). Expected output:

```text
position,player_id,score,rank
1,7,90,1
2,2,80,2
3,9,80,2
4,4,70,4
```

For revisions and season queries, run `./build/leaderboard_app --queries tests/players_sample.csv tests/queries_sample.txt build/updated.csv`. Responses go to standard output and final scores to `build/updated.csv`. The larger editable example is `players_synthetic.csv` with `queries_synthetic.txt`; write its output under `build/` to preserve the checked-in sample.

### CSV query workflow (Windows PowerShell)

The repository also keeps players_synthetic.csv, players_merge.csv, players_updated.csv, queries_synthetic.txt, and query_results.txt. To use the original editable CSV workflow from a fresh clone:

1. Clone the repository.

   ~~~powershell
   git clone https://github.com/Champei/Rank-queries-on-a-live-leaderboard.git
   cd Rank-queries-on-a-live-leaderboard
   ~~~

2. Build the application. With a C++17 g++ compiler on PATH, this direct command is an alternative to the full build script:

   ~~~powershell
   g++ -std=c++17 -O2 -I. src/structA/avl.cpp src/structB/fenwick.cpp app/player_hash.cpp app/leaderboard.cpp app/main.cpp -o leaderboard_app.exe
   ~~~

3. Edit the included CSV data and query file. players_synthetic.csv starts with the player_id,score header. players_merge.csv is available for a type 1 merge query. Add queries under the "# Queries here:" line; the commented lines show examples.

   ~~~powershell
   notepad .\players_synthetic.csv
   notepad .\queries_synthetic.txt
   ~~~

4. Run the queries and save both kinds of output.

   ~~~powershell
   .\leaderboard_app.exe --queries .\players_synthetic.csv .\queries_synthetic.txt .\players_updated.csv | Tee-Object -FilePath .\query_results.txt
   ~~~

Answers appear in the terminal and query_results.txt. Final player scores are saved to players_updated.csv; the starting CSV stays unchanged.

## Tests

```bash
bash tests/run_tests.sh
```

This runs hash and integration checks, the sorted-array differential test at **1,000,000 operations** with ID pool 1,000 and seed **253**, adversarial AVL checks at sizes 100, 1,000, and 10,000, and CSV input/output checks. Passing output includes `PASS targeted cases; 1000000 operations, seed 253` and ends with `PASS unit, differential, adversarial, and CSV checks`. The bounded ID pool keeps the sorted-array oracle feasible; the separate application replay covers one million players. Failures return nonzero.

## Evaluation interface (hidden tests)

The current executable accepts `./build/leaderboard_app --queries INPUT.csv QUERIES.txt OUTPUT.csv` (`.exe` on Windows). `INPUT.csv` has a `player_id,score` header and one nonnegative integer ID and integer score per row. `QUERIES.txt` has one whitespace-separated query per line; blank lines and lines beginning with `#` are ignored:

| Type | Arguments | Standard-output response |
|---|---|---|
| `0` | position | `0 player_id score` |
| `1` | other CSV path | `1 merged count players; total count` |
| `2` | player_id score | `2 player_id score` |
| `3` | player_id | `3 removed player_id` |
| `4` | player_id | `4 competition_rank` |
| `5` | first last | One `5 position player_id score` line per result |
| `6` | threshold | `6 above count at_or_below count` |
| `7` | none | `7 players count` |

The program ends with `saved count players to OUTPUT.csv`; that file has the same header and final players in leaderboard order. For example, `tests/players_sample.csv` and query `4 9` produce `4 2` before the saved-file line. Errors go to standard error and return nonzero. Public C++ interfaces are in `include/leaderboard.hpp`, `src/structA/avl.hpp`, `src/structB/fenwick.hpp`, and `app/player_hash.hpp`. The instructor's exact hidden-test format was not supplied; this contract must be checked against it before the final tag.

## Benchmarks and figures

```bash
bash scripts/run_all.sh
```

This builds, tests, runs small and one-million-player/one-million-revision replays, measures five trials per point at 1,000, 10,000, 100,000, and 1,000,000 players with seed 253, then regenerates four SVG figures. Replay output is in `build/`; raw trials, means, and sample standard deviations are in `results/raw/full.csv`; figures are in `results/figures/`. Runtime depends on the machine, especially the full-size Fenwick rebuilds. See `REPORT.md` for the protocol and analysis.

## Repository map

| Path | Purpose |
|---|---|
| `include/` | Integrated public interface |
| `src/structA/`, `src/structB/` | AVL and Fenwick implementations |
| `app/` | Player hash, application, and entry point |
| `oracle/`, `adversary/`, `tests/` | Reference, stress cases, and focused checks |
| `bench/`, `results/` | Measurement code, raw trials, and figures |
| `scripts/` | One-command build and full reproduction |
| `docs/derivations/`, `docs/slides/` | Member proofs and presentation material |
| `members/`, `scratch/` | Contributor notes and historical experiments |

`INTERFACE.md` describes invariants; `REPORT.md` and `REPORT.pdf` hold the analysis; `TOOLS.md` records tool use. All six roll-numbered derivation PDFs are in `docs/derivations/`, and the presentation PDF is `docs/slides/slides.pdf`.

## Results at a glance

Five-trial mean at `n = 1,000,000`, on the machine described in `REPORT.md`:

| Operation | AVL | Fenwick |
|---|---:|---:|
| Competition rank | 2.069 us | 0.640 us |
| Select entry / select score | 2.329 us | 1.070 us |
| Score split / rebuild two counts | 2.564 us | 186,554 us |
| Ordered join / rebuild combined counts | 0.200 us | 164,618 us |

The selection and season-operation pairs have different return values and preconditions; read `REPORT.md` before using these numbers as an application-level comparison.

## Known limitations

- The Fenwick score universe is fixed for each instance; unseen scores require a coordinate rebuild.
- `Avl::join` requires strict ordered key ranges. Application merge handles overlapping scores through individual inserts and requires disjoint IDs.
- Application split rebuilds hash and Fenwick state; the isolated AVL split timing is not an application split timing.
- The hidden-test format needs instructor confirmation. RAM, cache sizes, and profiler counters for this measurement machine remain unverified.

## Tools and references

See `TOOLS.md` for declared tools and `REPORT.md` for the source list. The supplied reading list includes Adelson-Velsky and Landis on AVL trees, Fenwick on cumulative frequency tables, Blelloch et al. on join, Tarjan on search trees, CLRS on augmentation, and Knuth on balanced trees.
