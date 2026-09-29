"""Button-only pilot used to exercise the shipped ROM's mission objectives.

Navigation observes the LCD tile map and visible sprites. It never writes ROM
or game RAM and never bypasses a success/failure transition.
"""
from collections import deque
from rom_harness import Console

def steering(dx,dy):
    buttons=[]
    if dx>2:buttons.append('right')
    elif dx<-2:buttons.append('left')
    if dy>2:buttons.append('down')
    elif dy<-2:buttons.append('up')
    return buttons

def path_direction(console,target):
    px,py=console.player()
    def clear(p):
        x,y=p
        return all((console.emu.tilemap_background[(x+dx)//8,(y+dy)//8]&255) not in (100,101,102,103) for dx,dy in [(-4,-4),(4,4)])
    if not hasattr(console,'nav_nodes'):
        console.nav_nodes={(x,y) for x in range(16,145,8) for y in range(32,113,8) if clear((x,y))}
    nodes=console.nav_nodes
    start=min(nodes,key=lambda p:abs(p[0]-px)+abs(p[1]-py))
    goal=min(nodes,key=lambda p:abs(p[0]-target[0])+abs(p[1]-target[1]))
    queue=deque([start]);parent={start:None}
    while queue:
        p=queue.popleft()
        if p==goal:break
        for dx,dy in [(8,0),(-8,0),(0,8),(0,-8)]:
            q=p[0]+dx,p[1]+dy
            if q in nodes and q not in parent:parent[q]=p;queue.append(q)
    if goal not in parent:return []
    p=goal
    while parent[p] is not None and parent[p]!=start:p=parent[p]
    if abs(px-start[0])+abs(py-start[1])>7:p=start
    return steering(p[0]-px,p[1]-py)

class Pilot:
    def __init__(self,console,mode):
        self.c=console;self.mode=mode;self.route=0;self.frames=0
        self.checkpoint_seen=False;self.checkpoint_time=None

    def step(self):
        c=self.c
        if not c.line(0).startswith('HP'):
            c.hold([],12)
            if not c.line(0).startswith('HP'):return False
        px,py=c.player()
        enemies=[s for i in range(6) if (s:=c.sprite(2+2*i))]
        objects=[s for i in range(4) if (s:=c.sprite(14+2*i))]
        closest=lambda things:min(things,key=lambda s:abs(px-s[0])+abs(py-s[1])) if things else None
        target=closest(enemies);navigation=None;channel=False
        if self.mode in ('RAID','SABOTAGE'):
            towers=[s for s in objects if s[2]==44]
            target=closest(towers)
            if self.mode=='SABOTAGE' and target:
                channel=abs(px-target[0])<16 and abs(py-target[1])<16
                navigation=target[:2]
        elif self.mode=='RESCUE':
            pods=[s for s in objects if s[2]==48]
            pod=closest(pods);navigation=pod[:2] if pod else (80,112)
        elif self.mode=='EVADE':
            gate=closest([s for s in objects if s[2]==70]);navigation=gate[:2] if gate else (80,112)
        elif self.mode=='SURVIVAL':
            route=[(80,32),(136,80),(80,112),(24,80)]
            navigation=route[self.route]
            if abs(px-navigation[0])+abs(py-navigation[1])<14:self.route=(self.route+1)%4
        elif self.mode in ('BOSS','CHASE'):
            target=closest([s for s in enemies if s[2]==56])
            if target:navigation=(min(104,max(56,target[0])),112 if self.mode=='CHASE' else 80)
        repair=closest([s for s in objects if s[2]==64])
        # Repairs are useful even at full hull and provide a natural evasive route.
        if repair and self.mode in ('DEFENSE','CHASE','BOSS') and abs(px-repair[0])+abs(py-repair[1])<60:navigation=repair[:2]
        if target and navigation is None and abs(px-target[0])+abs(py-target[1])>28:navigation=target[:2]
        move=path_direction(c,navigation) if navigation else []
        if channel:
            c.hold(['b'],8)
        elif target:
            dx,dy=target[0]-px,target[1]-py
            if abs(dx)>abs(dy)*2:dy=0
            elif abs(dy)>abs(dx)*2:dx=0
            aim=steering(dx,dy)
            c.hold(aim,2)
            keys=move+['a']
            hostile=[s for i in range(6) if (s:=c.sprite(30+i))]
            if any(abs(px-s[0])+abs(py-s[1])<20 for s in hostile):keys+=['b']
            c.hold(keys,6)
        else:c.hold(move,8)
        self.frames+=8
        if 'CHECKPOINT LOCKED' in c.line(17):
            self.checkpoint_seen=True
            if self.checkpoint_time is None:self.checkpoint_time=int(c.line(0)[16:19])
        return True

def run_all(rom):
    modes=['DEFENSE','RAID','RESCUE','ESCORT','SURVIVAL','SABOTAGE','CHASE','DEFENSE','EVADE','RAID','SURVIVAL','BOSS']
    results=[]
    for index,mode in enumerate(modes):
        c=Console(rom);c.resume(2648)
        for _ in range(11-index):c.tap('left',after=3)
        c.launch();c.hold([],65)
        pilot=Pilot(c,mode)
        for _ in range(1400):
            if not pilot.step():break
        c.hold([],20)
        status='PASS' if 'SORTIE COMPLETE' in c.text() else 'FAIL'
        c.screenshot(f'drill-{index+1:02}-{status.lower()}')
        result={'drill':index+1,'mode':mode,'status':status,'frames':pilot.frames,'checkpoint_seen':pilot.checkpoint_seen,'screen':c.text()}
        results.append(result)
        print(f'Drill {index+1:02} {mode}: {status} after {pilot.frames} frames',flush=True)
        if status=='FAIL':print(c.text(),flush=True)
        c.close()
    return results

if __name__=='__main__':
    import json,sys
    from pathlib import Path
    result=run_all(sys.argv[1] if len(sys.argv)>1 else 'build/rollback-e1-training.gbc')
    Path('build/playthrough.json').write_text(json.dumps(result,indent=2)+'\n')
    raise SystemExit(0 if all(r['status']=='PASS' for r in result) else 1)
