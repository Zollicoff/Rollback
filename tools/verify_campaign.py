"""Cartridge-boundary verification, including the independently supplied screenplay."""
import hashlib,json,sys
from pathlib import Path
import numpy as np
from rom_harness import Console
from campaign_playthrough import CampaignPilot,run_mission,resume_code
from verify_scrolling import verify_scrolling
from verify_waypoints import verify_waypoints
import story_contract

rom=sys.argv[1] if len(sys.argv)>1 else 'build/rollback.gbc'
data=Path(rom).read_bytes()
assert data[0x143]==0xc0 and data[0x147]==0x19 and data[0x149]==0
assert len(data)==32768<<data[0x148]
header=0
for b in data[0x134:0x14d]:header=(header-b-1)&255
assert header==data[0x14d]
assert (sum(data)-data[0x14e]-data[0x14f])&65535==int.from_bytes(data[0x14e:0x150],'big')
checks=['CGB-only MBC5 header, declared ROM size, header and whole-ROM checksums']
levels=json.loads(Path('data/campaign.json').read_text())['missions']

def audio_peak(c):
    peak=0
    for _ in range(120):
        c.tick();peak=max(peak,int(np.abs(c.emu.sound.ndarray.astype(np.int16)).max()))
    return peak

c=Console(rom)
assert 'CAMPAIGN' in c.text();c.screenshot('campaign-title')
assert audio_peak(c)>0
c.tap('select');assert audio_peak(c)==0
c.tap('select');assert audio_peak(c)>0
c.tap('down');c.tap('a');c.tap('a');assert 'INVALID CODE' in c.text()
c.close()
for count,difficulty,complete,loop in [(1,0,False,False),(41,3,False,False),(54,1,True,False),(53,1,True,True)]:
    c=Console(rom);c.resume(resume_code(count,difficulty,complete,loop));assert f'MISSION {count:02}' in c.text(),c.text()
    assert ['CADET','PILOT','ACE','AUDITOR'][difficulty] in c.text(),c.text()
    assert ('LOOP 2' in c.text())==loop;c.close()
checks.append('audible audio and mute; invalid codes rejected; fresh-boot mission, difficulty and loop codes')

# A distant checkpoint: actual play produces the checkpoint, then enemy fire
# causes a defeat. Position/time/camera/progress must rewind together.
c=Console(rom);c.resume(resume_code(8));c.launch();pilot=CampaignPilot(c,levels[7])
for _ in range(5000):
    assert pilot.step(),c.text()
    if pilot.checkpoint_seen:break
else:raise AssertionError('No raid checkpoint reached')
pilot.drain_radio();c.hold([],2)
position=c.player();camera=(c.camera(),c.camera_y());hud=c.line(1);timer=int(c.line(0)[16:19])
assert camera[0]>160 and camera[1]>144
c.screenshot('checkpoint');c.tap('start');assert 'TIMELINE PAUSED' in c.text()
c.hold([],180);c.tap('b');pilot.drain_radio();assert abs(int(c.line(0)[16:19])-timer)<=1
for _ in range(500):
    c.hold([],12);pilot.drain_radio()
    if 'COMPROMISED' in c.text():break
else:raise AssertionError('Enemy fire did not cause a defeat')
c.screenshot('defeat');c.hold([],30);c.tap('start',after=20);pilot.drain_radio();c.hold([],4)
assert c.line(0).startswith('HP'),c.text()
assert abs(c.player()[0]-position[0])<16 and abs(c.player()[1]-position[1])<16
assert abs(c.camera()-camera[0])<16 and abs(c.camera_y()-camera[1])<16
assert abs(int(c.line(0)[16:19])-timer)<=1
assert c.line(1)[14:]==hud[14:]
c.screenshot('rewind');c.close();checks.append('pause freezes flight; distant defeat/checkpoint rewind restores world, camera, clock and objective progress')
# The final return is the original Kestrel arena, not the Day Zero biome.
opening=[]
for n in (1,54):
    c=Console(rom);c.resume(resume_code(n));c.launch();c.hold([],40)
    opening.append([[c.tile(x,y) for x in range(32)] for y in range(32)])
    c.close()
assert opening[0]==opening[1],'Launch Night (Again) did not restore the opening arena'
checks.append('final Launch Night returns to the original Kestrel arena')
scrolling=verify_scrolling(rom);checks+=scrolling['checks'];checks+=verify_waypoints(rom)
results=[run_mission(rom,level) for level in levels]
assert all(r['status']=='PASS' for r in results)
checks.append('54 main missions completed with controller input, including authored briefings/debriefings and protected result screens')
secret=run_mission(rom,levels[52],loop=True)
assert secret['status']=='PASS'
# Read the epilogue reached through the second-loop campaign menu, then follow
# the actual ending and credits controls to the clean helmet card.
c=Console(rom);c.resume(resume_code(54,1,True,True));c.tap('a');pieces=[]
for _ in range(100):
    story_contract.wait_visible(c)
    if 'BRIEFING' not in c.line(1):break
    pieces.append(story_contract.body(c));c.tap('a',after=3)
assert story_contract.expected_dialogue('54-B','brief') in ' '.join(pieces)
assert 'THE LOOP IS OPEN' in c.text();c.screenshot('secret-ending')
for _ in range(30):
    if 'TALLY: 0' in c.text():break
    c.tap('start')
assert 'TALLY: 0' in c.text();c.screenshot('clean-helmet');c.close()
checks.append('second-loop talk-down fight, complete authored noncombat epilogue, credits and clean-helmet ending')
report=dict(rom=rom,sha256=hashlib.sha256(data).hexdigest(),bytes=len(data),
 emulator='PyBoy 2.7.0; CGB mode; controller input and hardware observations; no game RAM writes',
 checks=checks,scrolling=scrolling,missions=results,secret=secret,hardware_tested=False,
 story_source_hashes=json.loads(Path('story/source-sha256.json').read_text()))
Path('build/verification.json').write_text(json.dumps(report,indent=2)+'\n')
print(json.dumps({k:v for k,v in report.items() if k not in ['missions','story_source_hashes']},indent=2))
