"""Original pixel art, font and 2bpp tiles. No external art/font assets."""
from pathlib import Path
from PIL import Image, ImageDraw

FONT = {
'A':'01110/10001/10001/11111/10001/10001/10001','B':'11110/10001/10001/11110/10001/10001/11110',
'C':'01111/10000/10000/10000/10000/10000/01111','D':'11110/10001/10001/10001/10001/10001/11110',
'E':'11111/10000/10000/11110/10000/10000/11111','F':'11111/10000/10000/11110/10000/10000/10000',
'G':'01111/10000/10000/10111/10001/10001/01111','H':'10001/10001/10001/11111/10001/10001/10001',
'I':'11111/00100/00100/00100/00100/00100/11111','J':'00111/00010/00010/00010/10010/10010/01100',
'K':'10001/10010/10100/11000/10100/10010/10001','L':'10000/10000/10000/10000/10000/10000/11111',
'M':'10001/11011/10101/10101/10001/10001/10001','N':'10001/11001/10101/10011/10001/10001/10001',
'O':'01110/10001/10001/10001/10001/10001/01110','P':'11110/10001/10001/11110/10000/10000/10000',
'Q':'01110/10001/10001/10001/10101/10010/01101','R':'11110/10001/10001/11110/10100/10010/10001',
'S':'01111/10000/10000/01110/00001/00001/11110','T':'11111/00100/00100/00100/00100/00100/00100',
'U':'10001/10001/10001/10001/10001/10001/01110','V':'10001/10001/10001/10001/10001/01010/00100',
'W':'10001/10001/10001/10101/10101/11011/10001','X':'10001/10001/01010/00100/01010/10001/10001',
'Y':'10001/10001/01010/00100/00100/00100/00100','Z':'11111/00001/00010/00100/01000/10000/11111',
'0':'01110/10001/10011/10101/11001/10001/01110','1':'00100/01100/00100/00100/00100/00100/01110',
'2':'01110/10001/00001/00010/00100/01000/11111','3':'11110/00001/00001/01110/00001/00001/11110',
'4':'00010/00110/01010/10010/11111/00010/00010','5':'11111/10000/10000/11110/00001/00001/11110',
'6':'01110/10000/10000/11110/10001/10001/01110','7':'11111/00001/00010/00100/01000/01000/01000',
'8':'01110/10001/10001/01110/10001/10001/01110','9':'01110/10001/10001/01111/00001/00001/01110',
'.':'00000/00000/00000/00000/00000/00110/00110',',':'00000/00000/00000/00000/00110/00110/00100',
':':'00000/00110/00110/00000/00110/00110/00000',';':'00000/00110/00110/00000/00110/00110/00100',
'!':'00100/00100/00100/00100/00100/00000/00100','?':'01110/10001/00001/00010/00100/00000/00100',
'-':'00000/00000/00000/11111/00000/00000/00000','/':'00001/00001/00010/00100/01000/10000/10000',
"'":'00100/00100/00000/00000/00000/00000/00000','(':'00010/00100/01000/01000/01000/00100/00010',
')':'01000/00100/00010/00010/00010/00100/01000','>':'10000/01000/00100/00010/00100/01000/10000',
'<':'00001/00010/00100/01000/00100/00010/00001','+':'00000/00100/00100/11111/00100/00100/00000',
'=':'00000/00000/11111/00000/11111/00000/00000','*':'00000/10101/01110/11111/01110/10101/00000'}

def blank(w=8,h=8): return Image.new('L',(w,h),0)
def glyph(c):
    im=blank()
    for y,row in enumerate(FONT.get(c,'0/0/0/0/0/0/0').split('/')):
        for x,p in enumerate(row):
            if p=='1': im.putpixel((x+1,y),3)
    return im
def encode(im):
    result=[]
    for y in range(8):
        lo=hi=0
        for x in range(8):
            p=im.getpixel((x,y))
            lo|=(p&1)<<(7-x); hi|=((p>>1)&1)<<(7-x)
        result += [lo,hi]
    return result

