"""Export D2.md using ReportLab; install reportlab and pymupdf in a venv first."""
from pathlib import Path
from html import escape
import re
from reportlab.platypus import SimpleDocTemplate, Paragraph, Spacer, Preformatted, Table, TableStyle
from reportlab.lib.styles import getSampleStyleSheet, ParagraphStyle
from reportlab.lib import colors
from reportlab.lib.enums import TA_LEFT
from reportlab.lib.pagesizes import A4

ROOT = Path(__file__).resolve().parents[1]
source = ROOT / 'docs/derivations/D2.md'
output = ROOT / 'docs/derivations/D2.pdf'
styles = getSampleStyleSheet()
styles.add(ParagraphStyle(name='BodyD2', fontName='Helvetica', fontSize=10.5, leading=15, spaceAfter=8))
styles.add(ParagraphStyle(name='CellD2', fontName='Helvetica', fontSize=9, leading=12))
styles.add(ParagraphStyle(name='CodeD2', fontName='Courier', fontSize=8, leading=11, leftIndent=10, spaceBefore=5, spaceAfter=10))
styles['Title'].fontSize = 20
styles['Title'].leading = 25
styles['Title'].alignment = TA_LEFT
styles['Heading2'].fontSize = 13
styles['Heading2'].leading = 17
styles['Heading2'].spaceBefore = 12

def inline(s):
    s = escape(s.replace('—', '-').replace('’', "'"))
    s = re.sub(r'`([^`]+)`', r'<font name="Courier">\1</font>', s)
    return re.sub(r'\*\*([^*]+)\*\*', r'<b>\1</b>', s)

story = []
lines = source.read_text().splitlines()
i = 0
while i < len(lines):
    line = lines[i]
    if not line.strip():
        i += 1
        continue
    if line.startswith('```'):
        code = []
        i += 1
        while i < len(lines) and not lines[i].startswith('```'):
            code.append(lines[i])
            i += 1
        story.append(Preformatted('\n'.join(code), styles['CodeD2']))
        i += 1
        continue
    if line.startswith('|'):
        rows = []
        while i < len(lines) and lines[i].startswith('|'):
            cells = [c.strip() for c in lines[i].strip('|').split('|')]
            if not all(re.fullmatch(r'[:\- ]+', c) for c in cells):
                rows.append([Paragraph(inline(c), styles['CellD2']) for c in cells])
            i += 1
        widths = [79,64,59,77,204] if len(rows[0]) == 5 else [45,70,185,183]
        table = Table(rows, colWidths=widths, repeatRows=1, hAlign='LEFT')
        table.setStyle(TableStyle([
            ('BACKGROUND',(0,0),(-1,0),colors.HexColor('#e7edf4')),
            ('VALIGN',(0,0),(-1,-1),'TOP'),
            ('GRID',(0,0),(-1,-1),0.4,colors.HexColor('#bcc6d2')),
            ('LEFTPADDING',(0,0),(-1,-1),6),('RIGHTPADDING',(0,0),(-1,-1),6),
            ('TOPPADDING',(0,0),(-1,-1),6),('BOTTOMPADDING',(0,0),(-1,-1),6),
        ]))
        story.extend([table, Spacer(1,10)])
        continue
    if line.startswith('# '):
        style, content = styles['Title'], line[2:]
    elif line.startswith('## '):
        style, content = styles['Heading2'], line[3:]
    else:
        style, content = styles['BodyD2'], line
        while i+1 < len(lines) and lines[i+1].strip() and not lines[i+1].startswith(('#','|','```')) and not re.match(r'^\d+\. ', lines[i+1]):
            i += 1
            content += ' ' + lines[i]
    story.append(Paragraph(inline(content), style))
    i += 1

def footer(canvas, doc):
    canvas.setFont('Helvetica',8)
    canvas.setFillColor(colors.HexColor('#536174'))
    canvas.drawString(56,29,'Teerth (250001027) | Structure B | D2')
    canvas.drawRightString(A4[0]-56,29,str(doc.page))

SimpleDocTemplate(str(output), pagesize=A4, rightMargin=56, leftMargin=56,
                 topMargin=45, bottomMargin=48, title='D2: Correctness of Fenwick binary lifting',
                 author='Teerth (250001027)').build(story,onFirstPage=footer,onLaterPages=footer)
print(output)

import pymupdf
render_dir = Path('/private/tmp/teerth-d2-preview')
render_dir.mkdir(exist_ok=True)
doc = pymupdf.open(output)
for n,page in enumerate(doc):
    page.get_pixmap(matrix=pymupdf.Matrix(1.3,1.3)).save(render_dir/f'page-{n+1}.png')
text = '\n'.join(page.get_text() for page in doc)
assert 'Teerth' in text and 'Why the allowed universe' in text and 'loop' in text.lower()
print(f'{len(doc)} pages; text extraction checks passed; previews: {render_dir}')
