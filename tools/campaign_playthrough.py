"""Play native missions via public controls, LCD, and OAM. Never write game RAM."""
import json
from pathlib import Path
from rom_harness import Console
from playthrough import Pilot, path_direction, steering
import story_contract

def resume_code(unlocked,difficulty=1,complete=False,loop=False):
    payload=unlocked+64*complete+128*loop+256*difficulty
    return 100000+payload*97+(payload*13+7)%97

class CampaignPilot(Pilot):
    def __init__(self,c,level):
        self.level=level;mode=level['type']
        if level['rule'] in ['ESCAPE','UPLOAD_RACE']:mode='EVADE'
        if level['id']==9:mode='SURVIVAL'
        super().__init__(c,mode)
        self.radio_pages=[];self.uploading=False

    def drain_radio(self):
        for _ in range(60):
            story_contract.wait_visible(self.c)
            text=self.c.text()
            if 'SQUAD RADIO' not in text:return
            self.radio_pages.append(story_contract.body(self.c))
            if self.level['id']==48 and ('NINETY SECONDS' in ' '.join(text.split()) or 'UPLOADING RULE ZERO' in ' '.join(text.split())):self.uploading=True
            self.c.tap('a',after=3)
        raise AssertionError('Radio did not return to flight')

    def step(self):
        c=self.c;self.drain_radio()
        if 'MISSION COMPLETE' in c.text() or 'COMPROMISED' in c.text():return False
        if self.level['rule']=='RADAR' and ('SWEEP' in c.line(17) or (self.level['seconds']-int(c.line(0)[16:19]))%12>=8):
            px,py=c.player();cx,cy=c.camera(),c.camera_y()
            def solid(x,y):return c.tile(x//8,y//8) in [100,101,102,103]
            options=[]
            for x in range(((cx+8)//8)*8,((cx+152)//8)*8,8):
                for y in range(((cy+8)//8)*8,((cy+104)//8)*8,8):
                    if not any(solid(x+dx,y+dy) for dx,dy in [(-4,-4),(4,4)]) and any(solid(x+dx,y+dy) for dx,dy in [(-16,0),(16,0),(0,-16),(0,16)]):options.append((x,y))
            if options:
                target=min(options,key=lambda p:abs(px-p[0])+abs(py-p[1]))
                move=path_direction(c,target)
                enemies=[s for i in range(6) if (s:=c.sprite(2+2*i))]
                if enemies:
                    enemy=min(enemies,key=lambda p:abs(px-p[0])+abs(py-p[1]))
                    self.fly(steering(enemy[0]-px,enemy[1]-py),2);self.fly(move+['a'],6)
                else:self.fly(move,8)
                self.frames+=8;return True
        if self.level['rule'] in ['ESCAPE','UPLOAD_RACE','RADAR'] or (self.level['rule']=='DUPLICATE' and self.level['id']!=9):
            px,py=c.player();arrows=c.line(16)[10:12]
            gates=[s for i in range(3) if (s:=c.sprite(14+i*2)) and s[2]==70]
            target=gates[0][:2] if gates else (px+(112 if arrows[0:1]=='>' else -112 if arrows[0:1]=='<' else 0),py+(80 if arrows[1:2]=='V' else -80 if arrows[1:2]=='^' else 0))
            move=path_direction(c,target,diagonal=True)
            self.fly(move,2);self.fly(move+['a']+(['b'] if 'B OK' in c.line(0) else []),6);self.frames+=8;return True
        if self.level['rule']=='TALK':
            px,py=c.player();boss=c.sprite(2)
            if boss:
                target=(boss[0]+(44 if (self.frames//100)%2 else -44),boss[1]+40)
                move=path_direction(c,target)
                self.fly(move,2);self.fly(move+['b'],6);self.frames+=8;return True
        if self.level['id']==48 and self.uploading:self.mode='DEFENSE'
        # Break cages before collecting their occupants.
        if self.level['id']==36:
            towers=[s for i in range(3) if (s:=c.sprite(14+i*2)) and s[2]==44]
            self.mode='RAID' if towers else 'RESCUE'
        return super().step()

def run_mission(rom,level,*,loop=False,difficulty=1,max_steps=6000):
    if loop and level['id']==53:level={**level,'rule':'TALK'}
    c=Console(rom);c.resume(resume_code(level['id'],difficulty,loop,loop));initial_radio=story_contract.launch(c,str(level['id'])+('-B' if loop and level['id']>=53 else ''))
    pilot=CampaignPilot(c,level);pilot.radio_pages.extend(initial_radio);recoveries=0
    for _ in range(max_steps):
        if not pilot.step():
            c.hold([],15)
            if 'COMPROMISED' in c.text() and recoveries<5:
                c.tap('start',after=30);recoveries+=1;continue
            if c.line(0).startswith('HP') or 'SQUAD RADIO' in c.text():continue
            break
    c.hold([],15);pilot.drain_radio();status='PASS' if 'MISSION COMPLETE' in c.text() else 'FAIL'
    c.screenshot(f'mission-{level["id"]:02}-{status.lower()}')
    result=dict(mission=level['id'],rule=level['rule'],type=level['type'],status=status,frames=pilot.frames,recoveries=recoveries,radio_pages=len(pilot.radio_pages),screen=c.text())
    if status=='PASS':
        radio=' '.join(pilot.radio_pages)
        for trigger,expected in story_contract.radio_exchanges(str(level['id'])+('-B' if loop and level['id']>=53 else '')).items():
            if trigger!='convoy_hit':assert expected in radio,f'M{level["id"]} missing radio exchange {trigger}: {expected}\nActual {radio}'
        result['authored_radio_verified']=True
        for _ in range(3):c.tap('a')
        assert 'TIMELINE DAMAGE' in c.text(),'Firing skipped results'
        result['debrief_pages']=len(story_contract.debrief(c,str(level['id'])+('-B' if loop and level['id']>=53 else '')))
        if level['id']==54 and not loop:
            assert 'IT HAS ALREADY HAPPENED' in story_contract.words(c.text()),c.text()
            c.screenshot('main-ending')
            for _ in range(40):
                if 'LOOP 2 UNLOCKED' in c.text():break
                c.tap('start')
            assert 'LOOP 2 UNLOCKED' in c.text(),c.text()
            c.tap('start');assert 'BRIEFING' in c.text() and '01' in c.line(1),c.text()
            result['main_ending_and_loop_unlock']=True
    print(f'M{level["id"]:02} {level["rule"]}: {status}; frames={pilot.frames}; rewinds={recoveries}; radio pages={len(pilot.radio_pages)}',flush=True)
    if status=='FAIL':print(c.text(),flush=True)
    c.close();return result

if __name__=='__main__':
    import sys
    levels=json.loads(Path('data/campaign.json').read_text())['missions']
    ids=list(map(int,sys.argv[2:])) or list(range(1,55))
    results=[]
    for n in ids:
        result=run_mission(sys.argv[1],levels[n-1]);results.append(result)
        Path(f'build/playthrough-{ids[0]:02}.json').write_text(json.dumps(results,indent=2)+'\n')
        if result['status']=='FAIL':break
    raise SystemExit(0 if all(r['status']=='PASS' for r in results) else 1)
