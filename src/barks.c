#pragma bank 255
#include "game.h"
#include <string.h>
static uint16_t cooldown[BK_COUNT],quiet_ticks;
static uint8_t history[BK_COUNT][3],active_priority,offset,part_timer;
static char chatter[180];
static uint8_t serial;
static uint8_t priority(uint8_t trigger) {
    return trigger==BK_LOW?4:trigger==BK_RETRY?3:trigger==BK_MULTI?2:1;
}
static void show_part(void) {
    uint8_t i;
    for(i=0;i<20 && chatter[offset];i++)radio_line[i]=chatter[offset++];
    while(i<20)radio_line[i++]=' ';
    radio_line[20]=0;radio_timer=150;part_timer=150;
}
void bark_reset(void) BANKED {
    uint8_t i;quiet_ticks=0;chatter[0]=0;offset=0;radio_timer=0;active_priority=0;
    for(i=0;i<BK_COUNT;i++)cooldown[i]=0;
}
void bark(uint8_t trigger) BANKED {
    uint8_t pool,i,j,id=0,chosen=255,count=0,p=priority(trigger),n=mission_index+1,eligible;
    Bark item,selected;uint8_t selected_bank=0,remember=3;uint16_t random=game.rng+game.ticks+(++serial)*37u;
    if(trigger>=BK_COUNT || cooldown[trigger] || (radio_timer && p<=active_priority) || screen==RADIO)return;
    if((trigger==BK_KILL && (random&3)) || (trigger==BK_DAMAGE && random%10>2) || (trigger==BK_NEAR && random%20>2))return;
    if(trigger==BK_RETRY)remember=(n>=27 && n<=40)?1:(n<4 || n>=41)?2:3;
    if(trigger==BK_APPEAR)remember=1;
    if(trigger==BK_ENEMY_HEAVY || trigger==BK_ATTACK || trigger==BK_DESTROY)remember=0;
    for(pool=0;pool<bark_pool_count;pool++) for(i=0;i<bark_pools[pool].count;i++,id++) {
        bank_read(&item,bark_pools[pool].data+i,sizeof(item),bark_pools[pool].bank);
        if(item.trigger!=trigger && item.trigger!=255)continue;
        if(n<item.first || n>item.last)continue;
        if((item.flags&4) && n>=39 && n<=46)continue;
        if(trigger==BK_RETRY && item.speaker!=(n<4?1:n<41?4:3))continue;
        eligible=1;for(j=0;j<remember;j++)if(history[trigger][j]==id+1)eligible=0;
        if(!eligible)continue;
        count++;random=random*109u+89u;
        if(chosen==255 || random%count==0 || (item.speaker==1 && (random&7)==0)) {
            chosen=id;selected=item;selected_bank=bark_pools[pool].bank;
        }
    }
    if(chosen==255)return;
    history[trigger][2]=history[trigger][1];history[trigger][1]=history[trigger][0];history[trigger][0]=chosen+1;
    strcpy(chatter,speakers[selected.speaker]);strcat(chatter,": ");offset=strlen(chatter);
    bank_text(chatter+offset,selected.text,sizeof(chatter)-offset,selected_bank);
    if((selected.flags&2) && n<=40) {
        /* Mae speaks through Gus while he is still aboard. */
        char *s=chatter+offset;
        if(!strncmp(s,"FELIX",5)) {s[0]='G';s[1]='U';s[2]='S';memmove(s+3,s+5,strlen(s+5)+1);}
    }
    offset=0;active_priority=p;show_part();
    cooldown[trigger]=(trigger==BK_LOW?20:trigger==BK_MULTI?15:trigger==BK_NEAR?12:trigger==BK_DAMAGE?10:trigger==BK_IDLE?30:8)*60u;
}
void bark_tick(uint8_t elapsed) BANKED {
    uint8_t i,enemy=0;
    for(i=0;i<BK_COUNT;i++)cooldown[i]=cooldown[i]>elapsed?cooldown[i]-elapsed:0;
    if(part_timer) {part_timer--;if(!part_timer && chatter[offset])show_part();}
    if(radio_timer)radio_timer--;
    for(i=0;i<ENEMY_COUNT;i++)if(game.enemies[i].hp)enemy=1;
    if(enemy)quiet_ticks=0;else if(quiet_ticks<1200)quiet_ticks+=elapsed;
    else {bark(BK_IDLE);quiet_ticks=0;}
}
