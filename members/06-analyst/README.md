# Member 6 Analyst

Your topic is the predicted-cost model, five-trial measurements, figures, and cache evidence. Own `bench/benchmark.cpp`, `plot.cpp`, `REPORT.md`, `results/`, and `docs/derivations/D6.md`. The benchmark compares AVL and Fenwick rank/select across sizes, isolated AVL score split and ordered join against Fenwick rebuilds. It records seed 253, all five trial times, mean, and sample standard deviation. `plot_results` turns the CSV into four SVGs.

The prediction is already written in `REPORT.md`; preserve its pre-measurement status. After integration, run `./build/benchmark.exe --full results/full.csv`, then `./build/plot_results.exe results/full.csv results/full`. State processor, caches, compiler, flags, date, and full-scale replay results in the report. A claim about cache behavior needs profiler output. Complete D6 with your name and comparison of predictions with real measurements, then export `docs/derivations/D6.pdf`.

## Push your contribution

Clone the shared repository, open PowerShell in the clone, and run:

```powershell
git switch -c member6-analyst
Expand-Archive -LiteralPath 'C:\path\to\member6_analyst.zip' -DestinationPath . -Force
git add bench REPORT.md docs/derivations/D6.md members/06-analyst/README.md
git diff --cached
git commit -m "Add cost model and benchmark harness"
git push -u origin member6-analyst
```

Open a pull request and request a teammate's review. Commit real `results/full.csv`, generated figures, machine details, and analysis in later commits across the project period. Record your actual changes in `TOOLS.md`.
