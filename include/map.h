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

#endif