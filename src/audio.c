#include "game.h"
uint8_t sound_on=1;
static uint8_t music_tick,note;
static const uint16_t melody[16]={1310,0,1546,0,1468,0,1234,0,1310,0,1649,1546,1468,0,1234,0};

void audio_init(void) {
    NR52_REG=0x80; NR50_REG=0x55; NR51_REG=0xff;
    NR10_REG=0; music_tick=0; note=0;
}
void audio_toggle(void) {
    sound_on=!sound_on;
    if(sound_on) audio_init(); else NR52_REG=0;
}
void audio_tick(void) {
    uint16_t frequency;
    if(!sound_on) return;
    if(++music_tick<18) return;
    music_tick=0;
    frequency=melody[note++ & 15];
    if(!frequency) return;
    NR21_REG=0x80; NR22_REG=(screen==PLAYING)?0x32:0x52;
    NR23_REG=(uint8_t)frequency; NR24_REG=0x80 | (frequency>>8);
}
void audio_sfx(uint8_t effect) {
    if(!sound_on) return;
    if(effect==S_BOOM || effect==S_HIT) {
        NR41_REG=0x10; NR42_REG=(effect==S_BOOM)?0xc3:0x83;
        NR43_REG=(effect==S_BOOM)?0x35:0x21; NR44_REG=0xc0;
    } else {
        NR10_REG=(effect==S_REWIND)?0x6e:0x16;
        NR11_REG=0x80; NR12_REG=0x83;
        NR13_REG=(effect==S_FIRE)?0xc0:0x40;
        NR14_REG=(effect==S_FIRE)?0xc6:0xc7;
    }
}
