"""Build the interim report from docs/MILESTONE_1_REPORT.md.
Documentation-only dependencies: python-docx, reportlab and pypdf.
No game code or assets are changed. PDF pagination is checked after export.
"""
from pathlib import Path
from xml.sax.saxutils import escape
from docx import Document
from docx.shared import Inches, Pt, RGBColor
from docx.oxml import OxmlElement
from docx.oxml.ns import qn
from reportlab.lib import colors
from reportlab.lib.enums import TA_LEFT
from reportlab.lib.pagesizes import A4
from reportlab.lib.styles import ParagraphStyle
from reportlab.platypus import SimpleDocTemplate, Paragraph, PageBreak
from pypdf import PdfReader

root = Path(__file__).resolve().parents[1]
source = root / 'docs/MILESTONE_1_REPORT.md'
blocks = []
for raw in source.read_text().split('\n\n'):
    raw = raw.strip()
    if not raw:
        continue
    if raw == '<!-- pagebreak -->':
        blocks.append(('break', ''))
    elif raw.startswith('# '):
        lines = raw.splitlines()
        blocks.append(('title', lines[0][2:]))
        for line in lines[1:]:
            blocks.append(('subtitle', line))
    elif raw.startswith('## '):
        lines = raw.splitlines()
        blocks.append(('heading', lines[0][3:]))
        if len(lines) > 1:
            blocks.append(('body', ' '.join(lines[1:])))
    else:
        blocks.append(('reference' if raw.startswith('References:') else 'body', ' '.join(raw.splitlines())))

# Editable Word draft. PDF below is the checked pagination reference.
doc = Document()
section = doc.sections[0]
section.page_width = Inches(8.2677)
section.page_height = Inches(11.6929)
section.top_margin = Inches(.62)
section.bottom_margin = Inches(.62)
section.left_margin = section.right_margin = Inches(.68)
section.header_distance = section.footer_distance = Inches(.28)
normal = doc.styles['Normal']
normal.font.name = 'Arial'
normal.font.size = Pt(10.5)
normal.paragraph_format.line_spacing = 1.05
normal.paragraph_format.space_after = Pt(6)
normal.paragraph_format.widow_control = True
for name in ('Heading 1', 'Heading 2'):
    style = doc.styles[name]
    style.font.name = 'Arial'
    style.font.size = Pt(11)
    style.font.color.rgb = RGBColor.from_string('172C42')
    style.paragraph_format.space_before = Pt(7)
    style.paragraph_format.space_after = Pt(3)
    style.paragraph_format.keep_with_next = True
header = section.header.paragraphs[0]
header.text = 'THE HOUSE COLLECTS  /  IN3026  /  INTERIM REPORT DRAFT'
header.style = doc.styles['Normal']
for run in header.runs:
    run.font.size = Pt(8)
    run.font.color.rgb = RGBColor.from_string('67717D')
footer = section.footer.paragraphs[0]
footer.text = 'Draft for student review  •  '
field = OxmlElement('w:fldSimple')
field.set(qn('w:instr'), 'PAGE')
footer._p.append(field)
for run in footer.runs:
    run.font.size = Pt(8)
for kind, value in blocks:
    if kind == 'break':
        doc.add_page_break()
    elif kind == 'heading':
        doc.add_heading(value, level=1)
    else:
        paragraph = doc.add_paragraph(value)
        if kind == 'title':
            paragraph.paragraph_format.space_after = Pt(4)
            paragraph.paragraph_format.keep_with_next = True
            for run in paragraph.runs:
                run.font.size = Pt(23)
                run.font.bold = True
                run.font.color.rgb = RGBColor.from_string('172C42')
        elif kind == 'subtitle':
            paragraph.paragraph_format.space_after = Pt(3)
            paragraph.paragraph_format.keep_with_next = True
            for run in paragraph.runs:
                run.font.size = Pt(9)
                run.font.color.rgb = RGBColor.from_string('67717D')
        elif kind == 'reference':
            for run in paragraph.runs:
                run.font.size = Pt(9)
doc.core_properties.title = 'The House Collects — Milestone 1 interim report'
doc.core_properties.subject = 'AI-assisted draft for student review'
doc.core_properties.author = ''
doc.save(source.with_suffix('.docx'))

styles = {
    'title': ParagraphStyle('title', fontName='Helvetica-Bold', fontSize=23, leading=27, textColor=colors.HexColor('#172C42'), spaceAfter=5, keepWithNext=True),
    'subtitle': ParagraphStyle('subtitle', fontName='Helvetica', fontSize=9, leading=11, textColor=colors.HexColor('#67717D'), spaceAfter=3, keepWithNext=True),
    'heading': ParagraphStyle('heading', fontName='Helvetica-Bold', fontSize=11, leading=13, textColor=colors.HexColor('#172C42'), spaceBefore=8, spaceAfter=4, keepWithNext=True),
    'body': ParagraphStyle('body', fontName='Helvetica', fontSize=10.5, leading=13, spaceAfter=7, alignment=TA_LEFT),
    'reference': ParagraphStyle('reference', fontName='Helvetica', fontSize=9, leading=11, spaceAfter=4),
}

def page_frame(canvas, document):
    canvas.saveState()
    canvas.setFont('Helvetica', 8)
    canvas.setFillColor(colors.HexColor('#67717D'))
    canvas.drawString(49, 819, 'THE HOUSE COLLECTS  /  IN3026  /  INTERIM REPORT DRAFT')
    canvas.drawString(49, 25, 'Draft for student review')
    canvas.drawRightString(A4[0]-49, 25, str(document.page))
    canvas.restoreState()

pdf = source.with_suffix('.pdf')
document = SimpleDocTemplate(str(pdf), pagesize=A4, leftMargin=49, rightMargin=49, topMargin=43, bottomMargin=42,
                             title=doc.core_properties.title, author='')
story = [PageBreak() if kind == 'break' else Paragraph(escape(value), styles[kind]) for kind, value in blocks]
document.build(story, onFirstPage=page_frame, onLaterPages=page_frame)
reader = PdfReader(pdf)
assert len(reader.pages) == 2, f'Report must be two pages; got {len(reader.pages)}'
assert len(Document(source.with_suffix('.docx')).paragraphs) > 10
print(f'Generated editable Word draft and verified {len(reader.pages)}-page PDF; {len(source.read_text().split())} source words.')
