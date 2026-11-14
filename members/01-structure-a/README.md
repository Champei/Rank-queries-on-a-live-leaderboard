# Member 1: Structure A

**Owner: Chaitanya Sharma**

This contribution implements an order statistic AVL tree for a leaderboard. It orders higher scores first, then smaller player IDs, and stores both height and subtree count in every node. It includes insertion, erasure, competition rank, one-based selection, inclusive rank ranges, ordered join, pair split, score split, and a full invariant audit.

## Files

- `src/structA/avl.hpp`, `src/structA/avl.cpp`: implementation and public interface.
- `INTERFACE.md`: API behavior, balancing, ownership, preconditions, and complexity.
- `docs/derivations/D1.md`: ordered join derivation, diagrams, proof, and verification steps.
- `docs/derivations/D1.pdf`: exported derivation.
- `scratch/scratch-a/avl_random.cpp`: randomized correctness checks against a practice model.
- `TOOLS.md`: implementation and tool-use record.

## Build and run

From the repository root with a C++17 compiler:

```powershell
g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic -I. src/structA/avl.cpp scratch/scratch-a/avl_random.cpp -o scratch/scratch-a/avl_random.exe
./scratch/scratch-a/avl_random.exe
```

This harness checks 40,000 seeded insertions and deletions, rank, select, range, audit, split, and join. The assignment's separate adversarial integration check depends on Member 5's module.

## Change record

The implementation validates ordered-join boundaries before transferring ownership. The accompanying harness exercises split and rejoin, rejected joins, and core order-statistic operations. The derivation is authored under Chaitanya Sharma's name.
