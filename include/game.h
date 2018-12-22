#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

#include "annihilation.h"

#include "sprites.h"

#define DEFAULT_MAX_GOS     10

#define COMP_COUNT          3

typedef struct GameObject {

    u32 id;

    char *name;
    char *tag;
    void *components[COMP_COUNT];

    void (*update)(void *data);

} GameObject;

extern GameObject *game_object_new (const char *name, const char *tag);

/*** COMPONENTS ***/

typedef enum GameComponent {

    POSITION_COMP = 0,
    GRAPHICS_COMP,
    ANIMATOR_COMP,

    PLAYER_COMP,

} GameComponent;

typedef struct Position {

    u32 goID;
    u32 x, y;  

} Position;

#define UNSET_LAYER     0
#define GROUND_LAYER    1
#define LOWER_LAYER     2
#define MID_LAYER       3
#define TOP_LAYER       4

typedef struct Graphics {

    u32 goID;

    Sprite *sprite;
    SpriteSheet *spriteSheet;

    u32 x_sprite_offset, y_sprite_offset;
    bool multipleSprites;
    // SDL_Rect src_rect, dest_rect;
    u8 layer; 
    bool hasBeenSeen;
    bool visibleOutsideFov;

} Graphics;

extern void game_object_add_component (GameObject *go, GameComponent component);
extern void *game_object_get_component (GameObject *go, GameComponent component);

/*** GAME MANAGER ***/

extern u8 game_init (void);
extern void game_update (void);

#endif