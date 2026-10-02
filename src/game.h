#ifndef ROLLBACK_GAME_H
#define ROLLBACK_GAME_H
#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>

#include "campaign.h"
#define ENEMY_COUNT 6
#define SHOT_COUNT 8
#define HOSTILE_COUNT 6
#define OBJECT_COUNT 4
#define EFFECT_COUNT 4
#define MAX_HULL 6
#define WORLD_WIDTH 1600
#define WORLD_HEIGHT 1440
#define CAMERA_LEFT 56
#define CAMERA_RIGHT 104
#define CAMERA_TOP 40
#define CAMERA_BOTTOM 72

enum MissionType { DEFENSE, RAID, ESCORT, CHASE, SURVIVAL, RESCUE, EVADE, SABOTAGE, BOSS };
enum Screen { TITLE, SELECT_MISSION, BRIEFING, PLAYING, PAUSED, FALLEN, DEBRIEF, MANUAL, PASSWORD, COMPLETE, RADIO, SETTINGS, ARCHIVE, ENDING, EPISODE };
enum Sound { S_FIRE, S_HIT, S_BOOM, S_PICKUP, S_CONFIRM, S_REWIND, S_WIN };

typedef struct { int16_t x,y; int8_t dx,dy; uint8_t active,life,pierce; } Shot;
typedef struct { int16_t x,y; uint8_t hp,kind,cool,flash,role; } Enemy;
typedef struct { int16_t x,y; uint8_t hp,kind; } Objective;
typedef struct { int16_t x,y; uint8_t life; } Effect;
typedef struct {
    uint16_t ticks,frame,score,rng;
    int16_t x,y,camera_x,camera_y;
    uint8_t hull,face,fire,invuln,boost,boost_cool;
    int8_t move_x,move_y;
    uint8_t kills,progress,spawn_cool,spawned,checkpoint_mark;
    uint16_t channel,events,upload_ticks,kill_times[4];
    uint8_t convoy_step,convoy_clock,finishing,boss_stage,phase,exposure;
    uint8_t weapon,weapon_time,shield,brake,extra,chain,hits,perimeter_left;
    uint8_t target_damage,core_count,site_next,pickup;
    int16_t echo_x,echo_y;
    Enemy enemies[ENEMY_COUNT];
    Shot shots[SHOT_COUNT], hostile[HOSTILE_COUNT];
    Objective objects[OBJECT_COUNT];
    Effect effects[EFFECT_COUNT];
} Game;

extern Game game;
extern uint8_t screen,mission_index,unlocked,rewinds,brief_page,sound_on;
extern uint8_t radio_timer;
extern char radio_line[21];
extern char failure_reason[21];

void game_start(void) BANKED;
void game_tick(uint8_t held,uint8_t pressed,uint8_t elapsed) BANKED;
void game_rewind(void) BANKED;
void game_checkpoint(void) BANKED;
uint8_t map_solid(int16_t x,int16_t y) NONBANKED;
void game_target(int16_t *x,int16_t *y) BANKED;
void radio(const char *line,uint8_t priority) NONBANKED;

void video_init(void) NONBANKED;
void page_begin(void) NONBANKED;
void page_end(void) NONBANKED;
void text_at(uint8_t x,uint8_t y,const char *s,uint8_t palette) NONBANKED;
void number_at(uint8_t x,uint8_t y,uint16_t n,uint8_t digits,uint8_t palette) NONBANKED;
void wrapped(uint8_t x,uint8_t y,const char *s,uint8_t width,uint8_t rows,uint8_t palette) NONBANKED;
void frame(uint8_t x,uint8_t y,uint8_t w,uint8_t h,uint8_t palette) NONBANKED;
void draw_title(uint8_t choice) BANKED;
void draw_selection(void) BANKED;
void draw_brief(void) BANKED;
void draw_arena(void) BANKED;
void draw_game(void) BANKED;
void draw_pause(uint8_t choice) BANKED;
void draw_fallen(void) BANKED;
void draw_debrief(void) BANKED;
void draw_manual(void) BANKED;
void draw_password(const uint8_t *digits,uint8_t cursor,uint8_t invalid) BANKED;
void draw_complete(void) BANKED;
void draw_dialog(void) BANKED;
void draw_settings(uint8_t choice) BANKED;
void draw_archive(uint8_t index,uint8_t part) BANKED;
void draw_ending(uint8_t step) BANKED;
void portrait_at(uint8_t x,uint8_t y,uint8_t speaker,uint8_t mood) NONBANKED;
void game_fail(const char *reason) NONBANKED;
void game_finish(void) BANKED;

void audio_init(void);
void audio_tick(void);
void audio_sfx(uint8_t effect);
void audio_toggle(void);
#endif
