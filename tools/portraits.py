"""Authored native 32px talking-head tiles, eight expressions and a newsreel filter."""
from pathlib import Path
from PIL import Image,ImageDraw
from campaign import SPEAKERS, MOODS
OUT=Path('build/portraits');OUT.mkdir(parents=True,exist_ok=True)

def encode(im):
    raw=[]
    for ty in range(4):
        for tx in range(4):
            for y in range(8):
                lo=hi=0
                for x in range(8):
                    p=im.getpixel((tx*8+x,ty*8+y));lo|=(p&1)<<(7-x);hi|=(p>>1)<<(7-x)
                raw.extend([lo,hi])
    return raw

def face(who,mood):
    im=Image.new('L',(32,32),0);d=ImageDraw.Draw(im)
    d.rectangle((0,0,31,31),outline=1);d.line((2,2,10,2),fill=2);d.line((21,29,29,29),fill=2)
    if who==0:
        d.line((2,27,29,27),fill=3);d.rectangle((4,28,6,31),fill=1);d.rectangle((25,28,27,31),fill=1)
        d.pieslice((6,5,26,28),180,360,fill=2);d.rectangle((6,15,26,23),fill=2)
        d.rectangle((8,14,24,19),fill=0);d.line((10,15,22,15),fill=3)
        d.rectangle((7,22,25,25),fill=1);d.line((6,25,26,25),fill=3)
        return im
    if who in [4,7,8,17,18]:
        d.polygon([(7,5),(24,5),(27,9),(27,23),(23,27),(8,27),(4,23),(4,9)],fill=1)
        d.rectangle((7,8,24,23),fill=2);d.rectangle((9,10,22,21),fill=0)
        if who==8:
            d.line((12,12,12,16),fill=3);d.line((19,12,19,16),fill=3)
            d.rectangle((13,19,19,20),fill=2);d.line((16,2,16,7),fill=3)
        elif who==7:
            d.polygon([(16,9),(23,19),(8,19)],outline=3);d.line((16,12,16,16),fill=3)
        elif who==18:
            d.rectangle((10,10,21,18),fill=2);d.line((11,12,14,12),fill=0);d.line((18,12,21,12),fill=0);d.rectangle((14,20,18,24),fill=3)
        else:
            d.rectangle((11,12,13,14),fill=3);d.rectangle((19,12,21,14),fill=3)
            if mood in [4,6,7]:d.line((12,20,15,18,20,20),fill=3)
            else:d.line((12,17,14,20,19,20,21,17),fill=3)
        return im
    # Individual head silhouettes, uniforms, hair and equipment.
    d.polygon([(2,31),(6,27),(12,24),(11,20),(8,17),(8,9),(12,5),(21,5),(24,10),(23,20),(19,24),(27,27),(30,31)],fill=2)
    d.polygon([(2,31),(6,27),(12,25),(16,30),(20,25),(27,27),(30,31)],fill=1)
    d.line((7,29,11,27),fill=3);d.line((21,27,25,29),fill=3)
    if who in [1,20]:
        d.polygon([(8,9),(10,4),(19,3),(24,6),(23,11),(18,8),(12,10)],fill=1)
        d.rectangle((8,9,24,13),fill=0);d.rectangle((10,10,15,12),fill=3);d.rectangle((18,10,22,12),fill=3)
    elif who in [2,21]:
        d.polygon([(7,17),(7,8),(11,4),(21,4),(25,9),(24,19),(22,10),(10,10),(10,18)],fill=1)
        d.line((10,7,22,7),fill=3);d.line((8,14,8,21,13,22),fill=3)
    elif who==3:
        d.polygon([(8,10),(7,5),(12,6),(13,2),(18,5),(23,3),(25,9),(21,10)],fill=3)
        d.rectangle((9,11,15,15),outline=0);d.rectangle((18,11,24,15),outline=0);d.line((15,12,18,12),fill=0)
    elif who==5:
        d.polygon([(6,17),(6,7),(11,3),(22,4),(25,9),(25,20),(22,17),(21,9),(11,9),(10,19)],fill=1)
        d.rectangle((9,5,23,9),fill=3);d.rectangle((11,6,15,8),fill=0);d.rectangle((18,6,22,8),fill=0)
        d.line((7,24,3,28),fill=3)
    elif who in [6,9,19,22]:
        d.polygon([(5,7),(9,3),(24,3),(26,7),(22,10),(8,10)],fill=1)
        d.line((7,8,25,8),fill=3);d.rectangle((15,4,18,6),fill=3)
        if who==22:d.line((22,10,22,18),fill=0);d.line((20,11,24,15),fill=0)
    elif who in [10,23]:
        d.rectangle((6,5,24,9),fill=3 if who==23 else 1)
        d.line((4,10,24,10),fill=2);d.rectangle((11,18,21,20),fill=1)
        if who==23:d.rectangle((11,26,23,31),fill=3)
    elif who==13:
        d.polygon([(6,9),(8,3),(22,4),(26,12),(22,11),(20,8),(8,12)],fill=1);d.line((10,17,20,15),fill=1)
    elif who==14:
        d.polygon([(8,10),(8,5),(13,2),(20,3),(24,7),(23,11)],fill=3);d.rectangle((8,12,14,15),outline=1);d.rectangle((18,12,24,15),outline=1)
    else:
        d.polygon([(8,10),(9,5),(21,4),(25,9),(20,8),(13,8)],fill=1)
    # Eyebrows, eyes and mouths carry the same mood across every human actor.
    if who not in [1,20]:
        d.line((11,12,14,12),fill=0);d.line((19,12,22,12),fill=0)
        if mood in [3,7]:d.line((10,10,14,11),fill=0);d.line((19,11,23,10),fill=0)
        if mood==4:d.rectangle((11,12,13,14),fill=3);d.rectangle((19,12,21,14),fill=3)
    if mood==1:d.line((14,19,20,18),fill=0)
    elif mood==5:d.line((13,18,15,20,18,20,21,18),fill=0)
    elif mood==4:d.ellipse((15,18,19,22),fill=0)
    elif mood in [6,7]:d.line((13,20,16,18,20,20),fill=0)
    else:d.line((14,19,20,19),fill=0)
    if mood==8:
        for y in range(1,32,4):d.line((1,y,30,y),fill=1)
    return im

