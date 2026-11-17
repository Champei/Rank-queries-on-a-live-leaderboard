# Project : Rank Queries on a Live Leaderboard
Team Big_O(6) — CS 253 / MA 253, semester-3

## Team and roles

| Name | Roll no. | GitHub | Role | Module folder | Derivation |
|------|----------|--------|------|---------------|------------|
| Chaitanya Sharma | 250001020 | champei | Structure A owner | `src/structA/` | D1 |
| Teerth Dudhagara | 250001027 | coder-TD-24 | Structure B owner | `src/structB/` | D2 |
| Prathamesh Hingol | 250001056 | Prathamesh-Hingol | Architect: interface, hash table, application | `app/` | D3 |
| Herin Patel | 250001033 | herinishere | Oracle and differential testing | `oracle/` | D4 |
| Jay Gupta | 250001035 | Jay-Gupta0607 | Adversarial testing | `adversary/` | D5 |
| Sarthak Pandey | 250001063| spsbest7625-max | Analyst and benchmarks | `bench/`, `results/` | D6 |

## Summary

This project implements a live leaderboard with two complementary data structures. Structure A is an order-statistic AVL tree over `(score, player_id)` pairs; it supports ordered positions, competition rank, range queries, and split/join operations. Structure B is a coordinate-compressed Fenwick tree over the allowed score universe; it supports score counts, rank, and selected-score queries. The application keeps both structures and an open-addressing player hash table synchronized. The test modules provide a sorted-array oracle and adversarial AVL checks; benchmark results should be added after running the benchmark harness.

## Requirements

- OS: Windows, macOS, or Linux
- Compiler: C++17-capable GCC or Clang
  - The project uses `std::optional` and structured bindings.
  - Recorded environments include MinGW-w64 GCC 16.2.0 and Apple clang 17.
- Build system: No Makefile or CMake configuration is included. Use the commands below.

## Quick start: CSV queries (Windows PowerShell)

With Git and a C++17 `g++` compiler installed:

1. Clone the repository.

   ```powershell
   git clone https://github.com/Champei/Rank-queries-on-a-live-leaderboard.git
   cd Rank-queries-on-a-live-leaderboard
   ```

2. Build the application.

   ```powershell
   g++ -std=c++17 -O2 -I. src/structA/avl.cpp src/structB/fenwick.cpp app/player_hash.cpp app/leaderboard.cpp app/main.cpp -o leaderboard_app.exe
   ```

3. Edit the included CSV data and query file. `players_synthetic.csv` starts with the `player_id,score` header. `players_merge.csv` is available for a type `1` merge query. Add queries under `# Queries here:` in `queries_synthetic.txt`; its commented lines show examples.

   ```powershell
   notepad .\players_synthetic.csv
   notepad .\queries_synthetic.txt
   ```

4. Run the queries using the CSV data.

   ```powershell
   .\leaderboard_app.exe --queries .\players_synthetic.csv .\queries_synthetic.txt .\players_updated.csv | Tee-Object -FilePath .\query_results.txt
   ```

Answers appear in the terminal and `query_results.txt`. The final player scores are saved to `players_updated.csv`; the starting CSV stays unchanged.

## Tests

### Differential test

The differential runner compares the integrated application with a sorted-array reference. From the repository root:

```bash
g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic -I. \
  src/structA/avl.cpp \
  src/structB/fenwick.cpp \
  app/player_hash.cpp \
  app/leaderboard.cpp \
  oracle/reference.cpp \
  oracle/differential.cpp \
  -o build/differential

./build/differential 10000 1000 253
```

Arguments are operation count, ID-pool size, and random seed. A passing run prints a line beginning with `PASS targeted cases`.

### Adversarial AVL test

```bash
g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic -I. \
  src/structA/avl.cpp \
  adversary/generators.cpp \
  adversary/check.cpp \
  -o build/adversarial

./build/adversarial 100 1000 10000
```

The checker audits the tree during stress insertions, deletions, splits, and joins. It audits after every operation, so runtime grows approximately quadratically with the tested size.

### Other recorded checks

See `TOOLS.md` and the member test-result files for the commands and status recorded by each contributor. Some results were produced with different compiler versions. In particular, the Member 4 result file records that its local MinGW GCC 6.3 compiler could not compile the project’s C++17 features; rerun that test with a supported C++17 compiler before treating it as passed.

## Evaluation interface (hidden tests)

There is no `run_ops` executable. The application supports the CSV-backed `--queries` mode above as well as the numeric demo replay; hidden tests can call the public C++ interfaces below.

The public C++ interfaces are declared in:

- `app/leaderboard.hpp` for the integrated `Leaderboard`
- `src/structA/avl.hpp` for Structure A
- `src/structB/fenwick.hpp` for Structure B
- `app/player_hash.hpp` for the player hash table

## Benchmarks and figures

Build and run the benchmark harness from the repository root:

```bash
mkdir -p build results

g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic -I. \
  src/structA/avl.cpp \
  src/structB/fenwick.cpp \
  bench/benchmark.cpp \
  -o build/benchmark

g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic \
  bench/plot.cpp \
  -o build/plot_results

./build/benchmark --full results/full.csv
./build/plot_results results/full.csv results/full
```

The benchmark uses five trials per point and seed 253. The full run covers sizes 1,000 through 1,000,000 and writes four SVG figures. Runtime depends on the machine. The archive does not include a `results/` directory or completed benchmark output; add the generated CSV, figures, machine details, and analysis before reporting measured performance.

## Repository map

```text
.
├── app/                  # Leaderboard integration, player hash, demo
├── adversary/            # AVL stress generators and checker
├── bench/                # Benchmark harness and SVG plotter
├── docs/derivations/     # D1 through D6 derivation documents/PDFs
├── members/              # Member-specific guides
├── oracle/               # Sorted-array reference and differential runner
├── scratch/              # Member scratch code and recorded test results
├── src/structA/          # Order-statistic AVL tree
├── src/structB/          # Coordinate-compressed Fenwick tree
├── INTERFACE.md          # Shared API and invariants
├── REPORT.md             # Cost predictions and measurement report
└── TOOLS.md              # Tool-use and validation record
```

## Results at a glance

- Structure A stores `(score, player_id)` entries ordered by descending score, then ascending player ID.
- Structure B stores counts over a fixed, compressed score universe.
- The application synchronizes the AVL, Fenwick tree, and open-addressing player hash.
- The oracle module provides differential checks against an independently ordered sorted array.
- The adversary module tests rotations, ties, deletion orders, and ordered split/join cases.
- Benchmark code is present, but measured CSV results and generated figures are not included in this archive.

## Known limitations

- The score universe for the Fenwick tree is fixed before replay. An unseen score requires rebuilding the coordinate universe.
- `Avl::join` requires strict ordered key ranges. It cannot directly combine arbitrary overlapping leagues; the application merge inserts entries individually and requires disjoint player IDs.
- Application-level split rebuilds the player hash and Fenwick counts for each output partition.
- The benchmark report contains predictions and a protocol, but no completed `results/full.csv` or figures are included.
- Some member result files describe different compiler environments. Re-run the complete test set with one supported C++17 toolchain before publishing a unified validation claim.
- The interface documentation and differential tests differ on past-end ranges: the differential test expects `positions(size + 1, larger_end)` to return empty. Keep the implementation, documentation, and hidden-test contract consistent.

## Tools and references

See `TOOLS.md` for contributor-specific tool and validation records. See `REPORT.md` for the cost model and measurement protocol. Derivations D1-D6 are under `docs/derivations/`.
