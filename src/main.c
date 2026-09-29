#include "game.h"

uint8_t screen=TITLE,mission_index=0,unlocked=1,rewinds=0,brief_page=0;
static uint8_t title_choice,pause_choice,code_cursor,code_invalid;
static uint8_t code_digits[4]={1,0,0,0};

static void redraw(void) {
    switch(screen) {
        case TITLE: draw_title(title_choice); break;
        case SELECT_MISSION: draw_selection(); break;
        case BRIEFING: draw_brief(); break;
        case PLAYING: draw_arena(); break;
        case PAUSED: draw_pause(pause_choice); break;
        case FALLEN: draw_fallen(); break;
        case DEBRIEF: draw_debrief(); break;
        case MANUAL: draw_manual(); break;
        case PASSWORD: draw_password(code_digits,code_cursor,code_invalid); break;
        case COMPLETE: draw_complete(); break;
    }
}
void main(void) {
    uint8_t held,pressed,previous=0,old_screen,changed,decoded;
    uint16_t code,last_frame,current_frame;
    uint8_t elapsed;
    if(_cpu==CGB_TYPE) cpu_fast();
    video_init(); audio_init(); redraw(); last_frame=sys_time;
    while(1) {
        vsync();
        current_frame=sys_time; elapsed=(uint8_t)(current_frame-last_frame); last_frame=current_frame;
        held=joypad(); pressed=held & ~previous; previous=held;
        old_screen=screen; changed=0;
        audio_tick();
        if((pressed&J_SELECT) && screen!=FALLEN) audio_toggle();
        switch(screen) {
            case TITLE:
                if(pressed&J_UP) { title_choice=(title_choice+2)%3; changed=1; }
                if(pressed&J_DOWN) { title_choice=(title_choice+1)%3; changed=1; }
                if(pressed&(J_A|J_START)) {
                    if(title_choice==0) { mission_index=0; screen=SELECT_MISSION; }
                    else if(title_choice==1) { screen=PASSWORD; code_invalid=0; }
                    else screen=MANUAL;
                }
                break;
            case SELECT_MISSION:
                if(pressed&(J_LEFT|J_UP)) { mission_index=(mission_index?mission_index:unlocked)-1; changed=1; }
                if(pressed&(J_RIGHT|J_DOWN)) { mission_index=(mission_index+1)%unlocked; changed=1; }
                if(pressed&J_A) { screen=BRIEFING; brief_page=0; }
                if(pressed&J_B) screen=TITLE;
                break;
            case BRIEFING:
                if(pressed&J_A) { if(!brief_page) { brief_page=1; changed=1; } else game_start(); }
                if(pressed&J_B) { if(brief_page) { brief_page=0; changed=1; } else screen=SELECT_MISSION; }
                break;
            case PLAYING:
                if(pressed&J_START) { screen=PAUSED; pause_choice=0; }
                else { game_tick(held,pressed,elapsed); if(screen==PLAYING) draw_game(); else brief_page=0; }
                break;
            case PAUSED:
                if(pressed&J_UP) { pause_choice=(pause_choice+2)%3; changed=1; }
                if(pressed&J_DOWN) { pause_choice=(pause_choice+1)%3; changed=1; }
                if(pressed&(J_B|J_START)) screen=PLAYING;
                if(pressed&J_A) {
                    if(pause_choice==0) screen=PLAYING;
                    else if(pause_choice==1) game_start();
                    else screen=SELECT_MISSION;
                }
                break;
            case FALLEN:
                if(pressed&J_START) game_rewind();
                else if(pressed&J_B) game_start();
                else if(pressed&J_SELECT) screen=SELECT_MISSION;
                break;
            case DEBRIEF:
                if(pressed&J_START) {
                    if(!brief_page) { brief_page=1; changed=1; }
                    else if(mission_index==MISSION_COUNT-1) screen=COMPLETE;
                    else { mission_index++; brief_page=0; screen=BRIEFING; }
                }
                break;
            case MANUAL:
                if(pressed&(J_A|J_B|J_START)) screen=TITLE;
                break;
            case PASSWORD:
                if(pressed&J_LEFT) { code_cursor=(code_cursor+3)&3; changed=1; }
                if(pressed&J_RIGHT) { code_cursor=(code_cursor+1)&3; changed=1; }
                if(pressed&J_UP) { code_digits[code_cursor]=(code_digits[code_cursor]+1)%10; changed=1; }
                if(pressed&J_DOWN) { code_digits[code_cursor]=(code_digits[code_cursor]+9)%10; changed=1; }
                if(pressed&J_A) {
                    code=code_digits[0]*1000u+code_digits[1]*100u+code_digits[2]*10u+code_digits[3];
                    decoded=progress_decode(code);
                    if(decoded) { unlocked=decoded; mission_index=decoded-1; screen=SELECT_MISSION; }
                    else { code_invalid=1; changed=1; }
                }
                if(pressed&J_B) screen=TITLE;
                break;
            case COMPLETE:
                if(pressed&J_START) screen=SELECT_MISSION;
                break;
        }
        if(old_screen!=screen || changed) { redraw(); audio_sfx(S_CONFIRM); last_frame=sys_time; }
    }
}
