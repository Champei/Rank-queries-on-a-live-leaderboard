# Member 4: Herin Patel - Oracle

This contribution checks the integrated leaderboard against a sorted-array reference. The reference independently orders higher scores first and smaller player IDs first within ties. Competition rank counts only players with strictly higher scores; positions are one based.

## Files

- `oracle/reference.hpp`, `oracle/reference.cpp`: sorted-array reference for revise, remove, rank, select, and inclusive position ranges. Standard-library containers are permitted in this test module.
- `oracle/differential.cpp`: seeded differential runner, targeted edge cases, full-tree audits, and failure reduction by deleting operation chunks.
- `docs/derivations/D4.md`: rotation metadata and selection proof, authored by Herin Patel.
- `scratch/scratch-d/member4-test-results.txt`: validation record and any environment limitations.
- `TOOLS.md`: concise tool-use record.

## Correctness checks

The targeted checks cover empty leaderboards, missing IDs, invalid positions and ranges, negative IDs, equal scores, ID tie order, repeated revisions, deletion back to empty, and score splits followed by merges. Split checks include thresholds below, inside, and above the score range and confirm that consumed sources are empty.

The random runner compares sizes after each operation, samples selection and rank, checks position ranges periodically, and invokes the integrated AVL, Fenwick, and hash audit. It compares every final position and rank against the reference. Removal outcomes are checked independently so an exception from one implementation cannot conceal disagreement.

The runner accepts an operation count, ID-pool size, and seed, defaulting to 10,000, 1,000, and 253. Random scores span 0 through 10 to exercise ties. A bounded ID pool keeps the linear sorted-array reference feasible for long traces. A failure returns a nonzero exit code and prints a reduced operation sequence; reduction does not promise a globally smallest counterexample.

Build with a compiler supporting the project's C++17 features, including `std::optional` and structured bindings. These are correctness checks; no benchmark timings are claimed. The existing application and structure implementations are unchanged.
