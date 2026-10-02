#pragma bank 255
#include "game.h"

static uint16_t dist(int16_t x) {return x<0?-x:x;}
void story_poll(void) BANKED {
    uint8_t i,ready;StoryEvent event;int16_t x,y;
    for(i=0;i<story.event_count;i++) {
        if(game.events&(1u<<i))continue;
        bank_read(&event,story.events+i,sizeof(event),story_bank);ready=0;
        switch(event.condition) {
            case C_START:ready=1;break;
            case C_TICKS:ready=game.ticks>=event.value || game.finishing;break;
            case C_PROGRESS:ready=game.progress>=event.value;break;
            case C_KILLS:ready=game.kills>=event.value;break;
            case C_BOSS:ready=game.enemies[0].hp<=event.value || game.progress;break;
            case C_DONE:ready=game.finishing;break;
            case C_HIT:ready=game.target_damage;break;
            case C_CHANNEL:ready=game.upload_ticks!=0;break;
            case C_CONTACT:
                game_target(&x,&y);ready=dist(x-game.x)<128 && dist(y-game.y)<112;break;
            case C_EXPOSURE:ready=game.exposure>=event.value;break;
        }
        if(ready) {
            game.events|=1u<<i;
            story_open(event.scene,PLAYING);screen=RADIO;return;
        }
    }
    if(game.finishing)campaign_complete();
}
void campaign_complete(void) BANKED {
    uint8_t n=mission_index+1;
    if(n>=unlocked && unlocked<MISSION_COUNT)unlocked=n+1;
    if(!rewinds)achievements|=1ul<<6;
    if(n==1)achievements|=1ul;
    if(n==12)achievements|=1ul<<1;
    if(n==13)achievements|=1ul<<2;
    if(n==23)achievements|=1ul<<7;
    if(n==26 && game.ticks<270u*60u)achievements|=1ul<<8;
    if(n==30)achievements|=1ul<<9;
    if(n==39 && !game.target_damage)achievements|=1ul<<10;
    if(n==40)achievements|=1ul<<11;
    if(n==47)achievements|=1ul<<12;
    if(n==53)achievements|=1ul<<14;
    if(n==54) {
        main_complete=1;achievements|=1ul<<16;
        if(!game.perimeter_left)achievements|=1ul<<15;
    }
    screen=DEBRIEF;brief_page=0;audio_sfx(S_WIN);
}
