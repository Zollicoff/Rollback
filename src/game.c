#include "game.h"
#include <string.h>

Game game;
static Game checkpoint;
uint8_t radio_timer;
const char *radio_line;
const char *failure_reason;
static uint8_t radio_priority;
static const int8_t vx[8]={0,1,1,1,0,-1,-1,-1};
static const int8_t vy[8]={-1,-1,0,1,1,1,0,-1};
static const int16_t gate_x[4]={240,800,1440,1552};
static const int16_t gate_y[4]={224,752,1232,1376};
static const int16_t route_x[4]={400,400,1520,1520};
static const int16_t route_y[4]={80,656,656,1376};

static int8_t sign(int16_t n) { return (n>0)-(n<0); }
static uint16_t distance(int16_t n) { return n<0?-n:n; }
static uint8_t near(int16_t x,int16_t y,int16_t tx,int16_t ty,uint8_t radius) {
    return distance(x-tx)<radius && distance(y-ty)<radius;
}
static uint8_t random_byte(void) {
    game.rng^=game.rng<<7; game.rng^=game.rng>>9; game.rng^=game.rng<<8;
    return (uint8_t)game.rng;
}
uint16_t progress_code(uint8_t count) {
    return 1000u+count*137u+(count*7u)%10u;
}
uint8_t progress_decode(uint16_t code) {
    uint8_t i;
    for(i=1;i<=MISSION_COUNT;i++) if(progress_code(i)==code) return i;
    return 0;
}
uint8_t map_solid(int16_t x,int16_t y) {
    uint8_t layout=missions[mission_index].layout;
    uint8_t sector=0,local;
    if(x<8 || x>=WORLD_WIDTH-8 || y<24 || y>=WORLD_HEIGHT-24) return 1;
    // Leave a ship-wide perimeter lane; repeated cover must not pinch the
    // flight path against a world boundary.
    if(x<24 || x>=WORLD_WIDTH-24 || y<40 || y>=WORLD_HEIGHT-40) return 0;
    // Bounded subtraction avoids repeated 16-bit division in the collision
    // and tile-streaming hot path (at most nine steps on either axis).
    while(x>=160) { x-=160; sector++; }
    while(y>=144) { y-=144; sector++; }
    local=(uint8_t)x;
    // Cover varies across both axes; connected horizontal and vertical lanes
    // let vehicles navigate between all one hundred sectors.
    if(layout==0) return (local>=16+8*(sector%3) && local<64 && y>=40 && y<56) ||
                        (local>=104 && local<144 && y>=96 && y<104 && (sector&1));
    if(layout==1) return (y>=48 && y<64 && local>=24 && local<64) ||
                        (y>=96 && y<104 && local>=104 && local<136 && sector%3!=1);
    return ((y>=40 && y<56 && local>=16 && local<48) ||
            (y>=96 && y<104 && local>=112 && local<144)) && sector%3!=2;
}
void radio(const char *line,uint8_t priority) {
    if(radio_timer && priority<radio_priority) return;
    radio_line=line; radio_timer=150; radio_priority=priority;
}
static void effect(int16_t x,int16_t y) {
    uint8_t i;
    for(i=0;i<EFFECT_COUNT;i++) if(!game.effects[i].life) {
        game.effects[i].x=x; game.effects[i].y=y; game.effects[i].life=16; return;
    }
}
void game_checkpoint(void) {
    memcpy(&checkpoint,&game,sizeof(Game));
    radio("CHECKPOINT LOCKED",2);
}
void game_rewind(void) {
    memcpy(&game,&checkpoint,sizeof(Game));
    game.invuln=60;
    rewinds++; screen=PLAYING;
    radio("TIMELINE RESTORED",3); audio_sfx(S_REWIND);
}
static void fail(const char *reason) {
    failure_reason=reason; screen=FALLEN; audio_sfx(S_BOOM);
}
static void win(void) {
    if(mission_index+1>=unlocked && unlocked<MISSION_COUNT) unlocked=mission_index+2;
    screen=DEBRIEF; audio_sfx(S_WIN);
}
static void damage_player(void) {
    if(game.invuln || game.boost || screen!=PLAYING) return;
    if(game.hull) game.hull--;
    game.invuln=60; audio_sfx(S_HIT); effect(game.x,game.y);
    radio("HULL DAMAGE",1);
    if(!game.hull) fail("YOUR HULL FAILED");
}
static void destroy_enemy(uint8_t i) {
    Enemy *e=&game.enemies[i];
    e->hp=0; game.kills++; game.score+=100;
    effect(e->x,e->y); audio_sfx(S_BOOM);
    if(e->kind==2) { game.progress=1; return; }
    if(game.kills%3==0 && !game.objects[3].hp) {
        game.objects[3].x=e->x; game.objects[3].y=e->y;
        game.objects[3].hp=1; game.objects[3].kind=5;
    }
}
static void spawn_enemy(void) {
    uint8_t i,r;
    Enemy *e;
    for(i=0;i<ENEMY_COUNT;i++) if(!game.enemies[i].hp) {
        e=&game.enemies[i]; r=random_byte();
        e->x=(missions[mission_index].type==DEFENSE || missions[mission_index].type==ESCORT)?game.objects[0].x:game.x;
        e->x+=(r&1)?88:-88;
        if(e->x<16) e->x=16; if(e->x>WORLD_WIDTH-16) e->x=WORLD_WIDTH-16;
        e->y=(missions[mission_index].type==DEFENSE || missions[mission_index].type==ESCORT)?game.objects[0].y:game.y;
        e->y+=(r&4)?64:-64;
        if(e->y<32) e->y=32; if(e->y>WORLD_HEIGHT-32) e->y=WORLD_HEIGHT-32;
        if(map_solid(e->x,e->y)) e->x=(e->x/160)*160+80;
        e->kind=(r&8)?1:0; e->hp=missions[mission_index].difficulty>1?3:2;
        e->cool=60+(r&63); e->flash=0;
        game.spawned++; return;
    }
}
static void shoot(Shot *pool,uint8_t count,int16_t x,int16_t y,int8_t dx,int8_t dy,uint8_t speed) {
    uint8_t i;
    for(i=0;i<count;i++) if(!pool[i].active) {
        pool[i].x=x; pool[i].y=y; pool[i].dx=dx*speed; pool[i].dy=dy*speed;
        pool[i].active=1; pool[i].life=100; return;
    }
}
static void aim_shot(int16_t x,int16_t y,int16_t tx,int16_t ty) {
    int16_t dx=tx-x,dy=ty-y;
    int8_t sx=sign(dx),sy=sign(dy);
    if(distance(dx)>distance(dy)*2u) sy=0;
    if(distance(dy)>distance(dx)*2u) sx=0;
    if(!sx && !sy) sy=1;
    shoot(game.hostile,HOSTILE_COUNT,x,y,sx,sy,1);
}
void game_start(void) {
    uint8_t i,type=missions[mission_index].type;
    memset(&game,0,sizeof(Game));
    game.x=80; game.y=72; game.hull=MAX_HULL; game.rng=0x6a31+mission_index*139;
    game.spawn_cool=90; game.invuln=60; rewinds=0;
    if(type==DEFENSE || type==ESCORT) {
        game.objects[0].x=type==DEFENSE?800:80; game.objects[0].y=type==DEFENSE?800:80;
        game.objects[0].hp=12; game.objects[0].kind=type==DEFENSE?1:4;
        if(type==DEFENSE) { game.x=800; game.y=832; game.camera_x=720; game.camera_y=776; }
    } else if(type==RAID || type==SABOTAGE || type==RESCUE) {
        for(i=0;i<3;i++) {
            game.objects[i].x=gate_x[i]; game.objects[i].y=gate_y[i];
            game.objects[i].kind=type==RESCUE?3:2;
            game.objects[i].hp=type==RESCUE?1:4+missions[mission_index].difficulty;
        }
    } else if(type==EVADE) {
        game.objects[0].x=gate_x[0]; game.objects[0].y=gate_y[0];
        game.objects[0].kind=6; game.objects[0].hp=1;
    } else if(type==BOSS || type==CHASE) {
        game.enemies[0].x=type==BOSS?1456:240; game.enemies[0].y=type==BOSS?1328:224;
        game.enemies[0].kind=2; game.enemies[0].hp=type==BOSS?36:16;
        game.enemies[0].cool=60;
    }
    radio_timer=0; radio_priority=0;
    screen=PLAYING; game_checkpoint(); radio(missions[mission_index].hint,3);
}
static void player_tick(uint8_t held,uint8_t pressed) {
    int8_t dx=0,dy=0;
    int16_t nx,ny;
    uint8_t i,channeling=0;
    if(held&J_LEFT) dx=-1; if(held&J_RIGHT) dx=1;
    if(held&J_UP) dy=-1; if(held&J_DOWN) dy=1;
    if((dx || dy) && !(held&J_A)) {
        for(i=0;i<8;i++) if(dx==vx[i] && dy==vy[i]) { game.face=i; break; }
    }
    if(missions[mission_index].type==SABOTAGE && held&J_B) {
        for(i=0;i<3;i++) if(game.objects[i].hp && near(game.x,game.y,game.objects[i].x,game.objects[i].y,18)) {
            channeling=1;
            if(++game.channel>=90) {
                game.objects[i].hp=0; game.progress++; game.score+=250; game.channel=0;
                audio_sfx(S_PICKUP); radio("RELAY DISABLED",2);
            }
            break;
        }
    }
    if(!channeling) game.channel=0;
    if((pressed&J_B) && !channeling && !game.boost_cool) {
        game.boost=8; game.boost_cool=90; game.move_x=dx; game.move_y=dy;
        if(!dx && !dy) { game.move_x=vx[game.face]; game.move_y=vy[game.face]; }
        audio_sfx(S_CONFIRM);
    }
    if(channeling) { dx=0; dy=0; }
    else if(game.boost) { dx=game.move_x*3; dy=game.move_y*3; }
    // Diagonal normal flight alternates axes every other frame to bound speed.
    if(!game.boost && dx && dy && (game.ticks&1)) { if(game.ticks&2) dx=0; else dy=0; }
    nx=game.x+dx; ny=game.y+dy;
    if(!map_solid(nx-4,game.y-4) && !map_solid(nx+4,game.y+4)) game.x=nx;
    if(!map_solid(game.x-4,ny-4) && !map_solid(game.x+4,ny+4)) game.y=ny;
    if(game.x-game.camera_x>CAMERA_RIGHT) game.camera_x=game.x-CAMERA_RIGHT;
    else if(game.x-game.camera_x<CAMERA_LEFT) game.camera_x=game.x-CAMERA_LEFT;
    if(game.camera_x<0) game.camera_x=0;
    if(game.camera_x>WORLD_WIDTH-160) game.camera_x=WORLD_WIDTH-160;
    if(game.y-game.camera_y>CAMERA_BOTTOM) game.camera_y=game.y-CAMERA_BOTTOM;
    else if(game.y-game.camera_y<CAMERA_TOP) game.camera_y=game.y-CAMERA_TOP;
    if(game.camera_y<0) game.camera_y=0;
    if(game.camera_y>WORLD_HEIGHT-112) game.camera_y=WORLD_HEIGHT-112;
    if(game.boost) game.boost--;
    if(game.boost_cool) game.boost_cool--;
    if(game.invuln) game.invuln--;
    if(game.fire) game.fire--;
    if((held&J_A) && !game.fire && !channeling) {
        shoot(game.shots,SHOT_COUNT,game.x+vx[game.face]*8,game.y+vy[game.face]*8,vx[game.face],vy[game.face],3);
        game.fire=9; audio_sfx(S_FIRE);
    }
}
static void enemies_tick(void) {
    uint8_t i,type=missions[mission_index].type,phase,direction;
    int16_t tx,ty,nx,ny;
    Enemy *e;
    if(game.spawn_cool) game.spawn_cool--;
    if(!game.spawn_cool && type!=BOSS) {
        if(type!=DEFENSE || game.spawned<missions[mission_index].goal) spawn_enemy();
        game.spawn_cool=110-missions[mission_index].difficulty*20;
    }
    for(i=0;i<ENEMY_COUNT;i++) {
        e=&game.enemies[i]; if(!e->hp) continue;
        if(e->kind!=2 && type!=DEFENSE && type!=ESCORT && (distance(e->x-game.x)>272 || distance(e->y-game.y)>240)) { e->hp=0; continue; }
        if(e->flash) e->flash--;
        tx=game.x; ty=game.y;
        if(type==DEFENSE || type==ESCORT) { tx=game.objects[0].x; ty=game.objects[0].y; }
        if(e->kind==2) {
            if((game.ticks&1)==0) {
                if(type==CHASE) { if(e->x<1480) e->x++; if(e->y<1232) e->y++; }
                else {
                    e->x+=((game.ticks/112)&1)?-1:1;
                    if(e->x<1360) e->x=1360; if(e->x>1552) e->x=1552;
                }
            }
        } else if((game.ticks+i)%3==0) {
            nx=e->x+sign(tx-e->x); ny=e->y+sign(ty-e->y);
            if(!map_solid(nx,e->y)) e->x=nx;
            if(!map_solid(e->x,ny)) e->y=ny;
        }
        if(e->cool) e->cool--;
        if(!e->cool && distance(e->x-tx)<176 && distance(e->y-ty)<144) {
            if(e->kind==2 && type==BOSS) {
                phase=e->hp>24?0:(e->hp>12?1:2);
                if(game.channel!=phase) {
                    game.channel=phase;
                    radio(phase==1?"TARGET PHASE TWO":"TARGET PHASE THREE",2);
                    audio_sfx(S_CONFIRM);
                }
                aim_shot(e->x,e->y,game.x,game.y);
                if(phase) {
                    direction=(game.ticks/30)&7;
                    shoot(game.hostile,HOSTILE_COUNT,e->x,e->y,vx[direction],vy[direction],1);
                    shoot(game.hostile,HOSTILE_COUNT,e->x,e->y,vx[(direction+4)&7],vy[(direction+4)&7],1);
                }
                e->cool=60-phase*12;
            } else { aim_shot(e->x,e->y,tx,ty); e->cool=150-missions[mission_index].difficulty*20; }
        }
        if(near(e->x,e->y,game.x,game.y,11)) damage_player();
    }
}
static void shots_tick(void) {
    uint8_t i,j,type=missions[mission_index].type;
    Shot *s;
    for(i=0;i<SHOT_COUNT;i++) {
        s=&game.shots[i]; if(!s->active) continue;
        s->x+=s->dx; s->y+=s->dy;
        if(!s->life-- || map_solid(s->x,s->y)) { s->active=0; continue; }
        for(j=0;j<ENEMY_COUNT;j++) if(game.enemies[j].hp && near(s->x,s->y,game.enemies[j].x,game.enemies[j].y,9)) {
            game.enemies[j].hp--; game.enemies[j].flash=5; s->active=0;
            if(!game.enemies[j].hp) destroy_enemy(j); else audio_sfx(S_HIT);
            break;
        }
        if(s->active && type==RAID) for(j=0;j<3;j++) if(game.objects[j].hp && near(s->x,s->y,game.objects[j].x,game.objects[j].y,10)) {
            s->active=0; game.objects[j].hp--; audio_sfx(S_HIT);
            if(!game.objects[j].hp) { game.progress++; game.score+=250; effect(s->x,s->y); radio("RELAY DESTROYED",2); }
            break;
        }
    }
    for(i=0;i<HOSTILE_COUNT;i++) {
        s=&game.hostile[i]; if(!s->active) continue;
        s->x+=s->dx; s->y+=s->dy;
        if(!s->life-- || map_solid(s->x,s->y)) { s->active=0; continue; }
        if(near(s->x,s->y,game.x,game.y,6)) { s->active=0; damage_player(); }
        if(s->active && (type==DEFENSE || type==ESCORT) && near(s->x,s->y,game.objects[0].x,game.objects[0].y,9)) {
            s->active=0;
            if(game.objects[0].hp) game.objects[0].hp--;
            effect(game.objects[0].x,game.objects[0].y);
            if(!game.objects[0].hp) fail(type==DEFENSE?"BEACON LOST":"TRANSPORT LOST");
        }
    }
}
static void objectives_tick(void) {
    uint8_t i,type=missions[mission_index].type,checkpoint_ready=0;
    Objective *o;
    if(type==ESCORT) {
        o=&game.objects[0];
        if(++game.convoy_clock>=2 && game.convoy_step<4) {
            game.convoy_clock=0;
            o->x+=sign((int16_t)route_x[game.convoy_step]-o->x);
            o->y+=sign(route_y[game.convoy_step]-o->y);
            if(o->x==route_x[game.convoy_step] && o->y==route_y[game.convoy_step]) { game.convoy_step++; game.progress++; }
        }
    }
    if(type==RESCUE) for(i=0;i<3;i++) {
        o=&game.objects[i];
        if(o->hp && near(game.x,game.y,o->x,o->y,12)) {
            o->hp=0; game.progress++; game.score+=200; audio_sfx(S_PICKUP); radio("POD RECOVERED",2);
        }
    }
    if(type==EVADE) {
        o=&game.objects[0];
        if(near(game.x,game.y,o->x,o->y,12)) {
            game.progress++; audio_sfx(S_PICKUP); radio("GATE CONFIRMED",2);
            if(game.progress<4) { o->x=gate_x[game.progress]; o->y=gate_y[game.progress]; }
        }
    }
    o=&game.objects[3];
    if(o->hp && o->kind==5 && near(game.x,game.y,o->x,o->y,12)) {
        o->hp=0; game.hull+=2; if(game.hull>MAX_HULL) game.hull=MAX_HULL;
        radio("HULL REPAIRED",1); audio_sfx(S_PICKUP);
    }
    if(screen!=PLAYING) return;
    if((type==DEFENSE && game.kills>=missions[mission_index].goal) ||
       ((type==RAID || type==SABOTAGE) && game.progress>=3) ||
       ((type==BOSS || type==CHASE) && game.progress==1) ||
       ((type==ESCORT || type==EVADE) && game.progress>=4) ||
       (type==RESCUE && game.progress>=3 && near(game.x,game.y,80,112,14))) { win(); return; }
    if(game.ticks>=missions[mission_index].seconds*60u) {
        if(type==SURVIVAL) win(); else fail("TIME EXPIRED");
        return;
    }
    if(game.checkpoint_mark || !game.hull) return;
    switch(type) {
        case DEFENSE: checkpoint_ready=game.kills>=(missions[mission_index].goal>>1); break;
        case RAID: case RESCUE: case SABOTAGE: case ESCORT: case EVADE:
            checkpoint_ready=game.progress>=2; break;
        case SURVIVAL: checkpoint_ready=game.ticks>=missions[mission_index].seconds*30u; break;
        case BOSS: checkpoint_ready=game.enemies[0].hp<=18; break;
        default: break;
    }
    if(checkpoint_ready) {
        game.checkpoint_mark=1; game_checkpoint();
    }
}
void game_target(int16_t *x,int16_t *y) {
    uint8_t i,type=missions[mission_index].type;
    uint16_t best=65535,delta;
    *x=game.x; *y=game.y;
    if(type==DEFENSE || type==ESCORT || type==EVADE) { *x=game.objects[0].x; *y=game.objects[0].y; return; }
    if(type==BOSS || type==CHASE) { *x=game.enemies[0].x; *y=game.enemies[0].y; return; }
    if(type==RESCUE && game.progress>=3) { *x=80; *y=112; return; }
    if(type==RAID || type==RESCUE || type==SABOTAGE) {
        for(i=0;i<3;i++) if(game.objects[i].hp) {
            delta=distance(game.x-game.objects[i].x)+distance(game.y-game.objects[i].y);
            if(delta<best) { best=delta; *x=game.objects[i].x; *y=game.objects[i].y; }
        }
    }
}
void game_tick(uint8_t held,uint8_t pressed,uint8_t elapsed) {
    uint8_t i;
    game.ticks+=elapsed;
    if(radio_timer) radio_timer--;
    for(i=0;i<EFFECT_COUNT;i++) if(game.effects[i].life) game.effects[i].life--;
    player_tick(held,pressed); enemies_tick();
    if(screen!=PLAYING) return;
    shots_tick();
    if(screen==PLAYING) objectives_tick();
}
