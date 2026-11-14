# Tools used

## Member 1: Chaitanya Sharma (250001020) - Structure A

| Tool | Use and changes |
|:---:|---|
| OpenAI Codex | Assisted with the AVL implementation updates, INTERFACE.md documentation, D1 proof rewrite with height-case diagrams, adversary test harness and PDF export. Chaitanya reviewed the provided AVL header, implementation, derivation draft and member checklist, and directed changes to join precondition validation and proof structure. |
| MinGW `g++.exe` (C++17) | Attempted to compile and run the 10,000-entry audit in `adversary/check.cpp`. The compiler could not start because Windows reported "Illegal System DLL Relocation"; no alternative C++ compiler is installed on the machine. The harness is included for execution in the integrated build environment. |
| Markdown and PDF export | Replaced the derivation draft with a named proof and height-case diagrams in `docs/derivations/D1.md`, then exported it to PDF. |

No benchmark timings were collected in these checks.

---

## Member 2: Teerth (250001027) - Structure B

| Tool | Use and changes |
|:---:|---|
| OpenAI Codex | Assisted with the Fenwick class implementation, D2 proof draft, random tester, PDF export script and documentation. Teerth wrote a standalone practice implementation and requested changes to names, type aliases and loops to match his preferred coding style. |
| Apple clang 17.0.0 (`g++`, C++17) | Codex compiled and ran the application (1,000 players, 5,000 revisions), the existing differential test (10,000 operations, seed 253), the practice example and the sorted-array tester (120,000 random updates, seed 253, plus edge cases). All passed; output is now in `scratch/scratch-b/member2-test-results.txt`. |
| ReportLab and PyMuPDF | Generated D2.pdf from D2.md and rendered its pages for visual inspection. |

No benchmark timings were collected in these checks.

---

## Member 3: Architect

| Tool | Use and changes |
|:---:|---|
| OpenAI Codex | Adapted the Member 3 starter ZIP, wrote the hash and application tests, expanded D3, aligned Architect and AVL standard-library namespace style with Member 2, and checked integration. |
| MSYS2 g++ 16.2.0 | Compiled and ran the hash and application tests, 40,000 randomized AVL updates, 120,000 Fenwick updates, and a replay with 1,000,000 players and 1,000,000 revisions against the in-repository modules. All passed. |

No benchmark timings were collected in these checks.

---

## Member 4: Herin Patel - Oracle

| Tool | Use and changes |
|:---:|---|
| OpenAI Codex | Integrated the supplied sorted-array oracle, corrected removal exception comparison, added independent ordering and targeted edge cases, expanded D4, and updated the Member 4 and project READMEs. Existing application and structure code was preserved. |
| MinGW g++ 6.3.0 | Attempted the integrated C++17 build. Compilation stopped because this compiler lacks `std::optional` and structured bindings required by the existing project; runtime checks are not claimed. An Ubuntu WSL fallback also had no `g++` installed. See `scratch/scratch-d/member4-test-results.txt`. |

No benchmark timings were collected in these checks.

---

## Member 5: Jay Gupta - Adversary

| Tool | Use and changes |
|:---:|---|
| Claude Code (Anthropic, Claude Opus 5.5) | Explained the starter code and the AVL, Fenwick and application modules. Wrote the `adversary/check.cpp` extensions: rotation reporting and the enforced Theorem 1 count, four delete orders, tied-score tests, split side checks, join shapes and random joins, join boundary tests, and phase-and-step failure messages. Added `ordered_join_case` and `tied_inserts`. Ran a greedy delete search (`scratch/scratch-e/delete_search.cpp`) that produced the new `rotation_deletes` order. Drafted D5: the sorted-insert rotation proof, the join height lemma, the split telescoping proof and the insert diagrams. Wrote `export_d5.py`, `plant_bugs.sh` and `proof_checks.py`. A second Claude agent then reviewed the code and D5 independently and found no correctness bugs. Its points on test coverage, wording and proof detail were fixed. Own review and edits: [Jay to fill in before submission]. |
| MinGW-w64 `g++` 16.2.0 (C++17, `-O2 -Wall -Wextra -Wpedantic`) | Compiled and ran `adversarial.exe` at sizes from 1 to 10,000 and `delete_search.exe` against the AVL on `main`. The checker sizes are listed in the results file. `delete_search.exe` covers every n up to 200 against the greedy search, every n up to 3000 against `n - bitlen(n)`, and n = 10,000 to 1,000,000. All passed. The checker also passed when built with `-O0`, `-O3`, and with debug containers plus undefined-behaviour trapping (`-D_GLIBCXX_DEBUG -fsanitize=undefined -fsanitize-trap=undefined -static`). `plant_bugs.sh` caught all 15 planted faults. Output is in `scratch/scratch-e/member5-test-results.txt`. |
| Python 3.13 | `scratch/scratch-e/proof_checks.py` checks the D5 insert lemmas for every k up to 3000, the worked split example, Lemma 4 on 20,000 random joins, and Theorem 2 on 3,000 random splits. `python-markdown` and headless Chrome export `D5.pdf`, and PyMuPDF rendered its pages for a visual check. |

No benchmark timings were collected in these checks.
