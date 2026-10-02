#include "game.h"
#include "assets.h"
BANKREF_EXTERN(background_tiles)
#include <string.h>

static const palette_color_t bg_palettes[32]={
 RGB(2,3,6),RGB(5,9,12),RGB(8,18,21),RGB(25,29,28),
 RGB(2,3,6),RGB(3,9,13),RGB(4,14,18),RGB(11,24,25),
 RGB(2,3,6),RGB(7,10,13),RGB(12,17,18),RGB(18,24,24),
 RGB(2,3,6),RGB(10,7,5),RGB(23,14,5),RGB(31,22,10),
 RGB(2,3,6),RGB(10,5,8),RGB(23,9,10),RGB(31,17,16),
 RGB(2,3,6),RGB(4,10,10),RGB(8,20,16),RGB(16,29,21),
 RGB(2,3,6),RGB(5,7,10),RGB(8,11,14),RGB(13,17,20),
 RGB(2,3,6),RGB(4,10,14),RGB(8,20,24),RGB(17,30,30)
};
static const palette_color_t sprite_palettes[32]={
 RGB(0,0,0),RGB(2,7,12),RGB(7,23,26),RGB(29,31,29),
 RGB(0,0,0),RGB(8,3,8),RGB(26,9,11),RGB(31,22,16),
 RGB(0,0,0),RGB(9,6,3),RGB(24,15,4),RGB(31,26,12),
 RGB(0,0,0),RGB(2,9,8),RGB(6,23,15),RGB(23,31,21),
 RGB(0,0,0),RGB(3,6,13),RGB(9,17,28),RGB(23,28,31),
 RGB(0,0,0),RGB(9,3,2),RGB(31,14,3),RGB(31,29,19),
 RGB(0,0,0),RGB(3,14,17),RGB(13,27,29),RGB(31,31,27),
 RGB(0,0,0),RGB(19,22,24),RGB(27,28,29),RGB(31,31,31)
};
uint8_t hud_hull=255,hud_boost=255,hud_progress=255,hud_goal=255,hud_core=255,hud_channel=255;
uint16_t hud_time=65535;
uint8_t hud_sector=255,hud_row=255,hud_direction=255;
char hud_radio[21];
volatile uint8_t arena_active,next_scroll_x,next_scroll_y;
int16_t loaded_column,loaded_row;

