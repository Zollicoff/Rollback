"""Verify the actual cartridge binary through hardware inputs and outputs.

Contracts: bootable GBC/MBC5 header; audio on/off; controls, cover and pause;
password rejection/restoration across a fresh boot; checkpoint recovery;
objective completion for every bundled scenario. No game RAM is modified.
"""
import hashlib
import json
import sys
from pathlib import Path
import numpy as np
from rom_harness import Console
from playthrough import Pilot,run_all

rom=sys.argv[1] if len(sys.argv)>1 else 'build/rollback-e1-training.gbc'
data=Path(rom).read_bytes()
assert len(data)==65536,'ROM must match its 64 KiB cartridge declaration'
assert data[0x143]==0xc0,'This release is CGB-only'
assert data[0x147:0x14a]==bytes([0x19,1,0]),'MBC5, 64 KiB ROM, no external RAM'
header=0
for byte in data[0x134:0x14d]:header=(header-byte-1)&255
assert header==data[0x14d],'Invalid Game Boy header checksum'
assert (sum(data)-data[0x14e]-data[0x14f])&65535==int.from_bytes(data[0x14e:0x150],'big'),'Invalid ROM checksum'
checks=['cartridge header and both checksums']

def audio_peak(c,frames=120):
    peak=0
    for _ in range(frames):
        c.tick();peak=max(peak,int(np.abs(c.emu.sound.ndarray.astype(np.int16)).max()))
    return peak

c=Console(rom)
assert 'TRAINING SIMULATOR' in c.text()
c.screenshot('title')
assert audio_peak(c)>0,'No synthesized audio reached the emulator output'
c.tap('select');assert audio_peak(c)==0,'Mute must silence output'
c.tap('select');assert audio_peak(c)>0,'Unmute must restore audio'
checks.append('boot, title and audible/muted/unmuted audio output')
c.tap('down');c.tap('a');c.tap('a')
assert 'INVALID CODE' in c.text(),'Invalid resume code was accepted'
c.tap('b');c.tap('up');c.tap('a');c.launch();c.hold([],65)
initial=c.player()
c.hold(['left'],20);c.hold([],5)
left=c.player();assert left[0]<initial[0]-10,'Left input did not move the player'
c.hold(['right'],20);c.hold([],5)
assert c.player()[0]>left[0]+10,'Right input did not move the player'
c.hold(['up'],2);c.hold(['a'],5)
assert any(c.sprite(22+i) for i in range(8)),'Firing did not produce a visible projectile'
c.hold([],5)
before=c.line(0)[16:19]
c.tap('start');assert 'TIMELINE PAUSED' in c.text()
c.hold([],240);c.tap('start')
after=c.line(0)[16:19]
assert abs(int(before)-int(after))<=1,'Paused time advanced the mission timer'
checks.append('movement, visible weapon fire and frozen pause timer')
c.screenshot('gameplay')
c.close()

# A newly booted console restores a password, without relying on emulator RAM.
c=Console(rom);c.resume(1690)
assert '05' in c.line(3) and 'HIGH WATER' in c.text(),'Fresh boot did not restore drill 5'
c.launch();c.hold([],65)
pilot=Pilot(c,'SURVIVAL')
for _ in range(400):
    if not pilot.step():break
    if pilot.checkpoint_seen:break
assert pilot.checkpoint_seen,'Survival did not produce its midpoint checkpoint'
checkpoint_seconds=pilot.checkpoint_time
c.screenshot('checkpoint')
# Observe failure with no controls, then invoke the ROM's own rewind command.
c.hold([],1)
for _ in range(900):
    c.tick()
    if 'TIMELINE BROKEN' in c.text():break
assert 'TIMELINE BROKEN' in c.text(),'Expected hostile fire to exhaust hull after midpoint'
c.screenshot('failure')
c.tap('a',after=20)
assert c.line(0).startswith('HP'),'Mulligan did not return to gameplay'
assert int(c.line(0)[16:19])>=checkpoint_seconds-1,'Rewind failed to restore the earlier mission clock'
assert int(c.line(0)[16:19])<30,'Rewind restarted the mission instead of its midpoint checkpoint'
assert 'TIMELINE RESTORED' in c.text()
c.screenshot('rewind')
checks.append('fresh-boot resume, midpoint checkpoint, death and game-owned rewind')
c.close()

results=run_all(rom)
assert all(r['status']=='PASS' for r in results),'At least one scenario did not complete through player inputs'
checks.append('all 12 training scenarios across all 9 objective types')
report={'rom':str(rom),'sha256':hashlib.sha256(data).hexdigest(),'bytes':len(data),
        'emulator':'PyBoy 2.7.0; CGB mode; button input only; no game RAM writes',
        'checks':checks,'scenarios':results,'hardware_tested':False,'canonical_episode_1_story_integrated':False}
Path('build/verification.json').write_text(json.dumps(report,indent=2)+'\n')
print(json.dumps({k:v for k,v in report.items() if k!='scenarios'},indent=2))
