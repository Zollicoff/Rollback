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
        self.last_camera_y=0
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
        # ASCII tile identifiers plus the authored up-arrow tile. During play,
        # logical rows 0/1/16/17 are the four rows of the bottom HUD window.
        if not self.emu.memory[0xff40]&0x80:return ' '*20
        if self.emu.memory[0xff40]&0x20:
            if 2<=y<16:return ' '*20
            window=True;row=y-14 if y>=16 else y
        else:window=False;row=y
        def glyph(tile):
            tile&=255
            return '^' if tile==105 else (chr(tile) if 32<=tile<96 else ' ')
        return ''.join(glyph(self.tile(x,row,window)) for x in range(20))

    def tile(self,x,y,window=False):
        lcdc=self.emu.memory[0xff40]
        base=0x9c00 if lcdc&(0x40 if window else 0x08) else 0x9800
        # Bank zero holds tile IDs. The CPU may be paused while VBK selects
        # bank one to stream palettes; reading that bank would invent walls/text.
        return self.emu.memory[0,base+(y&31)*32+(x&31)]

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

    def camera_y(self):
        if not self.line(0).startswith('HP'):return self.last_camera_y
        raw=self.emu.screen.tilemap_position_list[64][1]
        row=self.line(16)[18:20]
        player=self.sprite_screen(0)
        py=player[1] if player else self.last_player[1]
        if row.isdigit():
            lo=(int(row)-1)*144
            candidates=[raw+256*n for n in range(6) if 0<=raw+256*n<=1328 and lo<=raw+256*n+py<lo+144]
            if candidates:self.last_camera_y=candidates[0];return self.last_camera_y
        self.last_camera_y+=(raw-self.last_camera_y+128)%256-128
        return self.last_camera_y

    def sprite(self,slot):
        value=self.sprite_screen(slot)
        return (value[0]+self.camera(),value[1]+self.camera_y(),*value[2:]) if value else None

    def player(self):
        p=self.sprite_screen(0)
        if p:self.last_player=p[:2]
        return self.last_player[0]+self.camera(),self.last_player[1]+self.camera_y()

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
