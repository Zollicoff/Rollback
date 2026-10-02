#include "game.h"

uint8_t screen=TITLE,mission_index=0,unlocked=1,rewinds=0,brief_page=0;
static uint8_t title_choice,pause_choice,code_cursor,code_invalid,settings_choice,settings_return;
static uint8_t code_digits[6]={1,0,0,0,0,0},archive_index=5,archive_part,ending_step,episode_card,result_ready,radio_ready;
static void start_brief(void) {
    mission_load();story_open(story.briefing,PLAYING);screen=BRIEFING;
}
static void brief_or_card(void) {
    mission_load();
    if(!replay && (mission_index==0 || mission_index==12 || mission_index==26 || mission_index==40)) {
        episode_card=loop_two && !mission_index?4:mission.episode-1;
        scene_page=0;scene.count=archive[episode_card].count;archive_load(episode_card,0);screen=EPISODE;
    } else if(loop_two && !mission_index) {
        episode_card=4;scene_page=0;scene.count=archive[4].count;archive_load(4,0);screen=EPISODE;
    } else start_brief();
}
static void next_mission(void) {
    if(mission_index==53) {screen=ENDING;ending_step=0;brief_page=0;}
    else {mission_index++;brief_or_card();}
}
static void redraw(void) {
    switch(screen) {
        case TITLE:draw_title(title_choice);break;
        case SELECT_MISSION:mission_load();draw_selection();break;
        case BRIEFING:case RADIO:case EPISODE:draw_dialog();break;
        case PLAYING:draw_arena();break;
        case PAUSED:draw_pause(pause_choice);break;
        case FALLEN:draw_fallen();break;
        case DEBRIEF:draw_debrief();break;
        case MANUAL:draw_manual();break;
        case PASSWORD:draw_password(code_digits,code_cursor,code_invalid);break;
        case SETTINGS:draw_settings(settings_choice);break;
        case ARCHIVE:draw_archive(archive_index,archive_part);break;
        case ENDING:draw_ending(ending_step);break;
    }
}
void main(void) {
    uint8_t held,pressed,previous=0,old_screen,changed,elapsed;
    uint16_t last_frame,current_frame;uint32_t code;
    if(_cpu==CGB_TYPE)cpu_fast();
    mission_load();video_init();audio_init();redraw();last_frame=sys_time;
    while(1) {
        vsync();current_frame=sys_time;elapsed=(uint8_t)(current_frame-last_frame);last_frame=current_frame;
        held=joypad();pressed=held&~previous;previous=held;old_screen=screen;changed=0;
        audio_tick();
        if((pressed&J_SELECT) && screen!=FALLEN)audio_toggle();
        switch(screen) {
            case TITLE:
                if(pressed&J_UP){title_choice=(title_choice+4)%5;changed=1;}
                if(pressed&J_DOWN){title_choice=(title_choice+1)%5;changed=1;}
                if(pressed&(J_A|J_START)) {
                    if(title_choice==0)screen=SELECT_MISSION;
                    else if(title_choice==1){screen=PASSWORD;code_invalid=0;}
                    else if(title_choice==2){screen=MANUAL;brief_page=0;}
                    else if(title_choice==3){screen=SETTINGS;settings_return=TITLE;}
                    else screen=ARCHIVE;
                }
                break;
            case SELECT_MISSION:
                if(pressed&(J_LEFT|J_UP)){mission_index=(mission_index?mission_index:unlocked)-1;changed=1;}
                if(pressed&(J_RIGHT|J_DOWN)){mission_index=(mission_index+1)%unlocked;changed=1;}
                if(pressed&J_A)brief_or_card();
                if(pressed&J_B)screen=TITLE;
                break;
            case EPISODE:
                if(pressed&J_A){if(++scene_page<scene.count){archive_load(episode_card,scene_page);changed=1;}else start_brief();}
                break;
            case BRIEFING:
                if(pressed&J_A) {
                    if(story_next())changed=1;
                    else if(loop_two && mission_index==53) {achievements|=1ul<<17;ending_step=0;screen=ENDING;}
                    else game_start();
                }
                if(pressed&J_B && scene_page){scene_page--;story_page();changed=1;}
                break;
            case RADIO:
                if(!radio_ready){if(!(held&J_A))radio_ready=1;break;}
                if(pressed&J_A){if(story_next())changed=1;else screen=PLAYING;}
                break;
            case PLAYING:
                if(pressed&J_START){screen=PAUSED;pause_choice=0;}
                else {game_tick(held,pressed,elapsed);if(screen==PLAYING)draw_game();}
                break;
            case PAUSED:
                if(pressed&J_UP){pause_choice=(pause_choice+3)%4;changed=1;}
                if(pressed&J_DOWN){pause_choice=(pause_choice+1)%4;changed=1;}
                if(pressed&(J_B|J_START))screen=PLAYING;
                if(pressed&J_A) {
                    if(pause_choice==0)screen=PLAYING;
                    else if(pause_choice==1)game_rewind();
                    else if(pause_choice==2){settings_return=PAUSED;screen=SETTINGS;}
                    else screen=SELECT_MISSION;
                }
                break;
            case FALLEN:
                if(!held && result_ready<15)result_ready++;
                if(result_ready<15)break;
                if(pressed&J_START)game_rewind();
                else if(pressed&J_B)game_start();
                else if(pressed&J_SELECT)screen=SELECT_MISSION;
                break;
            case DEBRIEF:
                if(pressed&J_START) {
                    if(!brief_page && story.debrief.count){brief_page=1;story_open(story.debrief,BRIEFING);changed=1;}
                    else if(brief_page && story_next())changed=1;
                    else next_mission();
                }
                break;
            case MANUAL:
                if(pressed&J_A){brief_page=(brief_page+1)%4;changed=1;}
                if(pressed&(J_B|J_START))screen=TITLE;
                break;
            case PASSWORD:
                if(pressed&J_LEFT){code_cursor=(code_cursor+5)%6;changed=1;}
                if(pressed&J_RIGHT){code_cursor=(code_cursor+1)%6;changed=1;}
                if(pressed&J_UP){code_digits[code_cursor]=(code_digits[code_cursor]+1)%10;changed=1;}
                if(pressed&J_DOWN){code_digits[code_cursor]=(code_digits[code_cursor]+9)%10;changed=1;}
                if(pressed&J_A) {
                    code=(uint32_t)code_digits[0]*100000ul+(uint32_t)code_digits[1]*10000ul+(uint32_t)code_digits[2]*1000ul+code_digits[3]*100u+code_digits[4]*10u+code_digits[5];
                    if(progress_decode(code)){mission_index=unlocked-1;screen=SELECT_MISSION;}else {code_invalid=1;changed=1;}
                }
                if(pressed&J_B)screen=TITLE;
                break;
            case SETTINGS:
                if(pressed&(J_UP|J_DOWN)){settings_choice^=1;changed=1;}
                if(pressed&(J_LEFT|J_RIGHT|J_A)) {
                    if(settings_choice)audio_toggle();else difficulty=(difficulty+((pressed&J_LEFT)?3:1))&3;
                    changed=1;
                }
                if(pressed&J_B)screen=settings_return;
                break;
            case ARCHIVE:
                if(pressed&J_LEFT){archive_index=archive_index>5?archive_index-1:archive_count-1;archive_part=0;changed=1;}
                if(pressed&J_RIGHT){archive_index=archive_index+1<archive_count?archive_index+1:5;archive_part=0;changed=1;}
                if(pressed&J_A){archive_part=(archive_part+1)%archive[archive_index].count;changed=1;}
                if(pressed&J_B)screen=TITLE;
                break;
            case ENDING:
                if(pressed&J_START) {
                    if(ending_step==2 && brief_page+1<archive[archive_count-1].count)brief_page++;
                    else if(ending_step<4){ending_step++;brief_page=0;}
                    else {loop_two=1;unlocked=1;mission_index=0;brief_or_card();}
                    changed=1;
                }
                if(pressed&J_B)screen=SELECT_MISSION;
                break;
        }
        if(old_screen!=screen && screen==FALLEN)result_ready=0;
        if(old_screen!=screen && screen==RADIO)radio_ready=0;
        if(old_screen!=screen || changed){redraw();audio_sfx(S_CONFIRM);last_frame=sys_time;}
    }
}
