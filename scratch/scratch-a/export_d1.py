"""Export docs/derivations/D1.md to D1.pdf using only Python's standard library."""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
src = ROOT / 'docs/derivations/D1.md'
out = ROOT / 'docs/derivations/D1.pdf'
W, H = 595, 842
pages, commands, y = [], [], 0

def esc(s):
    return s.replace('\\', '\\\\').replace('(', '\\(').replace(')', '\\)')

def new_page():
    global commands, y
    if commands:
        pages.append('\n'.join(commands))
    commands = ['0.10 0.16 0.25 rg',
                'BT /F2 9 Tf 48 808 Td (STRUCTURE A  /  D1 DERIVATION) Tj ET',
                '0.16 0.43 0.55 rg', '48 798 m 547 798 l S']
    y = 770

def text_line(s, size=10, font='F1', color='0.16 0.20 0.27'):
    global y
    commands.extend([f'{color} rg', f'BT /{font} {size} Tf 48 {y:.1f} Td ({esc(s)}) Tj ET'])
    y -= size * 1.52

def wrap(text, max_chars=91):
    text = re.sub(r'`([^`]*)`', r'\1', text)
    text = re.sub(r'\*\*(.*?)\*\*', r'\1', text)
    result, line = [], ''
    for word in text.split():
        candidate = word if not line else line + ' ' + word
        if len(candidate) > max_chars and line:
            result.append(line); line = word
        else:
            line = candidate
    if line: result.append(line)
    return result

def need(amount=62):
    if y < max(amount, 62): new_page()

new_page()
in_code = False
for raw in src.read_text(encoding='utf-8').splitlines():
    if raw.strip().startswith('```'):
        in_code = not in_code
        continue
    line = raw.rstrip() if in_code else raw.strip()
    if not line:
        y -= 4
        continue
    if in_code:
        need(20); text_line(line.expandtabs(4), 8, 'F3', '0.22 0.32 0.42')
    elif line.startswith('# '):
        need(52); text_line(line[2:], 21, 'F2', '0.08 0.25 0.40'); y -= 8
    elif line.startswith('**Author:**'):
        text_line('Author: Chaitanya Sharma  |  Order: score descending, player ID ascending',
                  10, 'F2', '0.16 0.43 0.55')
    elif line.startswith('## '):
        need(48); y -= 6; text_line(line[3:], 14, 'F2', '0.08 0.25 0.40'); y -= 3
    elif line.startswith('**') and line.endswith('**'):
        text_line(line.strip('*'), 10, 'F2')
    elif line.startswith('|'):
        continue
    elif line.startswith('+') or line.startswith('Before:') or line.startswith('After:') or line.startswith('      '):
        need(20); text_line(line, 9, 'F3', '0.22 0.32 0.42')
    else:
        for part in wrap(line):
            need(22); text_line(part, 9.5, 'F1')

if commands: pages.append('\n'.join(commands))
final_pages = [content + f"\n0.35 0.40 0.47 rg\nBT /F1 8 Tf 48 28 Td (Chaitanya Sharma  |  Order Statistic AVL) Tj ET\nBT /F1 8 Tf 520 28 Td ({i} / {len(pages)}) Tj ET"
               for i, content in enumerate(pages, 1)]
objects = []
def add(obj):
    objects.append(obj if isinstance(obj, bytes) else obj.encode('latin-1'))
    return len(objects)
font1 = add('<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica >>')
font2 = add('<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica-Bold >>')
font3 = add('<< /Type /Font /Subtype /Type1 /BaseFont /Courier >>')
content_ids = []
for content in final_pages:
    data = content.encode('latin-1', errors='replace')
    content_ids.append(add(f'<< /Length {len(data)} >>\nstream\n'.encode('latin-1') + data + b'\nendstream'))
page_ids = [add(f'<< /Type /Page /Parent PAGESREF /MediaBox [0 0 {W} {H}] /Resources << /Font << /F1 {font1} 0 R /F2 {font2} 0 R /F3 {font3} 0 R >> >> /Contents {cid} 0 R >>')
            for cid in content_ids]
pages_id = add('<< /Type /Pages /Kids [' + ' '.join(f'{pid} 0 R' for pid in page_ids) + f'] /Count {len(page_ids)} >>')
catalog_id = add(f'<< /Type /Catalog /Pages {pages_id} 0 R >>')
for i, obj in enumerate(objects):
    objects[i] = obj.replace(b'PAGESREF', f'{pages_id} 0 R'.encode('ascii'))
pdf = bytearray(b'%PDF-1.4\n%\xe2\xe3\xcf\xd3\n')
offsets = [0]
for i, obj in enumerate(objects, 1):
    offsets.append(len(pdf)); pdf.extend(f'{i} 0 obj\n'.encode('ascii'))
    pdf.extend(obj); pdf.extend(b'\nendobj\n')
xref = len(pdf)
pdf.extend(f'xref\n0 {len(objects)+1}\n'.encode('ascii'))
pdf.extend(b'0000000000 65535 f \n')
for off in offsets[1:]: pdf.extend(f'{off:010d} 00000 n \n'.encode('ascii'))
pdf.extend(f'trailer\n<< /Size {len(objects)+1} /Root {catalog_id} 0 R >>\nstartxref\n{xref}\n%%EOF\n'.encode('ascii'))
out.write_bytes(pdf)
print(f'Wrote {out} ({len(pages)} pages, {len(pdf)} bytes)')
