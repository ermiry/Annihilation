#ifndef MAP_H
#define MAP_H

#include <stdbool.h>

#include "annihilation.h"

#include "utils/llist.h"

typedef struct Coord {

    u16 x;
    u16 y;

} Coord;

typedef struct Room {

    LList *tiles;
    LList *edgeTiles;
    LList *connectedRooms;

    u16 roomSize;
    bool isMainRoom;
    bool isAccessibleFromMain;

} Room;

typedef struct Cave {

    u32 width, heigth;
    bool useRandomSeed;
    u32 seed;
    u32 fillPercent;
    u8 **map;

} Cave;

extern Cave *cave_generate (u32 width, u32 heigth, u32 seed, u32 fillPercent);
extern void cave_destroy (Cave *cave);

#endif