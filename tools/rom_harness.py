"""Hardware-boundary helpers for ROM playthroughs: buttons, LCD tiles and OAM.

No game-state writes, patched ROMs, injected wins, or debug-only game APIs.
"""
import io
from pathlib import Path
from pyboy import PyBoy
from PIL import Image

class Console:
    def __init__(self,rom):
        self.emu=PyBoy(rom,window='null',cgb=True,sound_emulated=True)
        self.emu.set_emulation_speed(0)
        self.held=set()
        self.last_player=(80,112)
        self.tick(180)

    def tick(self,frames=1):
        assert self.emu.tick(frames,True,True), 'Emulator unexpectedly stopped'

    def hold(self,buttons,frames=1):
        buttons=set(buttons)
        for key in self.held-buttons: self.emu.button_release(key)
        for key in buttons-self.held: self.emu.button_press(key)
        self.held=buttons
        self.tick(frames)

    def tap(self,key,after=10):
        self.hold([],3); self.hold([key],3); self.hold([],after)

    def line(self,y):
        # The on-cartridge font's public mapping uses ASCII tile identifiers.
        return ''.join(chr(self.emu.tilemap_background[x,y]&255) if 32 <= (self.emu.tilemap_background[x,y]&255)<96 else ' ' for x in range(20))

    def text(self): return '\n'.join(self.line(y) for y in range(18))

    def sprite(self,slot):
        # Game Boy OAM: Y+16, X+8, tile index, attributes. Two 8x16 strips form each vehicle.
        base=0xfe00+slot*4
        y,x,tile,attr=self.emu.memory[base:base+4]
        if not y or y>=160 or not x or x>=168: return None
        return x,y-8,tile,attr

    def player(self):
        p=self.sprite(0)
        if p:self.last_player=p[:2]
        return self.last_player

    def save(self):
        f=io.BytesIO();self.emu.save_state(f);return f.getvalue()

    def restore(self,state):
        self.hold([],1);self.emu.load_state(io.BytesIO(state));self.held=set();self.tick(2)

    def screenshot(self,name):
        dest=Path('build/screenshots');dest.mkdir(exist_ok=True)
        self.emu.screen.image.resize((640,576),Image.Resampling.NEAREST).save(dest/(name+'.png'))

    def resume(self,code):
        assert 'TRAINING SIMULATOR' in self.text()
        self.tap('down');self.tap('a')
        for i,(start,end) in enumerate(zip('1000',str(code))):
            change=(int(end)-int(start))%10
            button='up' if change<=5 else 'down'
            for _ in range(min(change,10-change)):self.tap(button,after=3)
            if i<3:self.tap('right',after=3)
        self.tap('a')

    def launch(self):
        assert 'FLIGHT SIMULATOR' in self.text(), self.text()
        for _ in range(3):self.tap('a')
        self.hold([],15)
        assert self.line(0).startswith('HP'),self.text()

    def close(self): self.emu.stop(save=False)
