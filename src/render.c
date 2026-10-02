#pragma bank 255
#include "game.h"
#include <string.h>
extern uint32_t seen_enemies;
extern uint8_t hud_hull,hud_boost,hud_progress,hud_goal,hud_core,hud_channel;
extern uint16_t hud_time;
extern uint8_t hud_sector,hud_row,hud_direction;
extern char hud_radio[21];
extern volatile uint8_t arena_active,next_scroll_x,next_scroll_y;
extern int16_t loaded_column,loaded_row;
void tile(uint8_t x,uint8_t y,uint8_t n,uint8_t p) NONBANKED;
static const palette_color_t terrain_palettes[40]={
 RGB(2,3,6),RGB(3,9,13),RGB(4,14,18),RGB(11,24,25),
 RGB(3,6,12),RGB(6,13,19),RGB(12,23,25),RGB(25,29,29),
 RGB(4,3,7),RGB(6,7,11),RGB(13,15,19),RGB(27,20,9),
 RGB(3,5,7),RGB(5,10,8),RGB(13,18,10),RGB(23,26,18),
 RGB(4,4,8),RGB(9,10,12),RGB(15,17,16),RGB(24,25,18),
 RGB(5,2,8),RGB(10,5,15),RGB(19,12,23),RGB(29,23,13),
 RGB(6,5,3),RGB(12,11,8),RGB(22,20,15),RGB(30,28,23),
 RGB(3,5,5),RGB(7,11,9),RGB(12,17,13),RGB(25,24,15),
 RGB(2,3,5),RGB(5,7,9),RGB(10,13,15),RGB(19,23,24),
 RGB(3,5,10),RGB(6,11,15),RGB(12,21,22),RGB(27,30,23)
};
static void map_cell(int16_t column,int16_t row,uint8_t *n,uint8_t *p) {
    uint8_t layout=mission.layout;
    uint8_t local_column=(uint8_t)column,local_row=(uint8_t)row;
    while(local_column>=20) local_column-=20;
    while(local_row>=18) local_row-=18;
    if(column==0 || column>=WORLD_WIDTH/8-1 || row<=2 || row>=WORLD_HEIGHT/8-3) { *n=103;*p=3; }
    else if(map_solid(column*8+4,row*8+4)) { *n=(row&1)?101:102;*p=2; }
    else if(layout<=2) { *n=96+((column+row)&1);*p=1; }
    else if(layout==3 || layout==7 || layout==9) { *n=(local_row==9 || local_column==10)?99:98;*p=2; }
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
void draw_arena(void) BANKED {
    uint8_t i;
    page_begin();
    set_bkg_palette(1,1,&terrain_palettes[mission.layout*4]);
    loaded_column=game.camera_x>>3; loaded_row=game.camera_y>>3;
    for(i=0;i<21;i++) map_column(loaded_column+i);
    hud_hull=hud_boost=hud_progress=hud_goal=hud_core=hud_channel=255;
    hud_time=65535; hud_radio[0]=0; hud_sector=hud_row=hud_direction=255;
    arena_active=1;
    number_at(0,1,mission_index+1,2,3); text_at(3,1,mode_names[mission.type],7);
    next_scroll_x=(uint8_t)game.camera_x; next_scroll_y=(uint8_t)game.camera_y;
    draw_game(); SHOW_WIN; SHOW_SPRITES; page_end();
}
static void ship(uint8_t slot,int16_t x,int16_t y,uint8_t first,uint8_t palette) {
    x-=game.camera_x; y-=game.camera_y;
    // Clip before converting to 8-bit OAM coordinates; distant objects must not wrap.
    if(x<=-8 || x>=168 || y<=-8 || y>104) return;
    if(slot>=2 && slot<=12 && first>=90 && first<158)seen_enemies|=1ul<<((first-90)/4);
    if(slot>=14 && (first==98 || first==102 || first==106))seen_enemies|=1ul<<((first-90)/4);
    set_sprite_tile(slot,first); set_sprite_tile(slot+1,first+2);
    set_sprite_prop(slot,palette|8); set_sprite_prop(slot+1,palette|8);
    move_sprite(slot,(uint8_t)x,(uint8_t)y+8); move_sprite(slot+1,(uint8_t)x+8,(uint8_t)y+8);
}
static void bullet(uint8_t slot,int16_t x,int16_t y,uint8_t first,uint8_t palette) {
    x-=game.camera_x; y-=game.camera_y;
    if(x<=-4 || x>=164 || y<=-4 || y>104) return;
    set_sprite_tile(slot,first); set_sprite_prop(slot,palette|8);
    move_sprite(slot,(uint8_t)x+4,(uint8_t)y+8);
}
// Eight-way arrows point from the ship toward a target. Clamp their position
// inside the playfield; the fixed HUD starts at scanline 112.
static void waypoint(uint8_t slot,int16_t x,int16_t y,uint8_t palette) {
    int16_t dx=x-game.x,dy=y-game.y,sx,sy;
    uint16_t ax=dx<0?-dx:dx,ay=dy<0?-dy:dy;
    uint8_t dir;
    if(ax<12 && ay<12) return;
    if(ax>ay*2u) dir=dx>0?2:6;
    else if(ay>ax*2u) dir=dy>0?4:0;
    else dir=dy>0?(dx>0?3:5):(dx>0?1:7);
    sx=x-game.camera_x; sy=y-game.camera_y-14;
    if(sx<8) sx=8; if(sx>152) sx=152;
    if(sy<8) sy=8; if(sy>100) sy=100;
    // Separate a threat arrow from an objective at the same screen edge.
    if(slot==39) { if(sx>140) sx-=10; else sx+=10; }
    set_sprite_tile(slot,74+dir*2); set_sprite_prop(slot,palette|8);
    move_sprite(slot,(uint8_t)sx+4,(uint8_t)sy+12);
}
static void draw_waypoints(void) {
    uint8_t i,type=mission.type,nearest=255;
    uint16_t best=65535,d;
    int16_t x,y,dx,dy;
    if(type!=SURVIVAL) {
        game_target(&x,&y);
        waypoint(38,x,y,(type==DEFENSE || type==ESCORT)?3:
            (type==RAID || type==BOSS || mission.rule==FLEET)?1:type==SABOTAGE?2:6);
    }
    // Always identify the closest living threat, including during protection
    // and rescue missions. Boss/chase already identify their primary enemy.
    if(type==BOSS || mission.rule==FLEET) return;
    for(i=0;i<ENEMY_COUNT;i++) if(game.enemies[i].hp) {
        dx=game.enemies[i].x-game.x; dy=game.enemies[i].y-game.y;
        d=(dx<0?-dx:dx)+(dy<0?-dy:dy);
        if(d<best) { best=d; nearest=i; }
    }
    if(nearest!=255) waypoint(39,game.enemies[nearest].x,game.enemies[nearest].y,1);
}
void draw_game(void) BANKED {
    uint8_t i,type=mission.type,progress=game.progress,goal=mission.goal;
    uint8_t channel_display=(type==RESCUE && game.progress>=mission.goal)?2:(game.channel!=0);
    uint16_t seconds=game.ticks/60u,left=seconds<mission.seconds?mission.seconds-seconds:0;
    int16_t tx,ty;
    uint8_t direction,sector=game.x/160+1,row=game.y/144+1;
    game_target(&tx,&ty); tx-=game.x; ty-=game.y;
    direction=(tx>16?2:(tx<-16?0:1))+3*(ty>16?2:(ty<-16?0:1));
    static const uint8_t object_tile[7]={0,40,44,48,52,64,70};
    static const uint8_t object_palette[7]={0,7,2,3,4,3,3};
    stream_map();
    for(i=0;i<40;i++) hide_sprite(i);
    if(!(game.invuln&4)) ship(0,game.x,game.y,game.face*4,game.boost?7:game.shield?6:0);
    for(i=0;i<ENEMY_COUNT;i++) if(game.enemies[i].hp)
        ship(2+i*2,game.enemies[i].x,game.enemies[i].y,90+game.enemies[i].role*4,game.enemies[i].flash?7:(game.enemies[i].role==14?2:1));
    for(i=0;i<OBJECT_COUNT;i++) if(game.objects[i].hp) {
        uint8_t appearance=object_tile[game.objects[i].kind],color=object_palette[game.objects[i].kind];
        if(game.objects[i].kind==2) {
            if(mission_index==4)appearance=98;
            else if(mission_index==7)appearance=102;
            else if(mission_index==14)appearance=106;
            else if(mission_index==34) {appearance=146;color=3;}
        }
        ship(14+i*2,game.objects[i].x,game.objects[i].y,appearance,color);
    }
    for(i=0;i<SHOT_COUNT;i++) if(game.shots[i].active) bullet(22+i,game.shots[i].x,game.shots[i].y,60,6);
    for(i=0;i<HOSTILE_COUNT;i++) if(game.hostile[i].active) bullet(30+i,game.hostile[i].x,game.hostile[i].y,62,1);
    for(i=0;i<EFFECT_COUNT-2;i++) if(game.effects[i].life) bullet(36+i,game.effects[i].x,game.effects[i].y,68,5);
    if(mission.rule==DUPLICATE || mission.rule==UPLOAD_RACE)ship(12,game.echo_x,game.echo_y,game.face*4,6);
    draw_waypoints();
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
    if(type==DEFENSE || mission.rule==PILOT_RESCUE)progress=game.kills;
    text_at(14,0,game.weapon==1?"S":game.weapon==2?"L":game.weapon==3?"M":"1",3);
    if(type==SURVIVAL) progress=game.ticks/60u;
    if(type==ESCORT) { progress=game.objects[0].hp; goal=12; }
    if(type==BOSS) {progress=game.enemies[(mission.rule==DUAL_BOSS && !game.enemies[0].hp)?1:0].hp;goal=36;}
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
        } else if(type==RESCUE && game.progress>=mission.goal) memcpy(nav,"HOME",4);
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
    message=radio_timer?radio_line:mission.hint;
    if(mission.rule==RADAR)message=game.ticks%720u>=600u?"RADAR SWEEP - COVER!":game.ticks%720u>=480u?"RADAR SOON - COVER!":"RADAR CLEAR - MOVE!";
    if(mission.rule==VIRUS && game.upload_ticks && !radio_timer)message="UPLOADING: HOLD SITE";
    if(strncmp(hud_radio,message,20)) {
        char padded[21];
        memset(padded,' ',20); padded[20]=0;
        for(i=0;i<20 && message[i];i++) padded[i]=message[i];
        text_at(0,17,padded,radio_timer?3:0); strncpy(hud_radio,message,20);hud_radio[20]=0;
    }
}
