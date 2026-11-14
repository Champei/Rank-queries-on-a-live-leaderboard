# Member 5: Adversary

**Owner: Jay Gupta**

This contribution builds inputs that stress the AVL tree (Structure A) and checks that it stays correct under them. It covers four things:

- an insert order that forces a rotation on almost every insert;
- delete orders that do the same on the way down;
- keys whose scores all tie, so only the ID tie-break orders them;
- ordered joins and splits at many height gaps, including joins that must be rejected.

The whole tree is audited after every insert of the stress families and after every delete. After every split, every key is checked to be on the correct side.

## Files

- `adversary/generators.hpp`, `adversary/generators.cpp`: the input families.
  - `rotation_inserts(n)`: rising scores, so each key becomes the new first key.
  - `rotation_deletes(n)`: the strongest delete order found.
  - `rotation_deletes_one_end(n)`: the starter delete order, kept for comparison.
  - `tied_inserts(n)`: the same shape with every score equal.
  - `unequal_join(n)`: a tall tree and a one-node tree to join.
  - `ordered_join_case(left_n, right_n)`: join inputs with any chosen sizes.
- `adversary/check.cpp`: the checker. It accepts several sizes in one run.
- `docs/derivations/D5.md` and `D5.pdf`. The derivation proves that sorted inserts cause exactly `n - bitlen(n)` rotations, and that split costs `O(log n)`. It also reports the measured delete family, with no proof claimed for it.
- `scratch/scratch-e/delete_search.cpp`: the search that found `rotation_deletes`. It runs a greedy one-step search on a copy of the AVL code. The copy is checked against the real tree after every operation. It then compares the family with the search for every n up to 200, and with `n - bitlen(n)` for every n up to 3000.
- `scratch/scratch-e/plant_bugs.sh`: plants 15 faults in temporary copies of `avl.cpp`, one at a time, and confirms the checker fails on each. The repository is not changed.
- `scratch/scratch-e/proof_checks.py`: numerical checks of the D5 lemmas, Lemma 4, Theorem 2 and the worked example, on a Python copy of the AVL code.
- `scratch/scratch-e/member5-test-results.txt`: output of all the runs below.
- `scratch/scratch-e/export_d5.py`: rebuilds `D5.pdf` from `D5.md`. It needs the Python `markdown` package and Chrome or Edge.

## Build and run

From the repository root, with a C++17 compiler:

```powershell
mkdir build -Force
g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic -I. src/structA/avl.cpp adversary/generators.cpp adversary/check.cpp -o build/adversarial.exe
./build/adversarial.exe 100 1000 10000
```

Each size prints one `PASS` line. Any failure prints `adversary error:` with a reason and exits with code 1. When an `audit()` fails, the reason also names the phase and step.

The checker audits the whole tree after every operation, so its run time grows with `n^2`. On the test machine, n = 10,000 takes about two seconds.

On Windows with several MinGW installs on `PATH`, a program can stop at once with exit code 127 and no output, because it loaded another compiler's runtime DLL. Adding `-static` to the `g++` command avoids this.

Other checks, from the repository root (`plant_bugs.sh` needs Git Bash on Windows):

```powershell
g++ -std=c++17 -O2 -I. src/structA/avl.cpp adversary/generators.cpp scratch/scratch-e/delete_search.cpp -o build/delete_search.exe
./build/delete_search.exe
bash scratch/scratch-e/plant_bugs.sh
python scratch/scratch-e/proof_checks.py
```

## Results for the report

These numbers come from Windows 11 and MinGW-w64 g++ 16.2.0 with `-O2`. The rotation counts are deterministic. The random trials (random delete order, random joins and random splits) use seed 253.

| n | insert rotations | per insert | delete rotations (`rotation_deletes`) | per delete | starter delete order |
|---:|---:|---:|---:|---:|---:|
| 100 | 93 | 0.930 | 93 | 0.930 | 44 |
| 1 000 | 990 | 0.990 | 990 | 0.990 | 491 |
| 1 001 | 991 | 0.990 | 992 | 0.991 | 491 |
| 10 000 | 9 986 | 0.9986 | 9 986 | 0.9986 | 4 988 |
| 1 000 000 | 999 980 | 1.000 | 999 980 | 1.000 | 499 981 |

The rows for n = 100 to 10,000 come from `adversarial.exe`, which audits after every step. The 1,000,000 row comes from the "Large sizes" part of `delete_search.exe`, which runs without per-step audits.

- **Inserts.** The count equals `n - bitlen(n)` exactly, as proved in D5 Theorem 1.
- **Deletes.** `rotation_deletes` matches the greedy search for every n up to 200, and reaches at least `n - bitlen(n) - 1` at every n up to 3000. That's about twice the starter order. This is measured only; there is no proof yet.
- **Correctness.** Every audit, join and split check passed at all sizes tested.
- **Fault detection.** All 15 planted faults were caught.

## Change record

The checker now does the following, beyond the starter version:
- reports insert and delete rotations, with and without the stronger delete order;
- fails if the insert count differs from `n - bitlen(n)` (D5 Theorem 1);
- deletes in four orders, including the middle key each time and a random order, so deleting a node with two children is exercised;
- tests a family where every score ties;
- checks every split by comparing in-order key sequences;
- tests 10 join shapes and 100 random joins, with shuffled inserts so joins have to rotate, plus random pivot and score splits.

It also checks that `Avl::join` rejects keys on the wrong side, including tied scores on the wrong side by ID. It checks that a rejected join leaves its inputs unchanged, and that a valid tied join is accepted. The stronger delete order replaces the starter one. The derivation is authored under Jay Gupta's name.

A separate AI review of the code and D5, done by a second Claude agent and recorded in `TOOLS.md`, found no correctness bugs. Its suggestions led to:
- the tie tests;
- the shuffled join inputs;
- the per-operation check of the search's AVL copy;
- clearer failure messages;
- the committed proof and fault scripts;
- tighter wording in Lemmas 1 and 4.
