"""Independent screenplay-to-screen contract; expected dialogue comes from supplied Markdown."""
import re
from pathlib import Path

def words(text):
    text=text.replace('’',"'").replace('‘',"'").replace('*','')
    return ' '.join(re.findall(r"[A-Z0-9]+(?:'[A-Z]+)?",text.upper()))

def expected_dialogue(mission,section):
    key=str(mission);active=False;part='';out=[]
    if key=='53-B' and section=='brief':key='53'
    for path in sorted(Path('story/source').glob('0[3-6]*')):
        for line in path.read_text().splitlines():
            heading=re.match(r'#{2,3} Mission (\d+(?:-B)?):',line)
            if heading:active=heading[1]==key;part='brief' if heading[1]=='54-B' else ''
            if not active:continue
            if line.startswith('**Briefing'):part='brief'
            elif line.startswith('**Debrief'):part='debrief'
            elif line.startswith('**In-mission'):part='radio'
            if part!=section or not line.startswith('|'):continue
            cells=[s.strip() for s in line.strip('|').split('|')]
            if cells[0] in ['Speaker','Trigger'] or set(cells[0])<=set('- '):continue
            body=cells[-1].replace('*(clue)*','').replace('(clue)','')
            out.append(body)
    return words(' '.join(out))

def body(c):
    # Radio and captions have no portrait; named character scenes reserve rows 2-5.
    top=5 if 'SQUAD RADIO' in c.line(1) or c.line(3)[1:2]!=' ' else 7
    return words(' '.join(c.line(y) for y in range(top,top+8)))

def wait_visible(c):
    for _ in range(100):
        if c.text().strip():return
        c.hold([],3)
    raise AssertionError('No visible screen returned')

def launch(c,mission):
    c.tap('a');pieces=[];radio=[]
    for _ in range(180):
        wait_visible(c)
        if c.line(0).startswith('HP'):
            c.hold([],15);wait_visible(c)
            if c.line(0).startswith('HP'):break
        text=c.text()
        assert 'BRIEFING' in text or 'SQUAD RADIO' in text,text
        if 'SQUAD RADIO' in text:radio.append(body(c))
        else:pieces.append(body(c))
        c.tap('a',after=3)
    else:raise AssertionError('Scene never launched')
    expected=expected_dialogue(mission,'brief');actual=' '.join(pieces)
    assert expected in actual, f'M{mission} briefing lost or changed authored words\nEXPECTED {expected}\nACTUAL {actual}'
    return radio

def debrief(c,mission):
    # Fresh Start is intentional: held firing may never skip results or dialogue.
    c.tap('start');pieces=[]
    for _ in range(120):
        wait_visible(c)
        if 'DEBRIEF' not in c.line(1):break
        pieces.append(body(c));c.tap('start',after=3)
    else:raise AssertionError('Debrief did not end')
    expected=expected_dialogue(mission,'debrief');actual=' '.join(pieces)
    assert expected==actual, f'M{mission} debrief changed authored words\nEXPECTED {expected}\nACTUAL {actual}'
    return pieces


def radio_exchanges(mission):
    active=False;part='';out={}
    for path in sorted(Path('story/source').glob('0[3-6]*')):
        for line in path.read_text().splitlines():
            heading=re.match(r'#{2,3} Mission (\d+(?:-B)?):',line)
            if heading:active=heading[1]==str(mission);part=''
            if not active:continue
            if line.startswith('**In-mission'):part='radio'
            elif line.startswith('**Debrief'):part=''
            if part!='radio' or not line.startswith('|'):continue
            cells=[s.strip() for s in line.strip('|').split('|')]
            if cells[0]=='Trigger' or set(cells[0])<=set('- '):continue
            trigger=cells[0].replace('\\','')
            out.setdefault(trigger,[]).append(cells[-1].replace('*(clue)*',''))
    return {k:words(' '.join(v)) for k,v in out.items()}
