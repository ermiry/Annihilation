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

typedef struct Map {

    u32 width, heigth;
    GameObject ***go_map;
    Cave *cave;

} Map;

extern Cave *cave_generate (Map *map, u32 width, u32 heigth, u32 seed, u32 fillPercent);
extern void cave_destroy (Cave *cave);

extern Map *map_create (u32 width, u32 height);
extern void map_destroy (Map *map);

#endif