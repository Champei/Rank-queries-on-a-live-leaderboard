# Tools and implementation record

# Member 1: Chaitanya Sharma (250001020) - Structure A
Tool	Use and changes
OpenAI Codex	Assisted with the AVL implementation updates, INTERFACE.md documentation, D1 proof rewrite with height-case diagrams, adversary test harness and PDF export. Chaitanya reviewed the provided AVL header, implementation, derivation draft and member checklist, and directed changes to join precondition validation and proof structure.
MinGW g++.exe (C++17)	Attempted to compile and run the 10,000-entry audit in adversary/check.cpp. The compiler could not start because Windows reported "Illegal System DLL Relocation"; no alternative C++ compiler is installed on the machine. The harness is included for execution in the integrated build environment.
Markdown and PDF export	Replaced the derivation draft with a named proof and height-case diagrams in docs/derivations/D1.md, then exported it to PDF.
No benchmark timings were collected in these checks.

# Member 2: Teerth (250001027) - Structure B
Tool	Use and changes
OpenAI Codex	Assisted with the Fenwick class implementation, D2 proof draft, random tester, PDF export script and documentation. Teerth wrote a standalone practice implementation and requested changes to names, type aliases and loops to match his preferred coding style.
Apple clang 17.0.0 (g++, C++17)	Codex compiled and ran the application (1,000 players, 5,000 revisions), the existing differential test (10,000 operations, seed 253), the practice example and the sorted-array tester (120,000 random updates, seed 253, plus edge cases). All passed; output is in scratch/member2-test-results.txt.
ReportLab and PyMuPDF	Generated D2.pdf from D2.md and rendered its pages for visual inspection.
No benchmark timings were collected in these checks.
