# Member 6 Analyst

Your topic is the predicted-cost model, five-trial measurements, figures, and cache evidence. Own `bench/benchmark.cpp`, `plot.cpp`, `REPORT.md`, `results/`, and `docs/derivations/D6.md`. The benchmark compares AVL and Fenwick rank/select across sizes, isolated AVL score split and ordered join against Fenwick rebuilds. It records seed 253, all five trial times, mean, and sample standard deviation. `plot_results` turns the CSV into four SVGs.

The prediction was committed before measurement in `0f3543f`; preserve its provenance in `REPORT.md`. Run `bash scripts/run_all.sh` to regenerate `results/raw/full.csv` and the four figures in `results/figures/`. The report now records the measured means, compiler, processor, and date; RAM and cache sizes still need confirmation. A claim about cache behavior needs profiler output. D6 should compare the model with these measurements before its PDF export.

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

Open a pull request and request a teammate's review. Commit the raw results, figures, completed machine details, and analysis with the integration changes. Record actual tools and prompts in `TOOLS.md`.
