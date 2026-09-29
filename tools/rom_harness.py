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
        self.last_camera=0
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
        for _ in range(120):
            if self.emu.memory[0xff40]&0x80:break
            self.tick()
        else:raise AssertionError('Display did not return after a menu/arena transition')
        self.tick(2)

    def line(self,y):
        # The on-cartridge font's public mapping uses ASCII tile identifiers.
        if not self.emu.memory[0xff40]&0x80:return ' '*20
        return ''.join(chr(self.emu.tilemap_background[x,y]&255) if 32 <= (self.emu.tilemap_background[x,y]&255)<96 else ' ' for x in range(20))

    def text(self): return '\n'.join(self.line(y) for y in range(18))

    def sprite_screen(self,slot):
        # Game Boy OAM: Y+16, X+8, tile index, attributes. Two 8x16 strips form each vehicle.
        base=0xfe00+slot*4
        y,x,tile,attr=self.emu.memory[base:base+4]
        if not y or y>=160 or not x or x>=168: return None
        return x,y-8,tile,attr

    def camera(self):
        # Hardware SCX wraps every 256 pixels. The displayed sector number
        # disambiguates that wrap without reading the game's world state.
        if not self.line(0).startswith('HP'):return self.last_camera
        raw=self.emu.screen.tilemap_position_list[64][0]
        sector=self.line(16)[15:17]
        player=self.sprite_screen(0)
        px=player[0] if player else self.last_player[0]
        if sector.isdigit():
            lo=(int(sector)-1)*160
            candidates=[raw+256*n for n in range(6) if 0<=raw+256*n<=1440 and lo<=raw+256*n+px<lo+160]
            if candidates:self.last_camera=candidates[0];return self.last_camera
        delta=(raw-self.last_camera+128)%256-128
        self.last_camera+=delta
        return self.last_camera

    def sprite(self,slot):
        value=self.sprite_screen(slot)
        return (value[0]+self.camera(),*value[1:]) if value else None

    def player(self):
        p=self.sprite_screen(0)
        if p:self.last_player=p[:2]
        return self.last_player[0]+self.camera(),self.last_player[1]

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
