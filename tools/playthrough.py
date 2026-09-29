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

def path_direction(console,target,diagonal=False):
    px,py=console.player()
    def clear(p):
        x,y=p
        if x<12 or x>1587 or y<28 or y>1411:return False
        return all(console.tile((x+dx)//8,(y+dy)//8) not in (100,101,102,103) for dx,dy in [(-4,-4),(4,4)])
    camera=console.camera();camera_y=console.camera_y()
    # Plan inside the actual visible terrain; discovery advances with the camera.
    nodes={(x,y) for x in range(((camera+8)//8)*8,((camera+152)//8)*8,8) for y in range(((camera_y+8)//8)*8,((camera_y+104)//8)*8,8) if clear((x,y))}
    start=min(nodes,key=lambda p:abs(p[0]-px)+abs(p[1]-py))
    goal=min(nodes,key=lambda p:abs(p[0]-target[0])+abs(p[1]-target[1]))
    directions=[(8,0),(-8,0),(0,8),(0,-8)]
    if diagonal:directions += [(8,8),(8,-8),(-8,8),(-8,-8)]
    queue=deque([start]);parent={start:None}
    while queue:
        p=queue.popleft()
        if not diagonal and p==goal:break
        for dx,dy in directions:
            q=p[0]+dx,p[1]+dy
            if dx and dy and ((p[0]+dx,p[1]) not in nodes or (p[0],p[1]+dy) not in nodes):continue
            if q in nodes and q not in parent:parent[q]=p;queue.append(q)
    # Pursuit needs diagonal flight to intercept a moving target. Choose only
    # reachable terrain; a closer isolated tile is a trap.
    if diagonal:goal=min(parent,key=lambda p:(max(abs(p[0]-target[0]),abs(p[1]-target[1])),abs(p[0]-target[0])+abs(p[1]-target[1])))
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
        for _ in range(20):
            if c.line(0).startswith('HP'):break
            if 'SORTIE COMPLETE' in c.text() or 'TIMELINE BROKEN' in c.text():return False
            c.hold([],6)  # Let a full arena/menu redraw finish with neutral input.
        if not c.line(0).startswith('HP'):return False
        px,py=c.player()
        arrows=c.line(16)[10:12]
        hint=(px+(112 if arrows[0:1]=='>' else -112 if arrows[0:1]=='<' else 0),
              py+(80 if arrows[1:2]=='V' else -80 if arrows[1:2]=='^' else 0))
        enemies=[s for i in range(6) if (s:=c.sprite(2+2*i))]
        objects=[s for i in range(4) if (s:=c.sprite(14+2*i))]
        closest=lambda things:min(things,key=lambda s:abs(px-s[0])+abs(py-s[1])) if things else None
        target=closest(enemies);navigation=None;channel=False;pursuit=False
        if self.mode in ('RAID','SABOTAGE'):
            towers=[s for s in objects if s[2]==44]
            target=closest(towers)
            if self.mode=='SABOTAGE' and target:
                channel=abs(px-target[0])<16 and abs(py-target[1])<16
                navigation=target[:2]
                threat=closest(enemies)
                if threat and abs(px-threat[0])+abs(py-threat[1])<32:
                    target=threat;channel=False
            if not towers and self.mode=='SABOTAGE':
                target=closest(enemies)
                navigation=hint
        elif self.mode=='RESCUE':
            pods=[s for s in objects if s[2]==48]
            pod=closest(pods)
            navigation=pod[:2] if pod else ((80,112) if 'HOME' in c.line(16) else None)
        elif self.mode=='EVADE':
            gate=closest([s for s in objects if s[2]==70]);navigation=gate[:2] if gate else None
        elif self.mode=='SURVIVAL':
            route=[(80,80),(720,512),(1440,1088),(720,112)]
            navigation=route[self.route]
            if abs(px-navigation[0])+abs(py-navigation[1])<14:self.route=(self.route+1)%4
        elif self.mode in ('BOSS','CHASE'):
            target=closest([s for s in enemies if s[2]==56])
            if target:navigation=(target[0],target[1]+(32 if self.mode=='CHASE' else 48))
            else:
                target=closest(enemies);navigation=hint
                pursuit=self.mode=='CHASE'
        elif self.mode=='ESCORT':
            transport=closest([s for s in objects if s[2]==52])
            if transport:navigation=(transport[0]+16,transport[1]+24)
        repair=closest([s for s in objects if s[2]==64])
        # Repairs are useful even at full hull and provide a natural evasive route.
        if repair and self.mode not in ('ESCORT','CHASE') and abs(px-repair[0])+abs(py-repair[1])<60:
            navigation=repair[:2];channel=False
        if target and navigation is None and abs(px-target[0])+abs(py-target[1])>28:navigation=target[:2]
        if navigation is None and (not target or self.mode in ('RESCUE','EVADE')):
            navigation=hint
        # Keep searching for distant mission objects even when a local drone is visible.
        if self.mode in ('RESCUE','EVADE') and navigation is None:
            navigation=hint
        move=path_direction(c,navigation,diagonal=self.mode=='CHASE') if navigation else []
        travel_boost=bool(move) and navigation and abs(px-navigation[0])+abs(py-navigation[1])>64 and 'B OK' in c.line(0)
        if channel:
            c.hold(['b'],8)
        elif pursuit:
            # Keep up with the moving mission target, firing along the route
            # instead of turning back toward each pursuing drone.
            c.hold(move,2)
            c.hold(move+['a']+(['b'] if travel_boost else []),6)
        elif target:
            dx,dy=target[0]-px,target[1]-py
            if abs(dx)>abs(dy)*2:dy=0
            elif abs(dy)>abs(dx)*2:dx=0
            aim=steering(dx,dy)
            c.hold(aim,2)
            keys=move+['a']
            hostile=[s for i in range(6) if (s:=c.sprite(30+i))]
            if travel_boost or any(abs(px-s[0])+abs(py-s[1])<20 for s in hostile):keys+=['b']
            c.hold(keys,6)
        else:c.hold(move+(['b'] if travel_boost else []),8)
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
        recoveries=0
        for _ in range(3600):
            if not pilot.step():
                c.hold([],30)
                if 'TIMELINE BROKEN' in c.text() and recoveries<3:
                    c.tap('a',after=50);recoveries+=1
                    continue
                if c.line(0).startswith('HP'):continue
                break
        c.hold([],20)
        status='PASS' if 'SORTIE COMPLETE' in c.text() else 'FAIL'
        c.screenshot(f'drill-{index+1:02}-{status.lower()}')
        mulligans=int(c.line(10)[14:17]) if c.line(10).startswith(' MULLIGANS') else None
        result={'drill':index+1,'mode':mode,'status':status,'frames':pilot.frames,'mulligans':mulligans,'explicit_checkpoint_recoveries':recoveries,'checkpoint_seen':pilot.checkpoint_seen,'screen':c.text()}
        results.append(result)
        print(f'Drill {index+1:02} {mode}: {status} after {pilot.frames} frames; mulligans={mulligans}',flush=True)
        if status=='FAIL':print(c.text(),flush=True)
        c.close()
    return results

if __name__=='__main__':
    import json,sys
    from pathlib import Path
    result=run_all(sys.argv[1] if len(sys.argv)>1 else 'build/rollback-e1-training.gbc')
    Path('build/playthrough.json').write_text(json.dumps(result,indent=2)+'\n')
    raise SystemExit(0 if all(r['status']=='PASS' for r in result) else 1)
