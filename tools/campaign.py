"""Compile the supplied screenplay into banked, paginated native cartridge data.

The Markdown remains the editing source. Typography and emphasis are adapted to
an 18-column font; every spoken word and stage direction is retained. (clue) is
metadata, never dialogue. Gameplay event bindings are checked exhaustively.
"""
import hashlib
import json
import re
import textwrap
import unicodedata
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / 'story/source'
OUT = ROOT / 'build/campaign'
SPEAKERS = ['NARRATION','TEDDY','MAE','FELIX','GUS','DOTTY','VOSS','ACE','AUDITOR',
            'HOLLOWAY','BARLOW','CRANE','DWAYNE','ROOK','PETUNIA','BOMBER LEAD','TOWER',
            'MACHINE','BIG TEDDY','CADET VOSS','TEDDY PAST','BOTH MAES','WARLORD VOSS','VENDOR VOSS']
MOODS = ['NEUTRAL','SMUG','FLAT','ANGRY','PANIC','HAPPY','SAD','GRIM','NEWSREEL']
TRIGGERS = ['enemy_destroyed','multi_kill','taking_damage','low_health','near_miss','wave_incoming',
            'powerup_pickup','core_pickup','retry','idle','objective_complete',
            'Hits the player','Takes heavy damage','Appears on screen','Attacks','Destroyed']

def normalize(value):
    value = value.replace('\\_', '_').replace('\\', '').replace('*', '')
    for before, after in [('—',' - '),('–','-'),('…','...'),('“','"'),('”','"'),('’',"'"),('‘',"'"),('\u00a0',' ')]:
        value=value.replace(before,after)
    value=unicodedata.normalize('NFKD',value).encode('ascii','strict').decode().upper()
    return re.sub(r'\s+', ' ', value).strip()

def cells(line):
    if not line.startswith('|'): return None
    c=[s.strip() for s in line.strip('|').split('|')]
    if not c or set(c[0])<=set('- ') or c[0] in ('Speaker','Trigger','Stage','#','Card','Name','Enemy','Line','Achievement','Screen'):return None
    return c

def speaker(raw,mission):
    m=re.match(r'(.+?)(?: \((.+)\))?$',raw); name,mood=m.groups()
    flags=0
    if mood=='past':name='TEDDY PAST';mood='neutral';flags=4
    if raw=='Mae and Mae (past)':name='BOTH MAES';mood='neutral';flags=4
    if name=='Voss' and 34<=mission<=37:name='WARLORD VOSS'
    if name=='Voss' and 38<=mission<=40:name='VENDOR VOSS'
    return SPEAKERS.index(name.upper()),MOODS.index((mood or 'neutral').upper()),flags

def pages(raw,who='NARRATION',mission=0):
    sid,mood,flags=speaker(who,mission)
    clue=bool(re.search(r'\*?\(clue\)\*?',raw));raw=re.sub(r'\*?\(clue\)\*?','',raw)
    flags |= 2 if clue else 0
    result=[]
    # Italic parentheticals are separate caption cards, never spoken by the actor.
    for part in re.split(r'(\*\([^*]+\)\*)',raw):
        if not part.strip():continue
        caption=part.startswith('*(') and part.endswith(')*')
        text=normalize(part[2:-2] if caption else part)
        lines=textwrap.wrap(text,18,break_long_words=True,break_on_hyphens=True)
        for i in range(0,len(lines),8):
            result.append(dict(speaker=0 if caption else sid,mood=mood,flags=flags|(1 if caption else 0),text='\n'.join(lines[i:i+8])))
    return result

def read_scripts():
    missions={}; current=None;stage=None
    for p in sorted(SOURCE.glob('0[3-6]*.md')):
        for line in p.read_text().splitlines():
            h=re.match(r'#{2,3} Mission (\d+(?:-B)?): (.+)',line)
            if h:
                key,title=h.groups();current=missions.setdefault(key,dict(title=title,brief=[],debrief=[],events={},source=str(p.relative_to(ROOT))))
                stage='brief' if key=='54-B' else None
            if line.startswith('**Briefing'):stage='brief'
            elif line.startswith('**In-mission'):stage='events'
            elif line.startswith('**Debrief'):stage='debrief'
            c=cells(line)
            if not c or current is None:continue
            if len(c)==3:
                trigger,who,text=c;trigger=trigger.replace('\\','')
                current['events'].setdefault(trigger,[]).append((who,text))
            elif len(c)==2 and stage in ('brief','debrief'):current[stage].append(tuple(c))
            else:raise ValueError(f'Unparsed story row in {p}: {line}')
    assert set(missions)=={str(i) for i in range(1,55)}|{'53-B','54-B'}
    missions['53-B']['brief']=missions['53']['brief']
    return missions

