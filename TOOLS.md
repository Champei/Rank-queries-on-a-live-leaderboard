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

## Member 3: Prathamesh Hingol - Architect

| Tool | Use and changes |
|:---:|---|
| OpenAI Codex | Adapted the Member 3 existing code, wrote the application tests, expanded D3, aligned Architect and AVL namespace style with Member 2, and changed player IDs and scores to the existing `ll` alias. Container sizes and positions remain `size_t`. |
| MSYS2 g++ 16.2.0 | Compiled the application, oracle, adversary and benchmark after the type change. The hash and application tests, 10,000-operation differential check, 500-player adversarial check, and replay with 1,000,000 players and 1,000,000 revisions passed. Earlier checks covered 40,000 randomized AVL updates and 120,000 Fenwick updates. |

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
| Claude Code (Claude Opus 5.5) | Assisted with the adversary generators and checker, the delete-order search, the D5 proof draft with insert diagrams, the planted-fault and proof-check scripts, and PDF export. An independent review was run which found no correctness bugs and led to the added tie, join and deletion tests. |
| MinGW-w64 g++ 16.2.0 (C++17) | Compiled and ran the adversarial checker (sizes 1 to 10,000), the delete-order search (every n up to 3,000, plus n = 1,000,000) and 15 planted-fault checks against the AVL on `main`, including debug and `-O3` builds. All passed; output is in `scratch/scratch-e/member5-test-results.txt`. |
| Python, Chrome and PyMuPDF | Checked the D5 lemmas and worked example numerically (`proof_checks.py`), exported D5.pdf from D5.md and rendered its pages for visual inspection. |

No benchmark timings were collected in these checks.

---

## Member 6: Sarthak Pandey (250001063) - Analyst

The benchmark harness and first predicted-cost model entered through PR #12. Sarthak's own tool versions, affected-file review, and representative AI prompts have not been supplied in this checkout; Sarthak must complete this declaration from the tools actually used.

---

## Submission integration, 2026-11-21

| Tool and version | Used by | Used for | Files affected | What was changed or verified |
|---|---|---|---|---|
| OpenAI Codex (model version not recorded) | Prathamesh Hingol | Guideline audit, build and test integration, report drafting | `include/`, `tests/`, `scripts/`, `README.md`, `REPORT.md`, `INTERFACE.md`, `bench/plot.cpp`, `docs/derivations/` | Checked the supplied PDF, moved existing interfaces and tests into required folders, built the complete application, passed a one-million-operation differential run and a one-million-player replay, measured five trials per benchmark point, and marked unverified facts. Team members should review the prose and proofs before submission. |
| MSYS2 MinGW-w64 GCC 16.2.0 | Prathamesh Hingol | C++17 build and runtime checks | `build/` (ignored) | Built application, oracle, adversary, focused tests, benchmark, and plotter with `-O2 -Wall -Wextra -Wpedantic`. |
| Git Bash 2.51.0.windows.2 | Prathamesh Hingol | One-command build and test execution | `scripts/`, `tests/` | Ran the shell scripts; Windows line endings were normalized only for text-output comparisons. |
| Python 3.13.7 standard library | Prathamesh Hingol | SVG XML and PDF text checks | `results/figures/`, `REPORT.pdf`, `docs/derivations/`, `docs/slides/` | Parsed four SVGs, checked benchmark CSV rows, and verified PDF page counts and required text. |
| Chrome 158.0.8127.36 | Prathamesh Hingol | Local PDF export from Markdown-rendered HTML and slide HTML | `REPORT.pdf`, `docs/derivations/*.pdf`, `docs/slides/slides.pdf` | Printed the 15-page report, six one-to-two-page derivations, and nine-slide deck; checked extracted text. |

### Representative AI prompts actually available from this integration

- 2026-11-21, OpenAI Codex: “check if all the submission guidelines are full filled or not , if not give the implmentation plan to fix it”
- 2026-11-21, OpenAI Codex: “change the possible fixes”

Earlier Codex and Claude Code prompts should be added by the respective members from their own conversation records; their exact wording and dates are not available in this checkout.
