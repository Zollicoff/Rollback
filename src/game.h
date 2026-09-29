#ifndef ROLLBACK_GAME_H
#define ROLLBACK_GAME_H
#include <gb/gb.h>
#include <gb/cgb.h>
#include <stdint.h>

#define MISSION_COUNT 12
#define ENEMY_COUNT 6
#define SHOT_COUNT 8
#define HOSTILE_COUNT 6
#define OBJECT_COUNT 4
#define EFFECT_COUNT 4
#define MAX_HULL 6
#define WORLD_WIDTH 1600
#define CAMERA_LEFT 56
#define CAMERA_RIGHT 104

enum MissionType { DEFENSE, RAID, ESCORT, CHASE, SURVIVAL, RESCUE, EVADE, SABOTAGE, BOSS };
enum Screen { TITLE, SELECT_MISSION, BRIEFING, PLAYING, PAUSED, FALLEN, DEBRIEF, MANUAL, PASSWORD, COMPLETE };
enum Sound { S_FIRE, S_HIT, S_BOOM, S_PICKUP, S_CONFIRM, S_REWIND, S_WIN };

typedef struct {
    const char *title;
    const char *brief;
    const char *hint;
    const char *debrief;
    uint8_t type, layout, seconds, goal, difficulty;
} Mission;
typedef struct { int16_t x,y; int8_t dx,dy; uint8_t active,life; } Shot;
typedef struct { int16_t x,y; uint8_t hp,kind,cool,flash; } Enemy;
typedef struct { int16_t x,y; uint8_t hp,kind; } Objective;
typedef struct { int16_t x,y; uint8_t life; } Effect;
typedef struct {
    uint16_t ticks,score,rng;
    int16_t x,y,camera_x;
    uint8_t hull,face,fire,invuln,boost,boost_cool;
    int8_t move_x,move_y;
    uint8_t kills,progress,spawn_cool,spawned,checkpoint_mark;
    uint8_t channel,convoy_step,convoy_clock;
    Enemy enemies[ENEMY_COUNT];
    Shot shots[SHOT_COUNT], hostile[HOSTILE_COUNT];
    Objective objects[OBJECT_COUNT];
    Effect effects[EFFECT_COUNT];
} Game;

extern const Mission missions[MISSION_COUNT];
extern const char *const mode_names[9];
extern Game game;
extern uint8_t screen,mission_index,unlocked,rewinds,brief_page,sound_on;
extern uint8_t radio_timer;
extern const char *radio_line;
extern const char *failure_reason;

void game_start(void);
void game_tick(uint8_t held,uint8_t pressed,uint8_t elapsed);
void game_rewind(void);
void game_checkpoint(void);
uint8_t map_solid(int16_t x,int16_t y);
int16_t game_target_x(void);
void radio(const char *line,uint8_t priority);
uint16_t progress_code(uint8_t count);
uint8_t progress_decode(uint16_t code);

void video_init(void);
void page_begin(void);
void page_end(void);
void text_at(uint8_t x,uint8_t y,const char *s,uint8_t palette);
void number_at(uint8_t x,uint8_t y,uint16_t n,uint8_t digits,uint8_t palette);
void wrapped(uint8_t x,uint8_t y,const char *s,uint8_t width,uint8_t rows,uint8_t palette);
void frame(uint8_t x,uint8_t y,uint8_t w,uint8_t h,uint8_t palette);
void draw_title(uint8_t choice);
void draw_selection(void);
void draw_brief(void);
void draw_arena(void);
void draw_game(void);
void draw_pause(uint8_t choice);
void draw_fallen(void);
void draw_debrief(void);
void draw_manual(void);
void draw_password(const uint8_t *digits,uint8_t cursor,uint8_t invalid);
void draw_complete(void);

void audio_init(void);
void audio_tick(void);
void audio_sfx(uint8_t effect);
void audio_toggle(void);
#endif