# The thresholds correspond to observable level milestones, not a timer that
# substitutes for play. Optional damage chatter does not block later exchanges.
BINDINGS={
1:{'wave_2':('KILLS',2),'holloway_down':('KILLS',4),'final_wave':('KILLS',6)},
2:{'ambush':('PROGRESS',1),'convoy_hit':('HIT',1)},
3:{'vault_breached':('CONTACT',0),'alarm':('PROGRESS',1)},
4:{'wave_2':('KILLS',3),'close_call':('KILLS',7)},
5:{'hauler_exposed':('CONTACT',0)},
6:{'midpoint':('TICKS',22*60)},
7:{'auditor_appears':('PROGRESS',1),'chase_mid':('PROGRESS',2)},
8:{'ace_broadcast':('PROGRESS',1),'engine_room':('PROGRESS',2)},
9:{'jump':('START',0),'arrival':('TICKS',60),'glitch':('TICKS',15*60)},
10:{'wave_2':('KILLS',3),'door_breach':('KILLS',6),'barlow_down':('KILLS',9)},
11:{'waypoint_4':('PROGRESS',3)},
12:{'boss_phase_1':('CONTACT',0)},
13:{'half_cleared':('KILLS',2)},
14:{'wave_2':('PROGRESS',2)},
15:{'seeder_revealed':('CONTACT',0),'seeder_damaged':('PROGRESS',1)},
17:{'chase_mid':('PROGRESS',2),'last_boat':('PROGRESS',4)},
18:{'seeders_arrive':('KILLS',4)},
19:{'seeders_on_roof':('PROGRESS',1)},
20:{'near_detection':('PROGRESS',1)},
21:{'countdown_mid':('PROGRESS',1)},
22:{'past_contact':('PROGRESS',1),'gus_contact':('PROGRESS',2),'collision':('PROGRESS',3)},
23:{'glitch_1':('TICKS',10*60),'glitch_2':('TICKS',25*60),'glitch_3':('TICKS',40*60)},
24:{'wave_2':('KILLS',4)},
25:{'code_contact':('CONTACT',0),'servers_hit':('PROGRESS',1)},
26:{'boss_phase_1':('CONTACT',0),'boss_phase_2':('BOSS',24),'countdown':('BOSS',12),'seeder_down':('PROGRESS',1),'auditor_down':('PROGRESS',2)},
27:{'boxed_in':('PROGRESS',1)},
28:{'statue_reveal':('CONTACT',0),'core_vault':('PROGRESS',2)},
29:{'wave_2':('KILLS',4),'final_wave':('KILLS',8)},
31:{'chase_mid':('PROGRESS',2),'last_blimp':('PROGRESS',3)},
32:{'checkpoint_2':('PROGRESS',2),'checkpoint_3':('PROGRESS',3),'arrival':('DONE',0)},
34:{'evade_mid':('PROGRESS',2)},
35:{'valley_reveal':('CONTACT',0)},
36:{'cages_open':('PROGRESS',1)},
38:{'wave_2':('TICKS',20*60)},
39:{'wave_2':('KILLS',2)},
40:{'file_access':('CONTACT',0)},
41:{'wave_1':('TICKS',5*60),'wave_2':('TICKS',20*60)},
42:{'drifting':('TICKS',5*60)},
43:{'chase_mid':('PROGRESS',2),'chase_late':('PROGRESS',3)},
44:{'wave_1':('KILLS',2),'wave_2':('KILLS',5),'wave_3':('KILLS',9)},
45:{'dogfight_1':('PROGRESS',1),'dogfight_2':('PROGRESS',2)},
46:{'wave_2':('PROGRESS',2)},47:{'wave_2':('KILLS',4)},
48:{'plant':('CHANNEL',1)},49:{'factory_mid':('PROGRESS',1)},
51:{'chase_mid':('PROGRESS',2)},
52:{'wave_2':('TICKS',20*60),'wave_3':('TICKS',40*60)},
53:{'boss_phase_1':('CONTACT',0)},
54:{'wave_2':('KILLS',2),'holloway_down':('KILLS',4),'final_wave':('KILLS',6)}}
FINALS={'objective','core_secured','storm_ends','escaped','timer_end','jump','boss_retreat','launch','past_jump','seeder_flees','midnight','boss_down','survive_end','fail'}
def binding(n,trigger):
    if trigger in BINDINGS.get(n,{}):return BINDINGS[n][trigger]
    if trigger=='start':return ('START',0)
    if trigger in FINALS:return ('DONE',0)
    if trigger=='boss_phase_2':return ('BOSS',24)
    if trigger=='boss_phase_3':return ('BOSS',12)
    raise ValueError(f'Missing gameplay binding M{n}: {trigger}')