index=['#include "game.h"','typedef struct {uint8_t bank;const uint8_t *data;} PortraitRef;'];refs=[]
sheet=Image.new('RGB',(32*len(MOODS),32*len(SPEAKERS)))
palette=[(10,17,29),(39,58,74),(127,154,159),(230,233,203)]
for who,name in enumerate(SPEAKERS):
    data=[]
    for mood in range(len(MOODS)):
        im=face(who,mood);data+=encode(im)
        preview=Image.new('RGB',im.size);preview.putdata([palette[p] for p in im.getdata()]);sheet.paste(preview,(mood*32,who*32))
    name=f'portrait_{who}';lines=['#pragma bank 255','#include <gb/gb.h>',f'BANKREF({name})',f'const unsigned char {name}[] = {{']
    lines += [','.join(map(str,data[i:i+32]))+',' for i in range(0,len(data),32)];lines.append('};')
    (OUT/(name+'.c')).write_text('\n'.join(lines)+'\n');index += [f'BANKREF_EXTERN({name})',f'extern const uint8_t {name}[];'];refs.append('{BANK(%s),%s}'%(name,name))
index.append('const PortraitRef portraits[] = {'+','.join(refs)+'};');(OUT/'index.c').write_text('\n'.join(index)+'\n')
sheet.resize((576,1536),Image.Resampling.NEAREST).save('build/portrait-sheet.png')
print(f'Generated {len(SPEAKERS)} portraits with {len(MOODS)} expressions/filters each')
