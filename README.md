# Rank-queries-on-a-live-leaderboard

This C++17 project compares an order-statistic AVL tree with a coordinate-compressed Fenwick tree on a live leaderboard. Members 1, 2, and 3's modules are present. The oracle, adversary, and benchmark modules are still needed for the complete project.

Member 3's [interface](INTERFACE.md) defines competition ranks, ID tie order for positions, and season operations. The [architect guide](members/03-architect/README.md) has the hash and application test commands. The rank proof is in [D3](docs/derivations/D3.md). Member 1 and 2 guides are in [members/01-structure-a/README.md](members/01-structure-a/README.md) and [members/02-structure-b/README.md](members/02-structure-b/README.md).