def cstr(s):return json.dumps(s,ensure_ascii=True)
def write_pages(out,name,content):
    for i,p in enumerate(content):out.append(f'static const char {name}_text_{i}[] = {cstr(p["text"])};')
    out.append(f'const StoryPage {name}[] = {{')
    for i,p in enumerate(content):out.append('{%d,%d,%d,%s_text_%d},'%(p['speaker'],p['mood'],p['flags'],name,i))
    if not content:out.append('{0,0,0,0},')
    out.append('};')

def compile_campaign():
    OUT.mkdir(parents=True,exist_ok=True)
    for p in OUT.glob('*.c'):p.unlink()
    source=read_scripts(); levels=json.loads((ROOT/'data/campaign.json').read_text())['missions']
    index=['#include "game.h"'];refs=[];manifest=[]
    for i,key in enumerate([str(n) for n in range(1,55)]+['53-B','54-B']):
        n=int(key.split('-')[0]);m=source[key];level=levels[n-1].copy()
        if key=='53-B':level['rule']='TALK';level['hint']='B TALK AT EACH PHASE'
        if key=='54-B':level['setting']='DAY ZERO, 2089';level['layout']=9
        content=[]
        def add(lines):
            first=len(content)
            for who,text in lines:content.extend(pages(text,who,n))
            count=len(content)-first
            if count>255:raise ValueError('Scene too large')
            return (first,count)
        # Episode crawls are separate title cards in the archive, inserted by main.
        briefing=add([('NARRATION',m['title']),('NARRATION',level['setting'])]+m['brief'])
        objective=next(c[1] for line in (SOURCE/'08 On-Screen Text.md').read_text().splitlines() if (c:=cells(line)) and c[0]==str(n))
        if key=='53-B':objective='Talk him down'
        if key!='54-B':
            a,b=add([('NARRATION',objective),('NARRATION',level['hint'])]);briefing=(briefing[0],briefing[1]+b)
        debrief=add(m['debrief']);events=[]
        for trigger,lines in m['events'].items():
            scene=add(lines);condition,value=binding(n,trigger);events.append(dict(trigger=trigger,condition=condition,value=value,scene=scene))
        if len(events)>10:raise ValueError('Event mask capacity')
        name=f'mission_{i+1:02}';out=['#pragma bank 255','#include "game.h"',f'BANKREF({name})']
        write_pages(out,name+'_pages',content)
        out.append(f'static const StoryEvent {name}_events[] = {{')
        out += ['{C_%s,%d,{%d,%d}},'%(e['condition'],e['value'],*e['scene']) for e in events]
        if not events:out.append('{0,0,{0,0}},')
        out.append('};')
        strings=[normalize(m['title']),normalize(objective),level['hint'],level['setting']]
        for s,limit in zip(strings,[40,100,21,32]):
            if len(s)>=limit:raise ValueError(f'Text exceeds field: {s}')
        pts=level['points']+[[0,0]]*(8-len(level['points']))
        out.append(f'const StoryData {name} = '+'{'+','.join(map(cstr,strings))+',')
        out.append(','.join(str(level[k]) for k in ['type','layout','episode','rule','goal','enemy','boss','seconds'])+',')
        out.append('{%d,%d}, {'%tuple(level['start'])+','.join('{%d,%d}'%tuple(p) for p in pts)+'},')
        out.append('{%d,%d},{%d,%d},%s_pages,%s_events,%d};'%(*briefing,*debrief,name,name,len(events)))
        (OUT/(name+'.c')).write_text('\n'.join(out)+'\n')
        index += [f'BANKREF_EXTERN({name})',f'extern const StoryData {name};']
        refs.append('{BANK(%s),&%s}'%(name,name));manifest.append(dict(id=key,title=m['title'],level=level,brief=briefing,debrief=debrief,events=events,pages=content))
    index += ['const StoryRef campaign[56] = {'+','.join(refs)+'};',
        'const char *const speakers[] = {'+','.join(map(cstr,SPEAKERS))+'};',
        'const char *const moods[] = {'+','.join(map(cstr,MOODS))+'};',
        'const char *const mode_names[9] = {"DEFENSE","RAID","ESCORT","CHASE","SURVIVAL","RESCUE","EVADE","SABOTAGE","BOSS"};']
    compile_barks(index);compile_archive(index)
    (OUT/'index.c').write_text('\n'.join(index)+'\n')
    report={'source_hashes':{p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in sorted(SOURCE.glob('*.md'))},'missions':manifest}
    (ROOT/'build/campaign.json').write_text(json.dumps(report,indent=2)+'\n')
    print(f'Compiled 54 missions + Loop 2; {sum(len(m["pages"]) for m in manifest)} dialogue pages; {sum(len(m["events"]) for m in manifest)} event exchanges')

