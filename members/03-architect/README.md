# Member 3 Architect

Your topic is the common interface, the player ID hash table, and the replay application. Own `INTERFACE.md`, `app/`, and `docs/derivations/D3.md`. The hash uses open addressing, linear probing, tombstones and resizing, with no library map. `Leaderboard` keeps AVL, Fenwick and hash synchronized. Competition ranks share ties; positions use the ID tie break. Score split returns `(> threshold, <= threshold)`.

Review the interface with the other five members before merging structure code. Explain why AVL's ordered join cannot merge arbitrary overlapping regional leagues and why application-level season split rebuilds its Fenwick and hash state. Complete D3 with your name before submission.

## Current check

The current repo has both Member 1's AVL and Member 2's Fenwick modules. Run the standalone hash test:

```powershell
g++ -std=c++17 -Wall -Wextra -Wpedantic -I. app/player_hash.cpp scratch/scratch-c/player_hash_test.cpp -o scratch/scratch-c/player_hash_test.exe
./scratch/scratch-c/player_hash_test.exe
```

The test compares 100,000 deterministic insert, update, delete, lookup, and resize operations against a standard-library map used only as an oracle. A successful run exits zero.

Compile and run the integrated application and its targeted tests:

```powershell
g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic -I. src/structA/avl.cpp src/structB/fenwick.cpp app/player_hash.cpp app/leaderboard.cpp app/main.cpp -o scratch/scratch-c/leaderboard_app.exe
./scratch/scratch-c/leaderboard_app.exe 1000 5000
g++ -std=c++17 -Wall -Wextra -Wpedantic -I. src/structA/avl.cpp src/structB/fenwick.cpp app/player_hash.cpp app/leaderboard.cpp scratch/scratch-c/leaderboard_test.cpp -o scratch/scratch-c/leaderboard_test.exe
./scratch/scratch-c/leaderboard_test.exe
```

For the target replay, run `./scratch/scratch-c/leaderboard_app.exe 1000000 1000000` on a machine with sufficient memory. Keep teammates' files and documentation when integrating other roles.
