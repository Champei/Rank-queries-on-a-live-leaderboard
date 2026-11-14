"""Export docs/derivations/D5.md to D5.pdf.

Converts the Markdown to HTML with the `markdown` package, then prints it to PDF
with headless Google Chrome (or Microsoft Edge). Run from anywhere:
    python scratch/scratch-e/export_d5.py
"""
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile

import markdown

ROOT = Path(__file__).resolve().parents[2]
SRC = ROOT / "docs" / "derivations" / "D5.md"
OUT = ROOT / "docs" / "derivations" / "D5.pdf"

BROWSERS = [  # Chrome first: on the test machine headless Edge exited without printing.
    r"C:\Program Files\Google\Chrome\Application\chrome.exe",
    "google-chrome", "chromium", "chrome",
    r"C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe",
    r"C:\Program Files\Microsoft\Edge\Application\msedge.exe",
    "msedge",
]

CSS = """
@page { size: A4; margin: 18mm 16mm; }
body { font-family: "Segoe UI", Arial, sans-serif; font-size: 10.5pt; line-height: 1.45;
       color: #1d2733; }
h1 { font-size: 19pt; color: #14405f; border-bottom: 2px solid #2a6e8c; padding-bottom: 4px; }
h2 { font-size: 13.5pt; color: #14405f; margin-top: 20px; break-after: avoid; }
code { font-family: Consolas, "Courier New", monospace; font-size: 9.5pt;
       background: #eef2f5; padding: 0 2px; border-radius: 2px; }
pre { background: #f4f7f9; border: 1px solid #d5dde3; padding: 8px 10px; break-inside: avoid;
      font-size: 9pt; line-height: 1.25; }
pre code { background: none; padding: 0; }
table { border-collapse: collapse; margin: 8px 0; break-inside: avoid; font-size: 9.5pt; }
th, td { border: 1px solid #c9d3da; padding: 3px 8px; }
th { background: #e6edf2; }
li { margin: 2px 0; }
"""


def find_browser():
    for candidate in BROWSERS:
        path = shutil.which(candidate) or (candidate if Path(candidate).exists() else None)
        if path:
            return path
    sys.exit("No Edge or Chrome found to print the PDF.")


def github_lists_to_python_markdown(text):
    """D5.md uses GitHub list style: a list may follow a sentence directly, and nested
    items are indented by 2 spaces. Python-Markdown needs a blank line before a list
    and 4-space nesting, so add both. Code blocks are left untouched."""
    out, in_code, prev = [], False, ""
    for line in text.splitlines():
        if line.startswith("```"):
            in_code = not in_code
            out.append(line); prev = line
            continue
        if in_code:
            out.append(line)
            continue
        stripped = line.lstrip(" ")
        indent = len(line) - len(stripped)
        if stripped and indent:
            line = " " * (2 * indent) + stripped
        starts_list = stripped.startswith("- ") and indent == 0
        # Any non-list line before a top-level item, including an indented paragraph
        # that continues the previous item, would otherwise swallow the new item.
        prev_is_text = prev.strip() and not prev.lstrip(" ").startswith("- ")
        if starts_list and prev_is_text:
            out.append("")
        out.append(line); prev = line
    return "\n".join(out)


def main():
    text = github_lists_to_python_markdown(SRC.read_text(encoding="utf-8"))
    body = markdown.markdown(text, extensions=["tables", "fenced_code"])
    html = (f"<!doctype html><html><head><meta charset='utf-8'>"
            f"<title>D5 Split and adversarial rotations</title><style>{CSS}</style>"
            f"</head><body>{body}</body></html>")
    with tempfile.TemporaryDirectory() as tmp:
        page = Path(tmp) / "D5.html"
        page.write_text(html, encoding="utf-8")
        # A separate profile lets this run while the browser is already open.
        result = subprocess.run([find_browser(), "--headless=new", "--disable-gpu",
                                 f"--user-data-dir={Path(tmp) / 'profile'}",
                                 "--no-pdf-header-footer", f"--print-to-pdf={OUT}",
                                 page.as_uri()], capture_output=True, text=True, timeout=120)
        if not OUT.exists():
            sys.exit("The browser did not write the PDF.\n" + result.stderr[-2000:])
    print(f"wrote {OUT}")


if __name__ == "__main__":
    main()