def compile_barks(index):
    who=None;records=[]
    for line in (SOURCE/'07 Bark Library.md').read_text().splitlines():
        if line.startswith('## '):who=line[3:]
        c=cells(line)
        if not c or who not in ['Teddy','Mae','Felix','Dotty','Gus','Enemies']:continue
        first,last,flags=1,54,0
        if who=='Gus':
            stage,trigger,text=c;first,last={'Early':(4,26),'Mid':(27,40),'Late':(41,53)}[stage];actor='Gus';note=''
        elif who=='Enemies':
            actor,trigger,text=c;note='';first=7
            if actor=='Machine (Episode 4)':actor='Machine';first=41
        else:
            trigger,text,note=c;actor=who
            if who=='Dotty':first=13
            if who=='Felix':first=2
            if 'Ep 2+' in note:first=13
            if '1–40' in note:last=40
            if '13–32' in note:first,last=13,32
            if 'Episode 4' in note:first,last=41,54
            if 'silent treatment' in trigger:trigger='any';first,last=39,46;flags=2
            if actor=='Mae' and not flags:flags=4
        if '(clue)' in text:flags|=1;text=text.replace('*(clue)*','').strip()
        if '(clue)' in note:flags|=1
        if actor=='Felix' and 'Gus hums' in text:first=max(first,4)
        trigger=trigger.replace('\\','')
        records.append(dict(trigger=255 if trigger=='any' else TRIGGERS.index(trigger),speaker=SPEAKERS.index(actor.upper()),first=first,last=last,flags=flags,text=normalize(text)))
    refs=[]
    for i in range(0,len(records),40):
        group=records[i:i+40];name=f'barks_{i//40}'
        out=['#pragma bank 255','#include "game.h"',f'BANKREF({name})',f'const Bark {name}[] = {{']
        out+=['{%d,%d,%d,%d,%d,%s},'%(r['trigger'],r['speaker'],r['first'],r['last'],r['flags'],cstr(r['text'])) for r in group]
        out.append('};');(OUT/(name+'.c')).write_text('\n'.join(out)+'\n')
        index += [f'BANKREF_EXTERN({name})',f'extern const Bark {name}[];'];refs.append('{BANK(%s),%s,%d}'%(name,name,len(group)))
    index+=['const BarkRef bark_pools[] = {'+','.join(refs)+'};',f'const uint8_t bark_pool_count={len(refs)};']
    (ROOT/'build/barks.json').write_text(json.dumps(records,indent=2)+'\n')

def compile_archive(index):
    section='';entries=[]
    enemy_unlock=[1,1,5,8,15,7,13,17,20,24,27,30,31,34,41,45,53]
    enemy=0
    for line in (SOURCE/'08 On-Screen Text.md').read_text().splitlines():
        if line.startswith('## '):section=line[3:]
        c=cells(line)
        if not c:continue
        unlock=1
        if section=='Title screen and episode cards':
            unlock=[1,13,27,41,55][len(entries)];text=' '.join(c)
        elif section=='Power-ups':text=c[0]+'. '+c[1]+'. '+c[2]
        elif section=='Enemy roster':unlock=enemy_unlock[enemy];enemy+=1;text=c[0]+'. '+c[2]
        elif section=='Loading screens':unlock=int(re.search(r'\d+',c[2])[0]);text=c[0]
        elif section=='Achievements':text=c[0]+'. '+c[1]
        else:continue
        entries.append((pages(text),unlock,section,c[0]))
    credits=['ROLLBACK','Story and game direction: Zach. Native game adaptation: Codex. Built with GBDK 2020. Emulator verification: PyBoy.',
             'Timelines broken during production: 11','Time travel consultant: Dr. Felix Brandt (disputed)',
             'Catering: N. Voss Hot Dogs (one timeline only)','No chins were harmed. One was made too small.',
             'Special thanks to Chief Barlow, who held the door.','In memory of Captain Holloway. Twice.']
    entries.append((sum((pages(t) for t in credits),[]),1,'Credits','CREDITS'))
    refs=[]
    for i,(content,unlock,section,title) in enumerate(entries):
        name=f'archive_{i:02}';out=['#pragma bank 255','#include "game.h"',f'BANKREF({name})'];write_pages(out,name,content)
        (OUT/(name+'.c')).write_text('\n'.join(out)+'\n');index += [f'BANKREF_EXTERN({name})',f'extern const StoryPage {name}[];']
        refs.append('{BANK(%s),%s,%d,%d}'%(name,name,len(content),unlock))
    index+=['const ArchiveRef archive[] = {'+','.join(refs)+'};',f'const uint8_t archive_count={len(refs)};']
    (ROOT/'build/archive.json').write_text(json.dumps([dict(id=i,unlock=u,section=s,title=t,pages=p) for i,(p,u,s,t) in enumerate(entries)],indent=2)+'\n')

if __name__=='__main__':compile_campaign()
