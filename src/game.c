#pragma bank 255
#include "game.h"
#include <string.h>

Game game;
static Game checkpoint;
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
static void effect(int16_t x,int16_t y) {
    uint8_t i;
    for(i=0;i<EFFECT_COUNT;i++) if(!game.effects[i].life) {
        game.effects[i].x=x; game.effects[i].y=y; game.effects[i].life=16; return;
    }
}
void game_checkpoint(void) BANKED {
    if(game.hull<3)game.hull=3;
    memcpy(&checkpoint,&game,sizeof(Game));
    radio("CHECKPOINT LOCKED",2);
}
void game_rewind(void) BANKED {
    memcpy(&game,&checkpoint,sizeof(Game));
    game.invuln=60;
    rewinds++; total_rewinds++; if(total_rewinds>=10)achievements|=1ul<<5; screen=PLAYING;
    bark_reset(); bark(BK_RETRY); audio_sfx(S_REWIND);
}
void game_finish(void) BANKED { game.finishing=1; }
static void damage_player(void) {
    if(game.invuln || game.boost || game.shield || screen!=PLAYING) return;
    if(game.hull) game.hull--; game.hits++;
    game.invuln=60; audio_sfx(S_HIT); effect(game.x,game.y);
    bark(game.hull<2?BK_LOW:BK_DAMAGE);
    if(!game.hull) {
        if(game.extra) { uint8_t remaining=game.extra-1; game_rewind(); game.extra=remaining; }
        else game_fail("YOUR HULL FAILED");
    }
}
static void destroy_enemy(uint8_t i) {
    Enemy *e=&game.enemies[i];uint8_t k;
    e->hp=0; game.kills++; total_kills++; game.score+=100;
    if(total_kills>=212)achievements|=1ul<<4;
    if(e->role==14 || e->role==15)bark(BK_DESTROY);
    for(k=0;k<3;k++)game.kill_times[k]=game.kill_times[k+1];
    game.kill_times[3]=game.ticks;if(game.chain<4)game.chain++;
    if(game.chain>=4 && game.ticks-game.kill_times[0]<=300)achievements|=1ul<<3;
    bark(game.chain>=3 && game.ticks-game.kill_times[1]<=180?BK_MULTI:BK_KILL);
    effect(e->x,e->y); audio_sfx(S_BOOM);
    if(e->kind==2) {game.progress++;return;}
    if(game.kills%3==0 && !game.objects[3].hp) {
        game.objects[3].x=e->x; game.objects[3].y=e->y;
        game.objects[3].hp=1; game.objects[3].kind=5; game.pickup=(game.kills/3+mission_index)%8;
    }
}
static void spawn_enemy(void) {
    uint8_t i,r;
    Enemy *e;
    for(i=0;i<((mission.rule==DUPLICATE || mission_index==6 || mission_index==37)?ENEMY_COUNT-1:ENEMY_COUNT);i++) if(!game.enemies[i].hp) {
        e=&game.enemies[i]; r=random_byte();
        e->x=(mission.type==DEFENSE || mission.type==ESCORT || mission.rule==PILOT_RESCUE || mission.rule==VIRUS)?game.objects[0].x:game.x;
        e->x+=(r&1)?88:-88;
        if(e->x<16) e->x=16; if(e->x>WORLD_WIDTH-16) e->x=WORLD_WIDTH-16;
        e->y=(mission.type==DEFENSE || mission.type==ESCORT || mission.rule==PILOT_RESCUE || mission.rule==VIRUS)?game.objects[0].y:game.y;
        e->y+=(r&4)?64:-64;
        if(e->y<32) e->y=32; if(e->y>WORLD_HEIGHT-32) e->y=WORLD_HEIGHT-32;
        if(map_solid(e->x,e->y)) e->x=(e->x/160)*160+80;
        e->kind=(r&8)?1:0; e->role=(mission.enemy==0 && e->kind==1)?1:mission.enemy;
        if((mission_index==17 && game.kills>=4) || (mission_index==18 && game.progress))e->role=4;
        e->hp=difficulty>1?3:2;
        e->cool=60+(r&63); e->flash=0;
        game.spawned++; if(e->role==14 || e->role==15)bark(BK_APPEAR); if(game.spawned%4==1)bark(BK_WAVE); return;
    }
}
static void shoot(Shot *pool,uint8_t count,int16_t x,int16_t y,int8_t dx,int8_t dy,uint8_t speed) {
    uint8_t i;
    for(i=0;i<count;i++) if(!pool[i].active) {
        pool[i].x=x; pool[i].y=y; pool[i].dx=dx*speed; pool[i].dy=dy*speed;
        pool[i].active=1; pool[i].life=100; pool[i].pierce=0; return;
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
static void site(uint8_t slot,uint8_t point) {
    game.objects[slot].x=mission.points[point].x;game.objects[slot].y=mission.points[point].y;
    game.objects[slot].kind=mission.type==RESCUE?3:2;
    game.objects[slot].hp=mission.type==RESCUE?1:4+difficulty;
    if(mission_index==35) {game.objects[slot].kind=2;game.objects[slot].hp=4;}
}
static void heavy(uint8_t second) {
    Enemy *e=&game.enemies[second];
    e->kind=2;e->role=second?5:mission.boss;e->hp=36;e->cool=90;e->flash=0;
    e->x=mission.points[second].x;e->y=mission.points[second].y;
    if(mission.rule==FLEET) {
        e->x=game.x+112;if(e->x>1500)e->x=1500;e->y=game.y+80;if(e->y>1344)e->y=1344;
        e->hp=8;e->role=mission.enemy;
    }
}
void game_start(void) BANKED {
    uint8_t i,type=mission.type;
    memset(&game,0,sizeof(Game));
    game.x=mission.start.x;game.y=mission.start.y;game.hull=MAX_HULL;game.rng=0x6a31+mission_index*139;
    game.spawn_cool=90;game.invuln=90;rewinds=0;
    if(type==DEFENSE || type==ESCORT || mission.rule==VIRUS) {
        game.objects[0].x=type==ESCORT?game.x:mission.points[0].x;
        game.objects[0].y=type==ESCORT?game.y:mission.points[0].y;
        game.objects[0].hp=12;game.objects[0].kind=type==DEFENSE?1:type==ESCORT?4:2;
        if(type==DEFENSE) {game.x=game.objects[0].x;game.y=game.objects[0].y+32;}
    } else if(mission.rule==PILOT_RESCUE) {
        site(0,0);game.objects[0].kind=3;game.objects[0].hp=12;
    } else if(type==RAID || type==SABOTAGE || type==RESCUE) {
        for(i=0;i<3 && i<mission.goal;i++)site(i,i);
        game.site_next=i;
    } else if(type==EVADE || mission.rule==ESCAPE || mission.rule==UPLOAD_RACE) {
        game.objects[0].x=mission.points[0].x;game.objects[0].y=mission.points[0].y;
        game.objects[0].kind=6;game.objects[0].hp=1;
    } else if(type==BOSS || mission.rule==FLEET)heavy(0);
    if(mission.rule==DUPLICATE) {game.echo_x=game.x+64;game.echo_y=game.y+48;}
    if(mission_index==6 || mission_index==37) {
        Enemy *e=&game.enemies[5];e->x=game.x+80;e->y=game.y+64;e->hp=255;e->kind=2;e->role=5;e->cool=90;
    }
    game.camera_x=game.x>80?game.x-80:0;game.camera_y=game.y>CAMERA_BOTTOM?game.y-56:0;
    bark_reset();screen=PLAYING;game_checkpoint();radio(mission.hint,3);
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
    if(mission.type==SABOTAGE && held&J_B) {
        for(i=0;i<3;i++) if(game.objects[i].hp && near(game.x,game.y,game.objects[i].x,game.objects[i].y,18)) {
            channeling=1;
            if(mission.rule==VIRUS && game.upload_ticks) {channeling=0;break;}
            if(++game.channel>=90) {
                if(mission.rule==VIRUS) {game.upload_ticks=game.ticks;game.channel=0;radio("RULE ZERO UPLOADING",2);break;}
                game.objects[i].hp=0; game.progress++; game.score+=250; game.channel=0;
                audio_sfx(S_PICKUP); radio("SITE DISABLED",2);
            }
            break;
        }
    }
    if(!channeling) game.channel=0;
    if(mission.rule==TALK && (pressed&J_B) && near(game.x,game.y,game.enemies[0].x,game.enemies[0].y,72)) {
        channeling=0;
        if(game.ticks-game.upload_ticks>=360 && game.enemies[0].hp) {
            game.enemies[0].hp-=12;game.upload_ticks=game.ticks;
            if(!game.enemies[0].hp) {game.progress=1;game_finish();}
        }
    }
    if((pressed&J_B) && !channeling && !game.boost_cool) {
        game.boost=8; game.boost_cool=90; game.move_x=dx; game.move_y=dy;
        if(!dx && !dy) { game.move_x=vx[game.face]; game.move_y=vy[game.face]; }
        audio_sfx(S_CONFIRM);
    }
    if(channeling) { dx=0; dy=0; }
    else if(game.boost) { dx=game.move_x*3; dy=game.move_y*3; }
    // Diagonal normal flight alternates axes every other frame to bound speed.
    if(!game.boost && dx && dy && (game.frame&1)) { if(game.frame&2) dx=0; else dy=0; }
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
    if((held&J_A) && !game.fire && !channeling && mission.rule!=TALK && !(mission_index==22 && (game.ticks/300u)%3==2)) {
        shoot(game.shots,SHOT_COUNT,game.x+vx[game.face]*8,game.y+vy[game.face]*8,vx[game.face],vy[game.face],3);
        if(game.weapon==1) {
            shoot(game.shots,SHOT_COUNT,game.x,game.y,vx[(game.face+1)&7],vy[(game.face+1)&7],3);
            shoot(game.shots,SHOT_COUNT,game.x,game.y,vx[(game.face+7)&7],vy[(game.face+7)&7],3);
        }
        if(game.weapon==2) for(i=0;i<SHOT_COUNT;i++)if(game.shots[i].active)game.shots[i].pierce=1;
        game.fire=game.weapon==2?16:9; audio_sfx(S_FIRE);
    }
}
static void enemies_tick(void) {
    uint8_t i,type=mission.type,phase,direction;
    int16_t tx,ty,nx,ny;
    Enemy *e;
    if(game.spawn_cool) game.spawn_cool--;
    if(!game.spawn_cool && type!=BOSS && mission.rule!=PEACEFUL && mission_index!=8) {
        if(type==DEFENSE) {if(game.spawned<mission.goal)spawn_enemy();}
        else if(mission.rule==PILOT_RESCUE) {if(game.spawned<4 && near(game.x,game.y,game.objects[0].x,game.objects[0].y,144))spawn_enemy();}
        else spawn_enemy();
        game.spawn_cool=140-difficulty*20;
    }
    for(i=0;i<ENEMY_COUNT;i++) {
        e=&game.enemies[i]; if(!e->hp) continue;
        if(e->kind!=2 && type!=DEFENSE && type!=ESCORT && (distance(e->x-game.x)>272 || distance(e->y-game.y)>240)) { e->hp=0; continue; }
        if(e->flash) e->flash--;
        tx=game.x; ty=game.y;
        if(type==DEFENSE || type==ESCORT || (mission.rule==VIRUS && game.upload_ticks)) { tx=game.objects[0].x; ty=game.objects[0].y; }
        if(e->kind==2) {
            if((game.frame&1)==0) {
                if(mission_index==6 || mission_index==37) {if(game.frame%3==0){e->x+=sign(game.x-e->x);e->y+=sign(game.y-e->y);}}
                else if(mission.rule==FLEET) {if(e->x<1480)e->x++;if(e->y<1232)e->y++;}
                else {
                    e->x+=((game.ticks/112)&1)?-1:1;
                    if(e->x<1360) e->x=1360; if(e->x>1552) e->x=1552;
                }
            }
        } else if((game.frame+i)%3==0) {
            nx=e->x+sign(tx-e->x); ny=e->y+sign(ty-e->y);
            if(e->role==15 && game.boost) {nx=e->x+game.move_x*2;ny=e->y+game.move_y*2;}
            if(e->role==6 && (game.frame/60)&1)nx=e->x+sign(e->y-game.y);
            if(e->role==14 && (game.frame&1)) {nx=e->x;ny=e->y;}
            if(!map_solid(nx,e->y)) e->x=nx;
            if(!map_solid(e->x,ny)) e->y=ny;
        }
        if(e->cool) e->cool--;
        if(!e->cool && distance(e->x-tx)<176 && distance(e->y-ty)<144) {
            if(e->role==14 || e->role==15)bark(BK_ATTACK);
            if(e->kind==2 && type==BOSS) {
                phase=e->hp>24?0:(e->hp>12?1:2);
                if(game.phase!=phase) {
                    game.phase=phase;
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
            } else { aim_shot(e->x,e->y,tx,ty); e->cool=150-difficulty*20; }
        }
        if(near(e->x,e->y,game.x,game.y,11)) damage_player();
    }
}
static void shots_tick(void) {
    uint8_t i,j,type=mission.type;
    Shot *s;
    for(i=0;i<SHOT_COUNT;i++) {
        s=&game.shots[i]; if(!s->active) continue;
        if(game.weapon==3) {
            for(j=0;j<ENEMY_COUNT;j++)if(game.enemies[j].hp && near(s->x,s->y,game.enemies[j].x,game.enemies[j].y,80)) {
                s->dx=sign(game.enemies[j].x-s->x)*3;s->dy=sign(game.enemies[j].y-s->y)*3;break;
            }
        }
        s->x+=s->dx; s->y+=s->dy;
        if(!s->life-- || map_solid(s->x,s->y)) { s->active=0; continue; }
        for(j=0;j<ENEMY_COUNT;j++) if(game.enemies[j].hp && near(s->x,s->y,game.enemies[j].x,game.enemies[j].y,9)) {
            if((mission.rule==DUAL_BOSS && j==1 && game.enemies[0].hp) || (mission.rule==TALK && j==0) || ((mission_index==6 || mission_index==37) && j==5)) {s->active=0;break;}
            game.enemies[j].hp--; game.enemies[j].flash=5; if(!s->pierce)s->active=0;
            if(game.enemies[j].role==5 && game.enemies[j].hp==12)bark(BK_ENEMY_HEAVY);
            if(!game.enemies[j].hp) destroy_enemy(j); else audio_sfx(S_HIT);
            break;
        }
        if(s->active && (type==RAID || (type==RESCUE && mission.rule!=PILOT_RESCUE))) for(j=0;j<3;j++) if(game.objects[j].hp && game.objects[j].kind==2 && near(s->x,s->y,game.objects[j].x,game.objects[j].y,10)) {
            s->active=0; game.objects[j].hp--; audio_sfx(S_HIT);
            if(!game.objects[j].hp) {
                effect(s->x,s->y);
                if(type==RESCUE) {game.objects[j].hp=1;game.objects[j].kind=3;}
                else {
                    game.progress++; game.score+=250; radio("TARGET CLEARED",2);
                    if((mission_index==4 || mission_index==7 || mission_index==10 || mission_index==27) && game.progress>=mission.goal) {
                        total_cores++;game.core_count++;bark(BK_CORE);
                    }
                    if(game.site_next<mission.goal && game.site_next<WAYPOINTS)site(j,game.site_next++);
                }
            }
            break;
        }
    }
    for(i=0;i<HOSTILE_COUNT;i++) {
        if(game.brake && (game.frame&1))continue;
        s=&game.hostile[i]; if(!s->active) continue;
        s->x+=s->dx; s->y+=s->dy;
        if(!s->life-- || map_solid(s->x,s->y)) { s->active=0; continue; }
        if(near(s->x,s->y,game.x,game.y,6)) {
            s->active=0;damage_player();
            if(type==BOSS && game.enemies[0].role==5)bark(BK_ENEMY_HIT);
        } else if(near(s->x,s->y,game.x,game.y,12))bark(BK_NEAR);
        if(s->active && (type==DEFENSE || type==ESCORT || (mission.rule==VIRUS && game.upload_ticks)) && near(s->x,s->y,game.objects[0].x,game.objects[0].y,9)) {
            s->active=0;
            if(game.objects[0].hp) game.objects[0].hp--; game.target_damage++;
            effect(game.objects[0].x,game.objects[0].y);
            if(!game.objects[0].hp) game_fail(type==ESCORT?"TRANSPORT LOST":"PROTECTED SITE LOST");
        }
    }
}
static void objectives_tick(void) {
    uint8_t i,type=mission.type,checkpoint_ready=0;Objective *o;
    if(type==ESCORT) {
        o=&game.objects[0];
        if(++game.convoy_clock>=2 && game.convoy_step<mission.goal && near(game.x,game.y,o->x,o->y,112)) {
            game.convoy_clock=0;
            o->x+=sign(mission.points[game.convoy_step].x-o->x);o->y+=sign(mission.points[game.convoy_step].y-o->y);
            if(o->x==mission.points[game.convoy_step].x && o->y==mission.points[game.convoy_step].y) {game.convoy_step++;game.progress++;}
        }
    }
    if(type==RESCUE) for(i=0;i<3;i++) {
        o=&game.objects[i];
        if(o->hp && o->kind==3 && (mission.rule!=PILOT_RESCUE || game.kills>=4) && near(game.x,game.y,o->x,o->y,16)) {
            o->hp=0;game.progress++;game.score+=200;audio_sfx(S_PICKUP);bark(BK_DONE);
        }
    }
    if((type==EVADE && mission_index!=8) || mission.rule==ESCAPE || mission.rule==UPLOAD_RACE) {
        o=&game.objects[0];
        if(game.progress<mission.goal && near(game.x,game.y,o->x,o->y,16)) {
            game.progress++;audio_sfx(S_PICKUP);radio("WAYPOINT CONFIRMED",2);
            if(mission_index==21 && game.progress==3) {game.echo_x=game.x;game.echo_y=game.y;effect(game.x,game.y);damage_player();}
            if(game.progress<mission.goal) {o->x=mission.points[game.progress].x;o->y=mission.points[game.progress].y;}
        }
    }
    if(mission.rule==VIRUS && game.upload_ticks) {
        if(game.ticks-game.upload_ticks>=5400)game.progress=1;
    }
    if(mission.rule==FLEET && !game.enemies[0].hp && game.progress<mission.goal)heavy(0);
    if(mission.rule==DUAL_BOSS && game.enemies[0].hp<=24 && !game.boss_stage) {game.boss_stage=1;heavy(1);}
    if(mission.rule==UPLOAD_RACE) {
        game.echo_x=game.objects[0].x;game.echo_y=game.objects[0].y-48;
    }
    if(mission.rule==DUPLICATE) {
        if((game.frame&3)==0) {game.echo_x+=sign(game.x-game.echo_x);game.echo_y+=sign(game.y-game.echo_y);}
        if(near(game.x,game.y,game.echo_x,game.echo_y,12))damage_player();
    }
    if(mission.rule==RADAR && game.ticks%720u>=600u) {
        if(!map_solid(game.x-16,game.y) && !map_solid(game.x+16,game.y) && !map_solid(game.x,game.y-16) && !map_solid(game.x,game.y+16)) {
            if(game.exposure<120)game.exposure++;
            if(game.exposure==90)game_fail("RADAR DETECTED YOU");
        } else game.exposure=0;
    } else if(mission.rule==RADAR)game.exposure=0;
    if(mission_index==53 && (distance(game.x-800)>104 || distance(game.y-800)>96 || near(game.x,game.y,800,800,20)))game.perimeter_left=1;
    o=&game.objects[3];
    if(o->hp && o->kind==5 && near(game.x,game.y,o->x,o->y,16)) {
        o->hp=0;
        switch(game.pickup) {
            case 0:game.hull+=2;if(game.hull>MAX_HULL)game.hull=MAX_HULL;break;
            case 1:case 2:case 3:game.weapon=game.pickup;game.weapon_time=30;break;
            case 4:game.shield=10;break;
            case 5:game.brake=8;break;
            case 6:game.extra++;break;
            case 7:total_cores++;game.core_count++;break;
        }
        if(total_cores>=100)achievements|=1ul<<13;
        bark(game.pickup==7?BK_CORE:BK_PICKUP);audio_sfx(S_PICKUP);
    }
    if(screen!=PLAYING)return;
    if((type==DEFENSE && game.kills>=mission.goal) ||
       ((type==RAID || type==SABOTAGE || type==BOSS || type==CHASE || type==ESCORT) && game.progress>=mission.goal) ||
       (type==EVADE && mission_index!=8 && game.progress>=mission.goal) ||
       (mission.rule==PILOT_RESCUE && game.progress>=1) ||
       (type==RESCUE && mission.rule!=PILOT_RESCUE && game.progress>=mission.goal && near(game.x,game.y,80,112,18))) {game_finish();return;}
    if(game.ticks>=mission.seconds*60u) {
        if(type==SURVIVAL || mission_index==8)game_finish();else game_fail("TIME EXPIRED");return;
    }
    if(mission.rule==DUAL_BOSS && game.progress==1 && game.checkpoint_mark<2) {
        game.checkpoint_mark=2;game_checkpoint();
    }
    if(game.checkpoint_mark || !game.hull)return;
    if(type==DEFENSE)checkpoint_ready=game.kills>=mission.goal/2;
    else if(type==BOSS)checkpoint_ready=game.enemies[0].hp<=18;
    else if(type==SURVIVAL || mission_index==8)checkpoint_ready=game.ticks>=mission.seconds*30u;
    else checkpoint_ready=game.progress>=((mission.goal+1)/2);
    if(checkpoint_ready) {game.checkpoint_mark=1;game_checkpoint();}
}
void game_target(int16_t *x,int16_t *y) BANKED {
    uint8_t i,type=mission.type;uint16_t best=65535,delta;
    *x=game.x;*y=game.y;
    if(type==DEFENSE || type==ESCORT || type==EVADE || mission.rule==ESCAPE || mission.rule==UPLOAD_RACE || mission.rule==VIRUS || mission.rule==PILOT_RESCUE) {
        *x=game.objects[0].x;*y=game.objects[0].y;return;
    }
    if(type==BOSS || mission.rule==FLEET) {i=(mission.rule==DUAL_BOSS && !game.enemies[0].hp)?1:0;*x=game.enemies[i].x;*y=game.enemies[i].y;return;}
    if(type==RESCUE && game.progress>=mission.goal) {*x=80;*y=112;return;}
    if(type==RAID || type==RESCUE || type==SABOTAGE) for(i=0;i<3;i++)if(game.objects[i].hp) {
        delta=distance(game.x-game.objects[i].x)+distance(game.y-game.objects[i].y);
        if(delta<best) {best=delta;*x=game.objects[i].x;*y=game.objects[i].y;}
    }
}
void game_tick(uint8_t held,uint8_t pressed,uint8_t elapsed) BANKED {
    uint8_t i;uint16_t before=game.ticks;
    if(game.finishing) {story_poll();return;}
    game.ticks+=elapsed;game.frame++;
    if(before/60u!=game.ticks/60u) {
        if(game.weapon_time && !--game.weapon_time)game.weapon=0;
        if(game.shield)game.shield--;if(game.brake)game.brake--;
    }
    bark_tick(elapsed);
    for(i=0;i<EFFECT_COUNT;i++)if(game.effects[i].life)game.effects[i].life--;
    player_tick(held,pressed);
    if(!game.brake || !(game.frame&1))enemies_tick();
    if(screen!=PLAYING)return;
    shots_tick();
    if(screen==PLAYING)objectives_tick();
    if(screen==PLAYING)story_poll();
}
