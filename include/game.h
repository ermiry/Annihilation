#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

#include "annihilation.h"

#define DEFAULT_MAX_GOS     10

#define COMP_COUNT          3

typedef struct GameObject {

    u32 id;

    char *name;
    char *tag;
    void *components[COMP_COUNT];

    void (*update)(void *data);

} GameObject;

/*** COMPONENTS ***/

typedef enum GameComponent {

    POSITION_COMP = 0,
    GRAPHICS_COMP,

    PLAYER_COMP,

} GameComponent;

typedef struct Position {

    u32 goId;
    u32 x, y;  

} Position;

#define UNSET_LAYER     0
#define GROUND_LAYER    1
#define LOWER_LAYER     2
#define MID_LAYER       3
#define TOP_LAYER       4

typedef struct Graphics {

    u32 goId;
    u8 layer; 
    bool hasBeenSeen;
    bool visibleOutsideFov;

} Graphics;

#endif