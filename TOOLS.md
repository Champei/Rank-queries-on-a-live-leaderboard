# Tools used

## Member 2: Teerth (250001027) - Structure B

| Tool | Use and changes |
|---|---|
| OpenAI Codex | Assisted with the Fenwick class implementation, D2 proof draft, random tester, PDF export script and documentation. Teerth wrote a standalone practice implementation and requested changes to names, type aliases and loops to match his preferred coding style. |
| Apple clang 17.0.0 (`g++`, C++17) | Codex compiled and ran the application (1,000 players, 5,000 revisions), the existing differential test (10,000 operations, seed 253), the practice example and the sorted-array tester (120,000 random updates, seed 253, plus edge cases). All passed; output is in `scratch/member2-test-results.txt`. |
| ReportLab and PyMuPDF | Generated D2.pdf from D2.md and rendered its pages for visual inspection. |

No benchmark timings were collected in these checks.