bg=[blank() for _ in range(176)]
for c in FONT: bg[ord(c)]=glyph(c)
for i in range(96,128):
    im=bg[i]; d=ImageDraw.Draw(im)
    if i in (96,97):
        d.line((0,2,3,2),fill=1); d.line((4,6,7,6),fill=2 if i==97 else 1)
        d.point((6,1),fill=1)
    elif i==98:
        d.point((1,5),fill=1); d.point((6,2),fill=1)
    elif i==99: d.rectangle((3,1,4,5),fill=2)
    elif i==100:
        d.rectangle((0,0,7,7),fill=1); d.line((0,0,7,0),fill=3); d.line((7,1,7,7),fill=2)
    elif i==101:
        d.rectangle((0,0,7,7),fill=1); d.rectangle((1,2,2,4),fill=3); d.rectangle((5,2,6,4),fill=2)
    elif i==102:
        d.rectangle((0,0,7,7),fill=1); d.rectangle((1,1,6,6),outline=2); d.point((2,2),fill=3)
    elif i==103:
        for x in range(8):
            for y in range(8): im.putpixel((x,y),2 if (x+y)%8<3 else 0)
    elif i==104:
        d.line((0,0,7,0),fill=3); d.line((0,0,0,7),fill=3); d.line((2,7,7,2),fill=1)
    elif i==105: d.polygon([(4,1),(7,5),(5,5),(5,7),(3,7),(3,5),(1,5)],fill=3)
    elif i==106:
        d.line((0,3,7,3),fill=2); d.line((0,4,7,4),fill=1)
    elif i==107: d.rectangle((0,0,7,7),fill=2)
    elif i==108: d.rectangle((1,2,6,5),fill=3)
    elif i==109: d.rectangle((2,2,5,5),fill=3)
    elif i==110:
        d.line((7,0,7,7),fill=1); d.line((0,7,7,7),fill=1)
    elif i==111: d.rectangle((0,0,7,7),outline=3)
    elif i==112:
        d.rectangle((0,0,7,7),fill=1); d.line((0,7,7,7),fill=2)
    elif i==113: d.line((3,7,3,3,7,3),fill=2)
    elif i==114: d.line((0,3,7,3),fill=2)
    elif i==115: d.line((3,0,3,7),fill=2)
    elif i==116: d.line((0,3,7,3),fill=3)
for j,c in enumerate('ROLLBACK'):
    big=glyph(c).resize((16,16),Image.Resampling.NEAREST)
    # Deliberate signal break across the middle of the logotype.
    for x in range(16): big.putpixel((x,7),0)
    for y in range(2):
        for x in range(2): bg[128+j*4+y*2+x]=big.crop((x*8,y*8,x*8+8,y*8+8))
portrait=blank(32,32); d=ImageDraw.Draw(portrait)
d.rectangle((0,0,31,31),fill=1); d.line((0,0,31,0),fill=3)
d.polygon([(4,31),(6,25),(12,22),(11,17),(8,15),(8,7),(13,3),(21,3),(25,8),(24,19),(20,23),(27,26),(29,31)],fill=2)
d.rectangle((10,8,24,14),fill=0); d.line((11,10,22,10),fill=3)
d.rectangle((7,13,9,18),fill=3); d.line((9,18,13,20),fill=3)
d.rectangle((13,24,19,31),fill=0); d.line((5,29,10,26),fill=3)
for y in range(4):
    for x in range(4): bg[160+y*4+x]=portrait.crop((x*8,y*8,x*8+8,y*8+8))

