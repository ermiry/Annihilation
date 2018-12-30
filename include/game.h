#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

#include "annihilation.h"

#include "engine/sprites.h"
#include "vector2d.h"

#define DEFAULT_MAX_GOS     200

#define COMP_COUNT          5

// FIXME: implement parent - child hierarchy
typedef struct GameObject {

    i32 id;

    char *name;
    char *tag;
    void *components[COMP_COUNT];

    void (*update)(void *data);

} GameObject;

extern GameObject *game_object_new (const char *name, const char *tag);

/*** COMPONENTS ***/

typedef enum GameComponent {

    TRANSFORM_COMP = 0,
    GRAPHICS_COMP,
    ANIMATOR_COMP,
    BOX_COLLIDER_COMP,

    PLAYER_COMP,

} GameComponent;

extern void *game_object_add_component (GameObject *go, GameComponent component);
extern void *game_object_get_component (GameObject *go, GameComponent component);

typedef struct Transform {

    u32 goID;
    Vector2D position;

} Transform;

#define UNSET_LAYER     0
#define GROUND_LAYER    1
#define LOWER_LAYER     2
#define MID_LAYER       3
#define TOP_LAYER       4

typedef enum Flip {

    NO_FLIP = 0x00000000,
    FLIP_HORIZONTAL = 0x00000001,
    FLIP_VERTICAL = 0x00000002

} Flip;

typedef struct Graphics {

    u32 goID;

    Sprite *sprite;
    SpriteSheet *spriteSheet;

    u32 x_sprite_offset, y_sprite_offset;
    bool multipleSprites;
    u8 layer; 
    Flip flip;
    bool hasBeenSeen;
    bool visibleOutsideFov;

} Graphics;

extern void graphics_set_sprite (Graphics *graphics, const char *filename);
extern void graphics_set_sprite_sheet (Graphics *graphics, const char *filename);

typedef struct BoxCollider {

    u32 goID;

    u32 x, y;
    u32 w, h;

} BoxCollider;

/*** GAME MANAGER ***/

typedef enum State {

    MENU = 0,
    IN_GAME = 1,
    GAME_OVER = 2,

} State;

typedef struct GameSate {

    State state;

    void (*update)(void);
    void (*render)(void);

    void (*onEnter)(void);
    void (*onExit)(void);

} GameState;

typedef struct GameManager {

    GameState *currState;

} GameManager;

extern GameManager *game_manager;

extern GameManager *game_manager_new (GameState *initState);
extern State game_state_get_current (void);
extern void game_state_change_state (GameState *newState);

// TODO: maybe add a function to register when we change to a state,
// so that we can load many things with like an array of events?

/*** GAME STATE ***/

extern GameState *game_state;

extern GameState *game_state_new (void);

// FIXME: move this to on exit or something like that
extern void game_cleanUp (void);

#endif