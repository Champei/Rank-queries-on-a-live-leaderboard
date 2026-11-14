# Member 5: Adversary

**Owner: Jay Gupta**

This contribution builds inputs that stress the AVL tree (Structure A) and checks that it stays correct under them. It covers three things:

- an insert order that forces a rotation on almost every insert;
- a delete order that does the same on the way down;
- ordered joins and splits at many height gaps.

After every single insert and delete, the whole tree is audited. After every split, every key is checked to be on the correct side.

## Files

- `adversary/generators.hpp`, `adversary/generators.cpp`: the input families.
  - `rotation_inserts(n)`: rising scores, so each key becomes the new first key.
  - `rotation_deletes(n)`: the strongest delete order found.
  - `rotation_deletes_one_end(n)`: the starter delete order, kept for comparison.
  - `unequal_join(n)`: a tall tree and a one-node tree to join.
  - `ordered_join_case(left_n, right_n)`: join inputs with any chosen sizes.
- `adversary/check.cpp`: the checker. It accepts several sizes in one run.
- `docs/derivations/D5.md` and `D5.pdf`. The derivation proves that sorted inserts cause exactly `n - bitlen(n)` rotations, and that split costs `O(log n)`. It also reports the measured delete family, with no proof claimed for it.
- `scratch/scratch-e/delete_search.cpp`: the search that found `rotation_deletes`. It runs a greedy one-step search on a copy of the AVL code, checked against the real tree, and then checks the family at every size up to 3000.
- `scratch/scratch-e/member5-test-results.txt`: output of the runs below.
- `scratch/scratch-e/export_d5.py`: rebuilds `D5.pdf` from `D5.md`. It needs the Python `markdown` package and Chrome or Edge.

## Build and run

From the repository root, with a C++17 compiler:

```powershell
mkdir build -Force
g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic -I. src/structA/avl.cpp adversary/generators.cpp adversary/check.cpp -o build/adversarial.exe
./build/adversarial.exe 100 1000 10000
```

Each size prints one `PASS` line. Any failure prints `adversary error: <which check>` and exits with code 1.

The checker audits the whole tree after every operation, so its run time grows with `n^2`. On the test machine, n = 10,000 takes about one second.

To repeat the delete-order search:

```powershell
g++ -std=c++17 -O2 -I. src/structA/avl.cpp adversary/generators.cpp scratch/scratch-e/delete_search.cpp -o build/delete_search.exe
./build/delete_search.exe
```

## Results for the report

These numbers come from Windows 11 and MinGW-w64 g++ 16.2.0 with `-O2`. They are deterministic and use no random seed. The random split trials use seed 253.

| n | insert rotations | per insert | delete rotations (`rotation_deletes`) | per delete | starter delete order |
|---:|---:|---:|---:|---:|---:|
| 100 | 93 | 0.930 | 93 | 0.930 | 44 |
| 1 000 | 990 | 0.990 | 990 | 0.990 | 491 |
| 10 000 | 9 986 | 0.9986 | 9 986 | 0.9986 | 4 988 |
| 1 000 000 | 999 980 | 1.000 | 999 980 | 1.000 | 499 981 |

The rows for n = 100 to 10,000 come from `adversarial.exe`, which audits after every step. The 1,000,000 row comes from `delete_search.exe`, which runs without per-step audits.

- **Inserts.** The count equals `n - bitlen(n)` exactly, as proved in D5 Theorem 1.
- **Deletes.** `rotation_deletes` reaches at least `n - bitlen(n) - 1` at every n up to 3000. That's about twice the starter order. This is measured only; there is no proof yet.
- **Correctness.** Every audit, join and split check passed at all sizes tested.

## Change record

The checker now does the following, beyond the starter version:
- reports insert and delete rotations, with and without the stronger delete order;
- checks every split by comparing in-order key sequences;
- tests 10 join shapes, plus random pivot and score splits.

It also checks that `Avl::join` rejects overlapping keys and leaves its inputs unchanged. Run against the starter AVL, which had no such check, this test fails with "join accepted it". The stronger delete order replaces the starter one. The derivation is authored under Jay Gupta's name.
