#pragma bank 255
#include "game.h"
#include <string.h>
extern uint32_t seen_enemies;
void tile(uint8_t x,uint8_t y,uint8_t n,uint8_t p) NONBANKED;
static const char *const ranks[4]={"CADET","PILOT","ACE","AUDITOR"};
static void code_at(uint8_t y) {
    uint32_t code=progress_code();char text[7];uint8_t i=6;
    text[6]=0;while(i){text[--i]='0'+code%10u;code/=10u;}
    text_at(1,y,"RESUME",6);text_at(12,y,text,3);
}
static void logo(uint8_t y) {
    uint8_t i,x,j;
    for(i=0;i<8;i++)for(j=0;j<2;j++)for(x=0;x<2;x++)tile(2+i*2+x,y+j,128+i*4+j*2+x,i<4?0:3);
}
void draw_title(uint8_t choice) BANKED {
    uint8_t i;static const char *const choices[]={"CAMPAIGN","RESUME CODE","FIELD MANUAL","SETTINGS","ARCHIVE"};
    page_begin();logo(2);
    text_at(2,5,"ONE TIME MACHINE.",6);text_at(2,6,"ZERO GOOD FIXES.",6);
    for(i=0;i<5;i++)text_at(4,8+i*2,choices[i],choice==i?3:0);
    text_at(2,8+choice*2,">",3);text_at(1,17,"START TO REWIND",7);page_end();
}
void draw_selection(void) BANKED {
    page_begin();frame(0,0,20,18,6);
    text_at(1,1,loop_two?"LOOP 2":"CAMPAIGN",7);
    text_at(1,3,"MISSION",6);number_at(9,3,mission_index+1,2,3);text_at(12,3,"/ 54",6);
    wrapped(1,5,mission.title,18,3,0);
    text_at(1,9,mode_names[mission.type],7);text_at(11,9,ranks[difficulty],3);
    wrapped(1,11,mission.setting,18,2,6);
    code_at(14);text_at(1,16,"A BRIEFING  B BACK",7);page_end();
}
void draw_brief(void) BANKED {draw_dialog();}
void draw_dialog(void) BANKED {
    uint8_t top=7;
    page_begin();frame(0,0,20,18,6);
    text_at(1,1,screen==RADIO?"SQUAD RADIO":screen==DEBRIEF?"DEBRIEF":"BRIEFING",7);
    number_at(16,1,mission_index+1,2,3);
    if(screen==RADIO || !page.speaker) {
        text_at(1,3,page.flags&1?"STAGE DIRECTION":speakers[page.speaker],3);top=5;
    } else {
        portrait_at(1,2,page.speaker,page.mood);
        text_at(6,3,speakers[page.speaker],3);text_at(6,5,moods[page.mood],6);
    }
    wrapped(1,top,page.text,18,8,(page.flags&2) && replay?3:0);
    if((page.flags&2) && replay)text_at(16,15,"*",3);
    number_at(1,15,scene_page+1,2,6);text_at(3,15,"/",6);number_at(4,15,scene.count,2,6);
    text_at(8,16,screen==DEBRIEF?"START NEXT":"A CONTINUE",7);page_end();
}
void draw_pause(uint8_t choice) BANKED {
    uint8_t i;static const char *const options[]={"RESUME","TAKE A MULLIGAN","SETTINGS","ABANDON TIMELINE"};
    page_begin();frame(0,0,20,18,7);text_at(1,2,"TIMELINE PAUSED",7);
    for(i=0;i<4;i++)text_at(3,5+i*2,options[i],choice==i?3:0);
    text_at(1,5+choice*2,">",3);code_at(14);text_at(1,16,"A PICK  B RESUME",6);page_end();
}
void draw_fallen(void) BANKED {
    page_begin();frame(0,0,20,18,4);
    text_at(1,2,"TIMELINE",4);text_at(1,3,"COMPROMISED",4);wrapped(1,5,failure_reason,18,2,0);
    text_at(1,9,"TAKE A MULLIGAN",7);text_at(1,10,"START: CHECKPOINT",6);
    text_at(1,13,"B RESTART MISSION",0);text_at(1,15,"SELECT TO KESTREL",6);page_end();
}
void draw_debrief(void) BANKED {
    uint8_t rating=rewinds>=4?4:rewinds>=2?3:game.hits>=5?2:game.hits?1:0;
    static const char *const damage[]={"NEGLIGIBLE","MODERATE","MODERATE TO SEVERE","CATASTROPHIC","MAE IS PAINTING"};
    if(brief_page) {draw_dialog();return;}
    page_begin();frame(0,0,20,18,7);text_at(1,1,"MISSION COMPLETE",7);
    text_at(1,4,"TIMELINE DAMAGE",6);text_at(1,6,damage[rating],3);
    text_at(1,8,"SCORE",6);number_at(12,8,game.score,5,0);
    text_at(1,10,"MULLIGANS",6);number_at(14,10,rewinds,3,0);
    code_at(12);text_at(1,15,"START DEBRIEF",7);page_end();
}
void draw_manual(void) BANKED {
    page_begin();text_at(1,1,"FIELD MANUAL",7);number_at(16,1,brief_page+1,1,3);
    if(!brief_page) {
        text_at(1,3,"D PAD FLY / AIM",0);text_at(1,5,"A FIRE / HOLD AIM",0);text_at(1,7,"B BOOST / INTERACT",0);
        text_at(1,9,"START PAUSE",0);text_at(1,11,"SELECT SOUND",0);text_at(1,13,"RADIO PAUSES FLIGHT",6);
    } else if(brief_page==1) {
        text_at(1,3,"RED: ATTACK",4);text_at(1,5,"GREEN: PROTECT",5);text_at(1,7,"CYAN: RESCUE / GO",7);
        text_at(1,9,"AMBER: HOLD B",3);wrapped(1,11,"ESCORTS WAIT IF YOU FALL BEHIND. GUARD THEM AT EACH TURN.",18,4,0);
    } else if(brief_page==2) {
        wrapped(1,3,"CHECKPOINTS RESTORE THE WORLD, ENEMIES, TIMER AND STORY. START REWINDS AFTER DEFEAT.",18,6,0);
        wrapped(1,10,"WRITE DOWN YOUR SIX-DIGIT RESUME CODE. IT RESTORES MISSIONS, LOOP AND DIFFICULTY.",18,5,6);
    } else {
        wrapped(1,3,"RADAR SWEEPS EVERY 12 SECONDS. WAIT NEXT TO COVER WHEN THE HUD WARNS YOU.",18,6,0);
        wrapped(1,10,"ARCHIVE HOLDS ITEMS, ENEMIES AND ACHIEVEMENTS. STATS LAST FOR THIS SESSION.",18,5,6);
    }
    text_at(1,17,"A NEXT  B BACK",7);page_end();
}
void draw_password(const uint8_t *digits,uint8_t cursor,uint8_t invalid) BANKED {
    uint8_t i;page_begin();frame(0,0,20,18,6);text_at(4,2,"RESUME CODE",7);
    for(i=0;i<6;i++){number_at(2+i*3,7,digits[i],1,i==cursor?3:0);if(i==cursor)text_at(2+i*3,9,"=",3);}
    text_at(1,4,"CAMPAIGN / LOOP",6);text_at(1,11,invalid?"INVALID CODE":"ENTER SIX DIGITS",invalid?4:6);
    text_at(1,13,"UP/DOWN CHANGE",0);text_at(1,14,"LEFT/RIGHT MOVE",0);text_at(1,16,"A RESUME  B BACK",7);page_end();
}
void draw_settings(uint8_t choice) BANKED {
    page_begin();frame(0,0,20,18,6);text_at(2,2,"SETTINGS",7);
    text_at(3,6,"DIFFICULTY",0);text_at(4,8,ranks[difficulty],3);
    text_at(3,11,"SOUND",0);text_at(11,11,sound_on?"ON":"OFF",3);text_at(1,choice?11:6,">",3);
    text_at(1,14,"LEFT/RIGHT CHANGE",6);text_at(1,16,"B BACK",7);page_end();
}
void draw_archive(uint8_t index,uint8_t part) BANKED {
    uint8_t locked=archive[index].unlock>unlocked && !main_complete;
    if(index>=13 && index<30)locked=!main_complete && archive[index].unlock>=unlocked && !(seen_enemies&(1ul<<(index-13)));
    page_begin();frame(0,0,20,18,6);text_at(1,1,"ARCHIVE",7);number_at(14,1,index-4,2,3);
    if(locked)wrapped(1,5,"UNKNOWN. KEEP FLYING TO FIND THIS ENTRY.",18,5,6);
    else {archive_load(index,part);wrapped(1,4,page.text,18,8,0);}
    if(index>=45 && index<63)text_at(1,13,(achievements&(1ul<<(index-45)))?"EARNED":"NOT YET EARNED",3);
    text_at(1,15,"LEFT/RIGHT ENTRY",6);text_at(1,16,"A PAGE  B BACK",7);page_end();
}
void draw_complete(void) BANKED {draw_ending(0);}
void draw_ending(uint8_t step) BANKED {
    page_begin();
    if(step==0)wrapped(1,7,loop_two?"THE LOOP IS OPEN.":"IT HAS ALREADY HAPPENED.",18,3,0);
    else if(step==1)logo(7);
    else if(step==2) {archive_load(archive_count-1,brief_page);text_at(1,1,"CREDITS",7);wrapped(1,4,page.text,18,8,0);}
    else if(step==3) {
        if(loop_two){portrait_at(8,4,0,0);text_at(6,11,"TALLY: 0",0);}
        else {text_at(1,4,"GUS",3);wrapped(1,7,"CAPTAIN? HYPOTHETICALLY... WANT TO GO AGAIN?",18,5,0);}
    } else {text_at(1,3,"LOOP 2 UNLOCKED",7);wrapped(1,6,"YOU REMEMBER EVERYTHING. SO DO THEY.",18,4,0);code_at(12);text_at(1,15,"START NEW LOOP",7);text_at(1,17,"B MISSION SELECT",6);}
    if(step<4)text_at(1,17,"START CONTINUE",7);page_end();
}
