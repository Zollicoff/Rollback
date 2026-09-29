#include "game.h"
#include "assets.h"
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
static uint8_t hud_hull=255,hud_boost=255,hud_progress=255,hud_goal=255,hud_core=255,hud_channel=255;
static uint16_t hud_time=65535;
static uint8_t hud_sector=255,hud_row=255,hud_direction=255;
static const char *hud_radio;
static volatile uint8_t arena_active,next_scroll_x,next_scroll_y;
static int16_t loaded_column,loaded_row;

// A continuous bottom window keeps HUD rendering independent of tile-stream
// timing. It does not depend on scanline interrupts or window-counter tricks.
static void scroll_vblank(void) NONBANKED {
    SCX_REG=next_scroll_x; SCY_REG=next_scroll_y;
}
static void tile(uint8_t x,uint8_t y,uint8_t n,uint8_t p) {
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
    char line[6]; uint8_t i=digits;
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
            word=0; while(s[word] && s[word]!=' ') word++;
            if(used && used+word>width) break;
            if(word>width-used) word=width-used;
            for(i=0;i<word;i++) line[used++]=*s++;
            if(used>=width) break;
            if(*s==' ') { s++; used++; }
        }
        text_at(x,y++,line,palette);
    }
}
void frame(uint8_t x,uint8_t y,uint8_t w,uint8_t h,uint8_t p) {
    uint8_t i;
    for(i=0;i<w;i++) { tile(x+i,y,114,p); tile(x+i,y+h-1,114,p); }
    for(i=0;i<h;i++) { tile(x,y+i,115,p); tile(x+w-1,y+i,115,p); }
    tile(x,y,109,p); tile(x+w-1,y,109,p); tile(x,y+h-1,109,p); tile(x+w-1,y+h-1,109,p);
}
static void portrait(uint8_t x,uint8_t y) {
    uint8_t i,j;
    for(j=0;j<4;j++) for(i=0;i<4;i++) tile(x+i,y+j,160+j*4+i,7);
}
void video_init(void) {
    DISPLAY_OFF;
    VBK_REG=0; set_bkg_data(0,176,background_tiles); set_sprite_data(0,74,sprite_tiles);
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
    arena_active=0; next_scroll_x=next_scroll_y=0;
    VBK_REG=1; fill_bkg_rect(0,0,32,32,0);
    VBK_REG=0; fill_bkg_rect(0,0,32,32,32);
    VBK_REG=1; fill_win_rect(0,0,20,4,0x80);
    VBK_REG=0; fill_win_rect(0,0,20,4,32);
    for(i=0;i<40;i++) hide_sprite(i);
    move_bkg(0,0);
}
void page_end(void) { VBK_REG=0; SHOW_BKG; DISPLAY_ON; }
void draw_title(uint8_t choice) {
    uint8_t i,x,y;
    page_begin();
    for(y=0;y<18;y++) for(x=0;x<20;x++) {
        if(y<8) tile(x,y,110,6);
        else if(y>8) tile(x,y,96+((x+y)&1),1);
    }
    text_at(5,1,"EPISODE 01",7);
    for(i=0;i<8;i++) for(y=0;y<2;y++) for(x=0;x<2;x++) tile(2+i*2+x,3+y,128+i*4+y*2+x,i<4?0:3);
    text_at(1,6,"FORT KESTREL / 2131",6);
    text_at(1,8,"TRAINING SIMULATOR",7);
    text_at(3,11,"ENTER SIMULATOR",choice==0?3:0);
    text_at(3,13,"RESUME CODE",choice==1?3:0);
    text_at(3,15,"FIELD MANUAL",choice==2?3:0);
    text_at(1,11+choice*2,">",3);
    text_at(2,17,"A SELECT  V0.3.0",6);
    page_end();
}
void draw_selection(void) {
    page_begin(); frame(0,0,20,18,6);
    text_at(2,1,"FLIGHT SIMULATOR",7);
    text_at(2,3,"DRILL",6); number_at(8,3,mission_index+1,2,3); text_at(11,3,"/ 12",6);
    text_at(1,5,missions[mission_index].title,0);
    text_at(1,7,mode_names[missions[mission_index].type],7);
    text_at(1,9,"SECTOR",6);
    text_at(8,9,missions[mission_index].layout==0?"FLOOD ZONE":(missions[mission_index].layout==1?"SEA WALL":"HANGAR"),0);
    text_at(1,11,"UNLOCKED",6); number_at(11,11,unlocked,2,0); text_at(14,11,"/ 12",6);
    text_at(1,13,"RESUME CODE",6); number_at(13,13,progress_code(unlocked),4,3);
    text_at(1,15,"LEFT/RIGHT TO PICK",0); text_at(1,16,"A BRIEFING  B BACK",7);
    page_end();
}
void draw_brief(void) {
    page_begin(); frame(0,0,20,18,6);
    if(!brief_page) {
        text_at(1,1,"SIMULATION BRIEF",7);
        portrait(1,3); text_at(6,3,"FLIGHT OPS",0); text_at(6,5,"FORT KESTREL",6);
        text_at(1,8,missions[mission_index].title,3);
        text_at(1,10,mode_names[missions[mission_index].type],7);
        text_at(1,12,"TRAINING SCENARIO",6);
        text_at(1,15,"A READ OBJECTIVE",0);
    } else {
        text_at(1,1,missions[mission_index].title,3);
        wrapped(1,3,missions[mission_index].brief,18,11,0);
        text_at(1,15,"A LAUNCH SORTIE",7);
    }
    text_at(1,16,"B BACK",6); page_end();
}
static void map_cell(int16_t column,int16_t row,uint8_t *n,uint8_t *p) {
    uint8_t layout=missions[mission_index].layout;
    uint8_t local_column=(uint8_t)column,local_row=(uint8_t)row;
    while(local_column>=20) local_column-=20;
    while(local_row>=18) local_row-=18;
    if(column==0 || column>=WORLD_WIDTH/8-1 || row<=2 || row>=WORLD_HEIGHT/8-3) { *n=103;*p=3; }
    else if(map_solid(column*8+4,row*8+4)) { *n=(row&1)?101:102;*p=2; }
    else if(layout==0) { *n=96+((column+row)&1);*p=1; }
    else if(layout==1) { *n=(local_row==9 || local_column==10)?99:98;*p=2; }
    else { *n=((column+row)%5==0)?110:98;*p=2; }
    // Small floor intersections distinguish the grid without fencing it in.
    if(local_column==10 && local_row==9) { *n=109;*p=7; }
    if(row>=13 && row<15 && column>=9 && column<11) { *n=111;*p=7; }
}
static void map_column(int16_t column) {
    uint8_t i,tiles[15],palettes[15];
    for(i=0;i<15;i++) map_cell(column,loaded_row+i,&tiles[i],&palettes[i]);
    VBK_REG=1; set_bkg_tiles(column&31,loaded_row&31,1,15,palettes);
    VBK_REG=0; set_bkg_tiles(column&31,loaded_row&31,1,15,tiles);
}
static void map_row(int16_t row) {
    uint8_t i,tiles[21],palettes[21];
    for(i=0;i<21;i++) map_cell(loaded_column+i,row,&tiles[i],&palettes[i]);
    VBK_REG=1; set_bkg_tiles(loaded_column&31,row&31,21,1,palettes);
    VBK_REG=0; set_bkg_tiles(loaded_column&31,row&31,21,1,tiles);
}
static void stream_map(void) {
    int16_t column=game.camera_x>>3,row=game.camera_y>>3;
    while(loaded_column<column) { loaded_column++; map_column(loaded_column+20); }
    while(loaded_column>column) { loaded_column--; map_column(loaded_column); }
    while(loaded_row<row) { loaded_row++; map_row(loaded_row+14); }
    while(loaded_row>row) { loaded_row--; map_row(loaded_row); }
    next_scroll_x=(uint8_t)game.camera_x; next_scroll_y=(uint8_t)game.camera_y;
}
void draw_arena(void) {
    uint8_t i;
    page_begin();
    loaded_column=game.camera_x>>3; loaded_row=game.camera_y>>3;
    for(i=0;i<21;i++) map_column(loaded_column+i);
    hud_hull=hud_boost=hud_progress=hud_goal=hud_core=hud_channel=255;
    hud_time=65535; hud_radio=0; hud_sector=hud_row=hud_direction=255;
    arena_active=1;
    number_at(0,1,mission_index+1,2,3); text_at(3,1,mode_names[missions[mission_index].type],7);
    next_scroll_x=(uint8_t)game.camera_x; next_scroll_y=(uint8_t)game.camera_y;
    draw_game(); SHOW_WIN; SHOW_SPRITES; page_end();
}
static void ship(uint8_t slot,int16_t x,int16_t y,uint8_t first,uint8_t palette) {
    x-=game.camera_x; y-=game.camera_y;
    // Clip before converting to 8-bit OAM coordinates; distant objects must not wrap.
    if(x<=-8 || x>=168 || y<=-8 || y>104) return;
    set_sprite_tile(slot,first); set_sprite_tile(slot+1,first+2);
    set_sprite_prop(slot,palette); set_sprite_prop(slot+1,palette);
    move_sprite(slot,(uint8_t)x,(uint8_t)y+8); move_sprite(slot+1,(uint8_t)x+8,(uint8_t)y+8);
}
static void bullet(uint8_t slot,int16_t x,int16_t y,uint8_t first,uint8_t palette) {
    x-=game.camera_x; y-=game.camera_y;
    if(x<=-4 || x>=164 || y<=-4 || y>104) return;
    set_sprite_tile(slot,first); set_sprite_prop(slot,palette);
    move_sprite(slot,(uint8_t)x+4,(uint8_t)y+8);
}
void draw_game(void) {
    uint8_t i,type=missions[mission_index].type,progress=game.progress,goal=missions[mission_index].goal;
    uint8_t channel_display=(type==RESCUE && game.progress>=3)?2:(game.channel!=0);
    uint16_t left=missions[mission_index].seconds-game.ticks/60u;
    int16_t tx,ty;
    uint8_t direction,sector=game.x/160+1,row=game.y/144+1;
    game_target(&tx,&ty); tx-=game.x; ty-=game.y;
    direction=(tx>16?2:(tx<-16?0:1))+3*(ty>16?2:(ty<-16?0:1));
    static const uint8_t object_tile[7]={0,40,44,48,52,64,70};
    static const uint8_t object_palette[7]={0,7,2,3,4,3,3};
    stream_map();
    for(i=0;i<40;i++) hide_sprite(i);
    if(!(game.invuln&4)) ship(0,game.x,game.y,game.face*4,game.boost?7:0);
    for(i=0;i<ENEMY_COUNT;i++) if(game.enemies[i].hp)
        ship(2+i*2,game.enemies[i].x,game.enemies[i].y,game.enemies[i].kind==2?56:32+game.enemies[i].kind*4,game.enemies[i].flash?7:1);
    for(i=0;i<OBJECT_COUNT;i++) if(game.objects[i].hp)
        ship(14+i*2,game.objects[i].x,game.objects[i].y,object_tile[game.objects[i].kind],object_palette[game.objects[i].kind]);
    for(i=0;i<SHOT_COUNT;i++) if(game.shots[i].active) bullet(22+i,game.shots[i].x,game.shots[i].y,60,6);
    for(i=0;i<HOSTILE_COUNT;i++) if(game.hostile[i].active) bullet(30+i,game.hostile[i].x,game.hostile[i].y,62,1);
    for(i=0;i<EFFECT_COUNT;i++) if(game.effects[i].life) bullet(36+i,game.effects[i].x,game.effects[i].y,68,5);
    const char *message;
    if(hud_hull!=game.hull) {
        text_at(0,0,"HP",6);
        for(i=0;i<MAX_HULL;i++) tile(3+i,0,108,i<game.hull?5:6);
        hud_hull=game.hull;
    }
    if(hud_boost!=(game.boost_cool!=0)) {
        text_at(10,0,game.boost_cool?"B --":"B OK",game.boost_cool?6:7);
        hud_boost=(game.boost_cool!=0);
    }
    if(hud_time!=left) {
        number_at(16,0,left,3,0); text_at(19,0,"S",6);
        hud_time=left;
    }
    if(type==DEFENSE) progress=game.kills;
    if(type==SURVIVAL) progress=game.ticks/60u;
    if(type==ESCORT) { progress=game.objects[0].hp; goal=12; }
    if(type==BOSS || type==CHASE) { progress=game.enemies[0].hp; goal=type==BOSS?36:16; }
    if(hud_progress!=progress || hud_goal!=goal) {
        number_at(14,1,progress,2,0); text_at(16,1,"/",6); number_at(17,1,goal,2,6);
        hud_progress=progress; hud_goal=goal;
    }
    if(hud_channel!=channel_display || hud_core!=game.objects[0].hp || hud_sector!=sector || hud_row!=row || hud_direction!=direction) {
        char nav[21];
        memset(nav,' ',20); nav[20]=0;
        if(type==SABOTAGE && game.channel) {
            memcpy(nav,"UPLINK...",9);
        } else if(type==DEFENSE) {
            memcpy(nav,"BEACON",6);
            nav[7]='0'+game.objects[0].hp/10; nav[8]='0'+game.objects[0].hp%10;
        } else if(type==RESCUE && game.progress>=3) memcpy(nav,"HOME",4);
        else if(type==SURVIVAL) memcpy(nav,"ROAM",4);
        else memcpy(nav,"TARGET",6);
        nav[10]=tx>16?'>':(tx<-16?'<':'=');
        nav[11]=ty>16?'V':(ty<-16?105:'=');
        nav[15]='0'+sector/10; nav[16]='0'+sector%10;
        nav[17]='/'; nav[18]='0'+row/10; nav[19]='0'+row%10;
        text_at(0,16,nav,7);
        hud_channel=channel_display;
        hud_core=game.objects[0].hp;
        hud_sector=sector; hud_row=row; hud_direction=direction;
    }
    message=radio_timer?radio_line:missions[mission_index].hint;
    if(hud_radio!=message) {
        char padded[21];
        memset(padded,' ',20); padded[20]=0;
        for(i=0;i<20 && message[i];i++) padded[i]=message[i];
        text_at(0,17,padded,radio_timer?3:0); hud_radio=message;
    }
}
void draw_pause(uint8_t choice) {
    page_begin(); frame(0,0,20,18,7);
    text_at(2,2,"TIMELINE PAUSED",7);
    text_at(3,6,"RESUME",choice==0?3:0);
    text_at(3,9,"TAKE A MULLIGAN",choice==1?3:0);
    text_at(3,12,"END SORTIE",choice==2?3:0);
    text_at(1,6+choice*3,">",3);
    text_at(2,15,"A SELECT B RESUME",6); page_end();
}
void draw_fallen(void) {
    page_begin(); frame(0,0,20,18,4);
    text_at(2,2,"TIMELINE BROKEN",4); text_at(1,5,failure_reason,0);
    text_at(1,8,"A TAKE A MULLIGAN",7);
    text_at(1,10,"RESTORE CHECKPOINT",6);
    text_at(1,13,"B RESTART SORTIE",0); text_at(1,15,"START DRILL MENU",6); page_end();
}
void draw_debrief(void) {
    page_begin(); frame(0,0,20,18,7);
    text_at(2,1,"SORTIE COMPLETE",7);
    if(!brief_page) {
        text_at(1,4,"TIMELINE DAMAGE",6);
        text_at(1,6,rewinds==0?"MINIMAL":(rewinds<3?"CONTAINED":"UNSTABLE"),rewinds==0?5:3);
        text_at(1,8,"SCORE",6); number_at(12,8,game.score,5,0);
        text_at(1,10,"MULLIGANS",6); number_at(14,10,rewinds,3,0);
        text_at(1,12,"RESUME CODE",6); number_at(13,12,progress_code(unlocked),4,3);
        text_at(1,15,"A DEBRIEF",7);
    } else {
        wrapped(1,3,missions[mission_index].debrief,18,11,0);
        text_at(1,15,"A CONTINUE",7);
    }
    text_at(1,16,"B DRILL MENU",6); page_end();
}
void draw_manual(void) {
    page_begin(); text_at(3,1,"FIELD MANUAL",7);
    text_at(1,3,"D PAD   FLY / AIM",0); text_at(1,5,"A       FIRE",0);
    text_at(1,7,"HOLD A  LOCK AIM",0); text_at(1,9,"B       BOOST",0);
    text_at(1,11,"START   PAUSE",0); text_at(1,13,"SELECT  SOUND",0);
    text_at(1,15,"CODES KEEP PROGRESS",6); text_at(1,17,"A / B BACK",3); page_end();
}
void draw_password(const uint8_t *digits,uint8_t cursor,uint8_t invalid) {
    uint8_t i; page_begin(); frame(0,0,20,18,6);
    text_at(4,2,"RESUME CODE",7);
    for(i=0;i<4;i++) { number_at(5+i*3,7,digits[i],1,i==cursor?3:0); if(i==cursor) text_at(5+i*3,9,"=",3); }
    text_at(2,4,"TRAINING PROGRESS",6);
    text_at(2,11,invalid?"INVALID CODE":"ENTER FOUR DIGITS",invalid?4:6);
    text_at(1,13,"UP/DOWN CHANGE",0); text_at(1,14,"LEFT/RIGHT MOVE",0);
    text_at(1,16,"A RESUME  B BACK",7); page_end();
}
void draw_complete(void) {
    page_begin(); frame(0,0,20,18,7);
    text_at(2,2,"TRAINING COMPLETE",3); portrait(8,4);
    text_at(2,9,"12 DRILLS CLEARED",0);
    text_at(1,11,"FORT KESTREL / 2131",6);
    text_at(2,13,"RESUME CODE",6); number_at(14,13,progress_code(unlocked),4,3);
    text_at(2,16,"A REPLAY DRILLS",7); page_end();
}