sprites=[]
def sprite(im):
    # 8x16 hardware sprites: left column, then right column.
    for x in range(im.width//8):
        for y in range(2): sprites.append(im.crop((x*8,y*8,x*8+8,y*8+8)))
ship=blank(16,16); d=ImageDraw.Draw(ship)
d.polygon([(7,0),(9,4),(10,8),(14,11),(14,14),(9,12),(8,15),(6,15),(5,12),(1,14),(1,11),(5,8),(6,3)],fill=1)
d.polygon([(7,1),(8,5),(9,10),(12,12),(9,11),(7,13),(5,11),(3,12),(6,9),(6,5)],fill=2)
d.line((7,4,7,9),fill=3); d.point((7,14),fill=3)
for angle in range(0,360,45): sprite(ship.rotate(-angle,resample=Image.Resampling.NEAREST))
for kind in ['drone','gunner','core','relay','pod','transport','boss']:
    im=blank(16,16); d=ImageDraw.Draw(im)
    if kind in ('drone','gunner'):
        d.polygon([(2,4),(6,1),(9,1),(13,4),(13,12),(9,14),(6,14),(2,12)],fill=1)
        d.rectangle((4,3,11,12),fill=2); d.rectangle((6,6,9,9),fill=3)
        d.rectangle((0,5,2,11),fill=2); d.rectangle((13,5,15,11),fill=2)
        if kind=='gunner': d.line((7,0,7,15),fill=3)
    elif kind in ('core','relay'):
        d.rectangle((2,3,13,14),fill=1); d.rectangle((4,5,11,12),fill=2)
        d.rectangle((6,6,9,10),fill=3); d.line((7,0,7,4),fill=3)
        d.line((1,14,14,14),fill=2)
        if kind=='relay': d.line((2,1,12,1),fill=3)
    elif kind=='pod':
        d.rounded_rectangle((3,1,12,14),radius=3,fill=1)
        d.rectangle((5,3,10,12),fill=2); d.rectangle((6,5,9,8),fill=3)
    elif kind=='transport':
        d.rectangle((1,4,14,12),fill=1); d.rectangle((3,5,12,11),fill=2)
        d.rectangle((10,6,13,9),fill=3); d.rectangle((2,2,5,4),fill=2); d.rectangle((2,12,5,14),fill=2)
    else:
        d.polygon([(0,3),(4,0),(11,0),(15,3),(15,13),(11,15),(4,15),(0,13)],fill=1)
        d.rectangle((3,2,12,13),fill=2); d.rectangle((5,5,10,10),fill=1)
        d.line((6,6,9,9),fill=3); d.line((9,6,6,9),fill=3)
        for x in (0,13): d.rectangle((x,5,x+2,10),fill=3)
    sprite(im)
for hostile in (False,True):
    im=blank(8,16);d=ImageDraw.Draw(im)
    d.rectangle((2,5,5,8),fill=2);d.rectangle((3,6,4,7),fill=3)
    if not hostile:d.line((3,3,3,5),fill=3)
    sprite(im)
im=blank(16,16);d=ImageDraw.Draw(im)
d.rectangle((3,3,12,12),fill=1);d.rectangle((4,4,11,11),fill=2)
d.rectangle((7,5,8,10),fill=3);d.rectangle((5,7,10,8),fill=3);sprite(im)
im=blank(8,16);d=ImageDraw.Draw(im)
d.line((0,2,7,9),fill=2);d.line((7,2,0,9),fill=2);d.rectangle((2,4,5,7),fill=3);sprite(im)
im=blank(16,16);d=ImageDraw.Draw(im)
d.rectangle((1,1,14,14),outline=2);d.line((4,8,7,11,12,4),fill=3);sprite(im)
assert len(sprites)==74
def c_array(name,tiles):
    raw=sum((encode(t) for t in tiles),[])
    return 'const unsigned char '+name+'[] = {\n'+ '\n'.join('    '+','.join(f'0x{v:02x}' for v in raw[i:i+16])+',' for i in range(0,len(raw),16))+'\n};\n'
Path('build/assets.c').write_text(c_array('background_tiles',bg)+c_array('sprite_tiles',sprites))
Path('build/assets.h').write_text('#ifndef ASSETS_H\n#define ASSETS_H\nextern const unsigned char background_tiles[];\nextern const unsigned char sprite_tiles[];\n#endif\n')
sheet=Image.new('RGB',(128,128),(11,18,31)); palette=[(11,18,31),(29,55,69),(54,149,167),(202,236,225)]
for i,t in enumerate(bg[:128]):
    out=Image.new('RGB',(8,8));out.putdata([palette[p] for p in t.getdata()]);sheet.paste(out,((i%16)*8,(i//16)*8))
Path('assets').mkdir(exist_ok=True)
sheet.save('build/tile-sheet.png')
print('Generated original font, scenery, portrait and 74 sprite tiles')
