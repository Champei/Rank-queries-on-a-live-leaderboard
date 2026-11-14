# Member 2: Teerth - Structure B

Own `src/structB/fenwick.hpp`, `src/structB/fenwick.cpp` and `docs/derivations/D2.md`/`D2.pdf`. The Fenwick stores counts over a fixed, compressed score universe. It returns competition ranks and selected scores; the AVL resolves player IDs and their tie order.

The implementation and proof are AI-assisted; see `TOOLS.md`. Review the proof and practise modifying the module before the viva. The GitHub contribution is pending until a branch and PR exist in the team's repository.

## Run from the project root on macOS

`g++` on this Mac is Apple clang 17.0.0. CMake is not needed for these commands. Binaries below go into `scratch/` and should not be committed.

Practice example:

```bash
g++ -std=c++17 scratch/mine.cpp -o scratch/mine
./scratch/mine
```

Expected values: bit `2 2 1 6 0`, ranks `1 4 5 5`, selected scores `40 40 40 30 10 10`, practice audit `1`. The class's audit instead returns the player count.

Random checks of both practice code and integrated Fenwick against a sorted array:

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -I. src/structB/fenwick.cpp scratch/fenwick_random.cpp -o scratch/fenwick_random
./scratch/fenwick_random
```

Expected success: `PASS 120000 random updates (300 universes x 400), seed 253; edge cases passed`.

Integrated application:

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -I. src/structA/avl.cpp src/structB/fenwick.cpp app/player_hash.cpp app/leaderboard.cpp app/main.cpp -o scratch/leaderboard_app
./scratch/leaderboard_app 1000 5000
```

Existing differential tester:

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -I. src/structA/avl.cpp src/structB/fenwick.cpp app/player_hash.cpp app/leaderboard.cpp oracle/reference.cpp oracle/differential.cpp -o scratch/differential
./scratch/differential 10000 1000 253
```

Final-code checks were run by Codex on 14 November 2026 and passed. See `scratch/member2-test-results.txt`. These are correctness checks, not timing benchmarks.

## Submit through the team's existing GitHub repository

This contribution was prepared in a clone of https://github.com/Champei/Rank-queries-on-a-live-leaderboard on branch `member2-fenwick`. At preparation time main contained only the initial README. Integrated application and differential commands require the other members' modules after integration; the standalone Fenwick tester can run now.

In the clone, check `git status` and the team base branch, then create a branch from the agreed base:

```bash
git switch -c member2-fenwick
```

Compare and copy only your owned files from this folder into the clone. Keep other members' newer work. Merge the Member 2 section of `TOOLS.md` rather than replacing teammates' notes. Include these files:

- `src/structB/fenwick.hpp` and `fenwick.cpp`
- `docs/derivations/D2.md` and `D2.pdf`
- `members/02-structure-b/README.md`, `viva.md`, `PR.md`
- `scratch/mine.cpp`, `scratch/fenwick_random.cpp`, `scratch/member2-test-results.txt`, `scratch/export_d2.py`
- The merged `TOOLS.md`

Run the commands above inside the clone. Then stage the explicit files:

```bash
git add src/structB/fenwick.hpp src/structB/fenwick.cpp docs/derivations/D2.md docs/derivations/D2.pdf members/02-structure-b/README.md members/02-structure-b/viva.md members/02-structure-b/PR.md scratch/mine.cpp scratch/fenwick_random.cpp scratch/member2-test-results.txt scratch/export_d2.py TOOLS.md
git diff --cached --stat
git diff --cached
git commit -m "Complete Fenwick module, D2 proof and validation"
git push -u origin member2-fenwick
```

Review the staged diff for accidental unrelated changes. Do not stage compiled binaries or use `git add .`. On GitHub, open a PR from `member2-fenwick` into the team's agreed base branch. Use the title/body in `PR.md`, get teammate review, and merge after shared checks pass. A source file containing `using ll = long long` has been checked on this Mac; integration on a platform where `int64_t` differs from `long long` may require the exact project score alias.

## Re-export D2 after editing the proof

Use a temporary environment to avoid changing system Python packages:

```bash
python3 -m venv /private/tmp/teerth-d2-export
/private/tmp/teerth-d2-export/bin/pip install reportlab pymupdf
/private/tmp/teerth-d2-export/bin/python scratch/export_d2.py
```

This replaces `docs/derivations/D2.pdf` from `D2.md` and renders previews in `/private/tmp/teerth-d2-preview/`. Review all pages after changing the Markdown.
