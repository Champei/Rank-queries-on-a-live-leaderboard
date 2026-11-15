# Design and measurement report

This is the Analyst's working report. Keep the predictions below before recording measurements. Replace bracketed fields after the team has run the full experiments.

## Design decisions

The AVL orders pairs `(score descending, ID ascending)` and stores subtree counts for rank and selection. Fenwick counts score coordinates and answers the selected **score**; the AVL supplies the corresponding player ID. The application uses a hand written open addressing table. `INTERFACE.md` records API preconditions. AVL join is valid only for disjoint ordered key ranges; arbitrary regional league merging inserts entries one at a time.

## Predicted cost model, written before measuring

Let `n` be live players, `U` distinct possible scores, `m` entries in a merged league, and `k` returned range entries. AVL rank and select: approximately `a0 + a1 log2(n)` time due to pointer based tree descent. Fenwick rank and binary lifting select: approximately `b0 + b1 log2(U)` time with contiguous arrays. Constants `a1` and `b1` depend on the machine and compiler; do not set them from measured data before recording these predictions. AVL rank range: `O(log n + k)`. AVL split: `O(log n)` via joining the path pieces. AVL ordered join: `O(|h1-h2|+1)`. Fenwick division split or join: rebuild counts, `O(n log U)` with this implementation. Arbitrary overlapping league merge: `O(m log(n+m))` AVL insertion plus Fenwick updates.

Prediction: when `U` is close to `n`, Fenwick rank/select should have better locality but not necessarily lower wall time on every machine. A stand alone AVL split should become much cheaper than Fenwick rebuilding as `n` grows. Full application split includes rebuilding both player hash tables and Fenwick trees, so it cannot inherit AVL's isolated bound.

## Measurement protocol

Run `benchmark --full results/full.csv` on one stated machine. The harness uses seed 253 and five trials per point, recording each trial, mean, and sample standard deviation. Each AVL split/join trial averages 1000 repetitions because a single operation is too short for a stable timer reading. Run `leaderboard_app 1000000 1000000` for the full replay, plus the one million operation differential command from README. Record processor [ ], cache sizes [ ], RAM [ ], OS [ ], compiler and flags [ ], date [ ]. If discussing cache misses, attach profiler output from `perf stat -e cache-misses,cache-references` on Linux or a comparable profiler; wall time alone cannot support a cache claim.

## Results to complete

Insert tables or figures generated from `results/full.csv`. Plot AVL versus Fenwick rank/select latency against `n`, and AVL split/ordered join versus Fenwick rebuild. Explain gaps between the predicted curves and each measured mean, including standard deviation. Include rotation counts and adversarial full tree audit results. Record all seeds. Do not invent measured results.
