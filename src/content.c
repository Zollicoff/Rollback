#include "game.h"
#include <string.h>
Mission mission;
StoryData story;
uint8_t story_bank;
Page page;
Scene scene;
uint8_t scene_page,scene_return,replay,loop_two,main_complete,difficulty=1;
uint16_t total_kills,total_rewinds,total_cores;
uint32_t achievements,seen_enemies;
char radio_line[21],failure_reason[21];
uint8_t radio_timer;

void bank_read(void *dest,const void *source,uint16_t length,uint8_t bank) NONBANKED {
    uint8_t previous=CURRENT_BANK;
    SWITCH_ROM(bank); memcpy(dest,source,length); SWITCH_ROM(previous);
}
void bank_text(char *dest,const char *source,uint8_t limit,uint8_t bank) NONBANKED {
    uint8_t previous=CURRENT_BANK,i=0;
    SWITCH_ROM(bank);
    while(i+1<limit && source[i]) { dest[i]=source[i]; i++; }
    dest[i]=0; SWITCH_ROM(previous);
}
void mission_load(void) NONBANKED {
    uint8_t index=mission_index;
    if(loop_two && index>=52) index+=2;
    story_bank=campaign[index].bank;
    bank_read(&story,campaign[index].data,sizeof(story),story_bank);
    bank_text(mission.title,story.title,sizeof(mission.title),story_bank);
    bank_text(mission.objective,story.objective,sizeof(mission.objective),story_bank);
    bank_text(mission.hint,story.hint,sizeof(mission.hint),story_bank);
    bank_text(mission.setting,story.setting,sizeof(mission.setting),story_bank);
    mission.type=story.type;mission.layout=story.layout;mission.episode=story.episode;
    mission.rule=story.rule;mission.goal=story.goal;mission.enemy=story.enemy;mission.boss=story.boss;
    mission.seconds=story.seconds;mission.start=story.start;
    memcpy(mission.points,story.points,sizeof(mission.points));
    replay=main_complete || mission_index+1<unlocked;
}
void story_page(void) NONBANKED {
    StoryPage p;
    bank_read(&p,story.pages+scene.first+scene_page,sizeof(p),story_bank);
    page.speaker=p.speaker;page.mood=p.mood;page.flags=p.flags;
    bank_text(page.text,p.text,sizeof(page.text),story_bank);
}
void story_open(Scene group,uint8_t destination) NONBANKED {
    scene=group;scene_return=destination;scene_page=0;
    if(scene.count) story_page();
}
uint8_t story_next(void) NONBANKED {
    if(scene_page+1>=scene.count) return 0;
    scene_page++;story_page();return 1;
}
void archive_load(uint8_t index,uint8_t part) NONBANKED {
    StoryPage p;
    if(index>=archive_count || part>=archive[index].count)return;
    bank_read(&p,archive[index].pages+part,sizeof(p),archive[index].bank);
    page.speaker=p.speaker;page.mood=p.mood;page.flags=p.flags;
    bank_text(page.text,p.text,sizeof(page.text),archive[index].bank);
}
uint32_t progress_code(void) NONBANKED {
    uint16_t payload=unlocked+64u*main_complete+128u*loop_two+256u*difficulty;
    return 100000ul+(uint32_t)payload*97u+(payload*13u+7u)%97u;
}
uint8_t progress_decode(uint32_t code) NONBANKED {
    uint16_t payload;uint8_t count;
    if(code<100000ul || code>=200000ul)return 0;
    code-=100000ul;payload=code/97u;count=payload&63;
    if(!count || count>54 || payload>=1024 || code%97u!=(payload*13u+7u)%97u)return 0;
    if((payload&128) && !(payload&64))return 0;
    unlocked=count;main_complete=(payload>>6)&1;loop_two=(payload>>7)&1;difficulty=payload>>8;
    return 1;
}
void radio(const char *line,uint8_t priority) NONBANKED {
    uint8_t i=0;
    (void)priority;
    while(i<20 && line[i]) { radio_line[i]=line[i];i++; }
    while(i<20)radio_line[i++]=' ';
    radio_line[20]=0;radio_timer=150;
}
void game_fail(const char *reason) NONBANKED {
    uint8_t i=0;
    while(i<20 && reason[i]) {failure_reason[i]=reason[i];i++;}
    failure_reason[i]=0;screen=FALLEN;audio_sfx(S_BOOM);
}
