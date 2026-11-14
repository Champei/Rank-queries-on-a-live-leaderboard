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
| Claude Code (Claude Opus 5.5) | Assisted with the adversary generators and checker, the delete-order search, the D5 proof draft with insert diagrams, the planted-fault and proof-check scripts, and PDF export. Jay had each module and proof explained, and asked for an extra verification pass with an independent review. The review found no correctness bugs and led to the added tie, join and deletion tests. |
| MinGW-w64 g++ 16.2.0 (C++17) | Compiled and ran the adversarial checker (sizes 1 to 10,000), the delete-order search (every n up to 3,000, plus n = 1,000,000) and 15 planted-fault checks against the AVL on `main`, including debug and `-O3` builds. All passed; output is in `scratch/scratch-e/member5-test-results.txt`. |
| Python, Chrome and PyMuPDF | Checked the D5 lemmas and worked example numerically (`proof_checks.py`), exported D5.pdf from D5.md and rendered its pages for visual inspection. |

No benchmark timings were collected in these checks.
