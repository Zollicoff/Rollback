#ifndef ROLLBACK_CAMPAIGN_H
#define ROLLBACK_CAMPAIGN_H
#include <gb/gb.h>
#include <stdint.h>
#define MISSION_COUNT 54
#define PAGE_CHARS 160
#define MAX_EVENTS 10
#define WAYPOINTS 8
/* These records are the compiler/runtime cartridge interface. All pointers in a
   StoryData record belong to that record's bank. Only bank_read dereferences them. */
enum Rule { STANDARD, ESCAPE, DUPLICATE, PILOT_RESCUE, FLEET, RADAR, LAUNCH_FAILURE, SEARCH, DUAL_BOSS, PEACEFUL, UPLOAD_RACE, VIRUS, TALK };
enum Condition { C_START,C_TICKS,C_PROGRESS,C_KILLS,C_BOSS,C_DONE,C_HIT,C_CHANNEL,C_CONTACT,C_EXPOSURE };
enum BarkTrigger { BK_KILL,BK_MULTI,BK_DAMAGE,BK_LOW,BK_NEAR,BK_WAVE,BK_PICKUP,BK_CORE,BK_RETRY,BK_IDLE,BK_DONE,BK_ENEMY_HIT,BK_ENEMY_HEAVY,BK_APPEAR,BK_ATTACK,BK_DESTROY,BK_COUNT };
typedef struct { uint8_t speaker,mood,flags; const char *text; } StoryPage;
typedef struct { uint16_t first; uint8_t count; } Scene;
typedef struct { uint8_t condition; uint16_t value; Scene scene; } StoryEvent;
typedef struct { int16_t x,y; } Point;
typedef struct {
    const char *title,*objective,*hint,*setting;
    uint8_t type,layout,episode,rule,goal,enemy,boss;
    uint16_t seconds;
    Point start,points[WAYPOINTS];
    Scene briefing,debrief;
    const StoryPage *pages;
    const StoryEvent *events;
    uint8_t event_count;
} StoryData;
typedef struct { uint8_t bank; const StoryData *data; } StoryRef;
typedef struct {
    char title[40],objective[100],hint[21],setting[32];
    uint8_t type,layout,episode,rule,goal,enemy,boss;
    uint16_t seconds;
    Point start,points[WAYPOINTS];
} Mission;
typedef struct { uint8_t speaker,mood,flags; char text[PAGE_CHARS]; } Page;
typedef struct { uint8_t trigger,speaker,first,last,flags; const char *text; } Bark;
typedef struct { uint8_t bank; const Bark *data; uint8_t count; } BarkRef;
typedef struct { uint8_t bank; const StoryPage *pages; uint8_t count,unlock; } ArchiveRef;
extern const StoryRef campaign[56];
extern const BarkRef bark_pools[];
extern const uint8_t bark_pool_count;
extern const ArchiveRef archive[];
extern const uint8_t archive_count;
extern const char *const speakers[];
extern const char *const moods[];
extern const char *const mode_names[9];
extern Mission mission;
extern StoryData story;
extern uint8_t story_bank;
extern Page page;
extern Scene scene;
extern uint8_t scene_page,scene_return,replay,loop_two,main_complete,difficulty;
extern uint16_t total_kills,total_rewinds,total_cores;
extern uint32_t achievements;
void bank_read(void *dest,const void *source,uint16_t length,uint8_t bank) NONBANKED;
void bank_text(char *dest,const char *source,uint8_t limit,uint8_t bank) NONBANKED;
void mission_load(void) NONBANKED;
void story_open(Scene group,uint8_t destination) NONBANKED;
void story_page(void) NONBANKED;
uint8_t story_next(void) NONBANKED;
void story_poll(void) BANKED;
void campaign_complete(void) BANKED;
void bark(uint8_t trigger) BANKED;
void bark_tick(uint8_t elapsed) BANKED;
void bark_reset(void) BANKED;
void archive_load(uint8_t index,uint8_t part) NONBANKED;
uint32_t progress_code(void) NONBANKED;
uint8_t progress_decode(uint32_t code) NONBANKED;
#endif
