# Rank-queries-on-a-live-leaderboard

This C++17 project compares an order-statistic AVL tree with a coordinate-compressed Fenwick tree on a live leaderboard. Member 2's Fenwick module and Member 3's architect module are present. Member 1's AVL module and the oracle, adversary, and benchmark modules are still needed for the complete project.

Member 3's [interface](INTERFACE.md) defines competition ranks, ID tie order for positions, and season operations. The [architect guide](members/03-architect/README.md) has the current hash test and the application build command to use after AVL integration. The rank proof is in [D3](docs/derivations/D3.md). Member 2's guide is in [members/02-structure-b/README.md](members/02-structure-b/README.md).
