# Member 1: Structure A

**Owner: Chaitanya Sharma**

This contribution implements an order statistic AVL tree for a leaderboard. It orders higher scores first, then smaller player IDs, and stores both height and subtree count in every node. It includes insertion, erasure, competition rank, one-based selection, inclusive rank ranges, ordered join, pair split, score split, and a full invariant audit.

## Files

- `src/structA/avl.hpp`, `src/structA/avl.cpp`: implementation and public interface.
- `INTERFACE.md`: API behavior, balancing, ownership, preconditions, and complexity.
- `docs/derivations/D1.md`: ordered join derivation, diagrams, proof, and verification steps.
- `docs/derivations/D1.pdf`: exported derivation.
- `adversary/check.cpp`: adversarial correctness checks.
- `TOOLS.md`: implementation and tool-use record.

## Build and run

From the repository root with a C++17 compiler:

```powershell
New-Item -ItemType Directory -Force build
g++ -std=c++17 -O2 -Wall -Wextra -pedantic -I. src/structA/avl.cpp adversary/check.cpp -o build/adversarial.exe
./build/adversarial.exe 10000
```

The included harness can also be run with a smaller number for a quick check. The 10,000-entry run is the assignment's adversarial integration check.

## Change record

The implementation validates ordered-join boundaries before transferring ownership. The accompanying harness exercises height-unequal join via split and rejoin, rejected joins, and core order-statistic operations. The derivation is authored under Chaitanya Sharma's name. No remote repository details were provided in the input archive, so this package is ready for local review and integration.