// A continuous bottom window keeps HUD rendering independent of tile-stream
// timing. It does not depend on scanline interrupts or window-counter tricks.
static void scroll_vblank(void) NONBANKED {
    SCX_REG=next_scroll_x; SCY_REG=next_scroll_y;
}
void tile(uint8_t x,uint8_t y,uint8_t n,uint8_t p) {
    if(arena_active) {
        if(y>=16) y-=14;
        VBK_REG=1; set_win_tile_xy(x,y,p|0x80); VBK_REG=0; set_win_tile_xy(x,y,n);
    } else {
        VBK_REG=1; set_bkg_tile_xy(x,y,p); VBK_REG=0; set_bkg_tile_xy(x,y,n);
    }
}
void text_at(uint8_t x,uint8_t y,const char *s,uint8_t palette) {
    uint8_t line[20],n=0;
    while(*s && x+n<20) line[n++]=(uint8_t)*s++;
    if(!n) return;
    if(arena_active) {
        if(y>=16) y-=14;
        VBK_REG=1; fill_win_rect(x,y,n,1,palette|0x80);
        VBK_REG=0; set_win_tiles(x,y,n,1,line);
    } else {
        VBK_REG=1; fill_bkg_rect(x,y,n,1,palette);
        VBK_REG=0; set_bkg_tiles(x,y,n,1,line);
    }
}
void number_at(uint8_t x,uint8_t y,uint16_t n,uint8_t digits,uint8_t palette) {
    char line[7]; uint8_t i=digits;
    line[i]=0;
    while(i) { line[--i]='0'+n%10; n/=10; }
    text_at(x,y,line,palette);
}
void wrapped(uint8_t x,uint8_t y,const char *s,uint8_t width,uint8_t rows,uint8_t palette) {
    char line[21]; uint8_t used,word,i;
    while(*s && rows--) {
        used=0; memset(line,' ',width); line[width]=0;
        while(*s) {
            while(*s==' ') s++;
            if(*s=='\n') {s++;break;}
            word=0; while(s[word] && s[word]!=' ' && s[word]!='\n') word++;
            if(used && used+word>width) break;
            if(word>width-used) word=width-used;
            for(i=0;i<word;i++) line[used++]=*s++;
            if(used>=width) break;
            if(*s==' ') { s++; used++; }
        }
        if(*s=='\n')s++;
        text_at(x,y++,line,palette);
    }
}
void frame(uint8_t x,uint8_t y,uint8_t w,uint8_t h,uint8_t p) {
    uint8_t i;
    for(i=0;i<w;i++) { tile(x+i,y,114,p); tile(x+i,y+h-1,114,p); }
    for(i=0;i<h;i++) { tile(x,y+i,115,p); tile(x+w-1,y+i,115,p); }
    tile(x,y,109,p); tile(x+w-1,y,109,p); tile(x,y+h-1,109,p); tile(x+w-1,y+h-1,109,p);
}
void video_init(void) {
    DISPLAY_OFF;
    uint8_t previous=CURRENT_BANK;
    SWITCH_ROM(BANK(background_tiles));
    VBK_REG=0; set_bkg_data(0,176,background_tiles); VBK_REG=1; set_sprite_data(0,SPRITE_TILE_COUNT,sprite_tiles); VBK_REG=0;
    SWITCH_ROM(previous);
    set_bkg_palette(0,8,bg_palettes); set_sprite_palette(0,8,sprite_palettes);
    BGP_REG=0xe4; OBP0_REG=0xe4; OBP1_REG=0xe4;
    SPRITES_8x16; HIDE_WIN; SHOW_BKG; SHOW_SPRITES;
    LCDC_REG|=LCDCF_WIN9C00; move_win(7,112);
    move_bkg(0,0);
    CRITICAL {
        add_VBL(scroll_vblank);
    }
    set_interrupts(VBL_IFLAG);
}
void page_begin(void) {
    uint8_t i;
    DISPLAY_OFF; HIDE_SPRITES; HIDE_WIN;
    set_bkg_palette(7,1,&bg_palettes[28]);
    arena_active=0; next_scroll_x=next_scroll_y=0;
    VBK_REG=1; fill_bkg_rect(0,0,32,32,0);
    VBK_REG=0; fill_bkg_rect(0,0,32,32,32);
    VBK_REG=1; fill_win_rect(0,0,20,4,0x80);
    VBK_REG=0; fill_win_rect(0,0,20,4,32);
    for(i=0;i<40;i++) hide_sprite(i);
    move_bkg(0,0);
}
void page_end(void) { VBK_REG=0; SHOW_BKG; DISPLAY_ON; }

typedef struct {uint8_t bank;const uint8_t *data;} PortraitRef;
extern const PortraitRef portraits[];
void portrait_at(uint8_t x,uint8_t y,uint8_t speaker,uint8_t mood) NONBANKED {
    uint8_t pixels[256],i,j;
    static const palette_color_t skin[4]={RGB(2,3,6),RGB(5,8,11),RGB(16,20,20),RGB(28,29,25)};
    static const palette_color_t mae_skin[4]={RGB(2,3,6),RGB(7,5,5),RGB(15,11,9),RGB(26,24,19)};
    if(speaker>=24)speaker=0;if(mood>=9)mood=0;
    bank_read(pixels,portraits[speaker].data+(uint16_t)mood*256u,256,portraits[speaker].bank);
    set_bkg_data(160,16,pixels);set_bkg_palette(7,1,speaker==2 || speaker==21?mae_skin:skin);
    for(j=0;j<4;j++)for(i=0;i<4;i++)tile(x+i,y+j,160+j*4+i,7);
}
