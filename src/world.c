#include "game.h"
/* Every biome keeps the x=80 and y=80 sector crossings open. Objectives and
   transports use those connected lanes; cover never seals off a world sector. */
uint8_t map_solid(int16_t x,int16_t y) NONBANKED {
    uint8_t sector=mission_index==53?0:mission_index,local;
    if(x<8 || x>=WORLD_WIDTH-8 || y<24 || y>=WORLD_HEIGHT-24)return 1;
    if(x<24 || x>=WORLD_WIDTH-24 || y<40 || y>=WORLD_HEIGHT-40)return 0;
    while(x>=160){x-=160;sector++;}
    while(y>=144){y-=144;sector++;}
    local=(uint8_t)x;
    switch(mission.layout) {
        case 0: /* flooded Kestrel: rooftops and broken retaining walls */
            return (local>=16+8*(sector%3) && local<64 && y>=40 && y<56) ||
                   (local>=104 && local<144 && y>=96 && y<104 && (sector&1));
        case 1: /* Channel: sparse ship decks below open flight lanes */
            return !(sector&1) && local>=24 && local<56 && y>=96 && y<112;
        case 2: /* night river: alternating docks */
            return (local>=16 && local<64 && y>=40 && y<56) ||
                   (local>=112 && local<144 && y>=104 && y<120 && (sector&1));
        case 3: /* Cold War air base: radar shelters beside runways */
            return (y>=48 && y<64 && local>=24 && local<64) ||
                   (y>=96 && y<112 && local>=112 && local<136);
        case 4: /* server farm: paired racks with cross aisles */
            return ((local>=24 && local<56) || (local>=112 && local<136)) &&
                   ((y>=40 && y<56) || (y>=104 && y<120));
        case 5: /* cathedral: columns and buttresses */
            return ((local>=24 && local<40) || (local>=120 && local<136)) &&
                   ((y>=32 && y<64) || (y>=96 && y<120));
        case 6: /* newsreel city: alternating plazas and stage blocks */
            return sector%3 && local>=24 && local<64 && y>=40 && y<64;
        case 7: /* occupied valley: field walls, bunkers */
            return (local>=16 && local<56 && y>=104 && y<112) ||
                   (local>=112 && local<144 && y>=40 && y<56 && (sector&1));
        case 8: /* the Quiet One: isolated remnants */
            return sector%4==0 && local>=32 && local<56 && y>=40 && y<56;
        default: /* Day Zero: city blocks separated by open roads */
            return (local>=24 && local<64 && y>=32 && y<64) ||
                   (local>=112 && local<144 && y>=96 && y<120);
    }
}
