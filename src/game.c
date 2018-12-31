#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "annihilation.h"

#include "game.h"
#include "map.h"
#include "player.h"

#include "engine/renderer.h"
#include "engine/textures.h"
#include "engine/animation.h"

#include "utils/log.h"

/*** GAME COMPONENTS ***/

#pragma region GAME COMPONENTS

static Transform *transform_new (u32 objectID) {

    Transform *new_transform = (Transform *) malloc (sizeof (Transform));
    if (new_transform) {
        new_transform->goID = objectID;
        new_transform->position.x = 0;
        new_transform->position.y = 0;
    }

    return new_transform;

}

static void transform_destroy (Transform *transform) { if (transform) free (transform); }

static Graphics *graphics_new (u32 objectID) {

    Graphics *new_graphics = (Graphics *) malloc (sizeof (Graphics));
    if (new_graphics) {
        new_graphics->goID = objectID;
        new_graphics->sprite = NULL;
        new_graphics->spriteSheet = NULL;
        new_graphics->multipleSprites = false;
        new_graphics->x_sprite_offset = 0;
        new_graphics->y_sprite_offset = 0;
        new_graphics->layer = UNSET_LAYER;
        new_graphics->flip = NO_FLIP;
    }

    return new_graphics;

}

static void graphics_destroy (Graphics *graphics) {

    if (graphics) {
        if (graphics->sprite) sprite_destroy (graphics->sprite);
        if (graphics->spriteSheet) sprite_sheet_destroy (graphics->spriteSheet);

        free (graphics);
    }

}

void graphics_set_sprite (Graphics *graphics, const char *filename) {

    if (graphics && filename) {
        graphics->sprite = sprite_load (filename, main_renderer);
        graphics->spriteSheet = NULL;
        graphics->multipleSprites = false;
    }

}

void graphics_set_sprite_sheet (Graphics *graphics, const char *filename) {

    if (graphics && filename) {
        graphics->sprite = NULL;
        graphics->spriteSheet = sprite_sheet_load (filename, main_renderer);
        graphics->multipleSprites = true;
    }

}

static BoxCollider *collider_box_new (u32 objectID) {

    BoxCollider *new_collider = (BoxCollider *) malloc (sizeof (BoxCollider));
    if (new_collider) {
        new_collider->x = new_collider->y = 0;
        new_collider->w = new_collider->h = 0;
    }

    return new_collider;

}

// FIXME:
void collider_box_init (u32 x, u32 y, u32 w, u32 h) {}

bool collider_box_collision (const BoxCollider *a, const BoxCollider *b) {

    if (a && b) 
        if (a->x + a->w >= b->x &&
            b->x + b->w >= a->x &&
            a->y + a->h >= b->y &&
            b->y + b->h >= a->y)
                return true;

    return false;

}

static void collider_box_destroy (BoxCollider *box) { if (box) free (box); }

#pragma endregion

/*** GAME OBJECTS ***/

#pragma region GAME OBJECTS

GameObject **gameObjects;
u32 max_gos;
u32 curr_max_objs;
u32 new_go_id;

static bool game_objects_realloc (void) {

    u32 new_max_gos = curr_max_objs * 2;

    gameObjects = realloc (gameObjects, new_max_gos * sizeof (gameObjects));

    if (gameObjects) {
        max_gos = new_max_gos;
        return true;
    }

    return false;

}

// init our game objects array
static u8 game_objects_init_all (void) {

    gameObjects = (GameObject **) calloc (DEFAULT_MAX_GOS, sizeof (GameObject *));
    if (gameObjects) {
        for (u32 i = 0; i < DEFAULT_MAX_GOS; i++) gameObjects[i] = NULL;

        max_gos = DEFAULT_MAX_GOS;
        curr_max_objs = 0;
        new_go_id = 0;

        return 0;
    }

    return 1;

}

static i32 game_object_get_free_spot (void) {

    for (u32 i = 0; i < curr_max_objs; i++)
        if (gameObjects[i]->id == -1)
            return i;

    return -1;

}

static void game_object_init (GameObject *go, u32 id, const char *name, const char *tag) {

    if (go) {
        go->id = id;

        if (name) {
            go->name = (char *) calloc (strlen (name) + 1, sizeof (char));
            strcpy (go->name, name);
        }

        else go->name = NULL;

        if (tag) {
            go->tag = (char *) calloc (strlen (name) + 1, sizeof (char));
            strcpy (go->tag, tag);
        }

        else go->tag = NULL;

        for (u8 i = 0; i < COMP_COUNT; i++) go->components[i] = NULL;

        go->children = NULL;

        go->update = NULL;
    }

}

// game object constructor
GameObject *game_object_new (const char *name, const char *tag) {

    GameObject *new_go = NULL;

    // first check if we have a reusable go in the array
    i32 spot = game_object_get_free_spot ();

    if (spot >= 0) {
        new_go = gameObjects[spot];
        game_object_init (new_go, spot, name, tag);
    } 

    else {
        if (new_go_id >= max_gos) game_objects_realloc ();

        new_go = (GameObject *) malloc (sizeof (GameObject));
        if (new_go) {
            game_object_init (new_go, new_go_id, name, tag);
            gameObjects[new_go->id] = new_go;
            new_go_id++;
            curr_max_objs++;
        }
    } 

    return new_go;

}

void game_object_add_child (GameObject *parent, GameObject *child) {

    if (parent && child) {
        if (!parent->children) parent->children = llist_init (NULL);
        llist_insert_next (parent->children, llist_end (parent->children), child);
    }

}

// TODO:
void game_object_remove_child (GameObject *parent, GameObject *child) {}

// mark as inactive or reusable the game object
static void game_object_destroy (GameObject *go) {

    if (go) {
        go->id = -1;
        go->update = NULL;

        if (go->name) free (go->name);
        if (go->tag) free (go->tag);

        if (go->children) free (go->children);

        // individually destroy each component
        transform_destroy ((Transform *) go->components[TRANSFORM_COMP]);
        graphics_destroy ((Graphics *) go->components[GRAPHICS_COMP]);
        animator_destroy ((Animator *) go->components[ANIMATOR_COMP]);

        player_destroy_comp ((Player *) go->components[PLAYER_COMP]);
    }

}

static void game_object_delete (GameObject *go) {

    if (go) {
        go->update = NULL;

        // individually destroy each component
        transform_destroy ((Transform *) go->components[TRANSFORM_COMP]);
        graphics_destroy ((Graphics *) go->components[GRAPHICS_COMP]);
        animator_destroy ((Animator *) go->components[ANIMATOR_COMP]);

        player_destroy_comp ((Player *) go->components[PLAYER_COMP]);

        if (go->name) free (go->name);
        if (go->tag) free (go->tag);

        free (go);
    }

}

// TODO: implement object pooling for components
void *game_object_add_component (GameObject *go, GameComponent component) {

    void *retval = NULL;

    if (go) {
        switch (component) {
            case TRANSFORM_COMP: 
                retval = go->components[component] = transform_new (go->id); 
                break;
            case GRAPHICS_COMP: 
                retval = go->components[component] = graphics_new (go->id); 
                break;
            case ANIMATOR_COMP: 
                retval = go->components[component] = animator_new (go->id); 
                break;

            case PLAYER_COMP: 
                retval = go->components[component] = player_create_comp (go->id); 
                go->update = player_update;
                break;

            default: break;
        }
    }

    return retval;

}

void *game_object_get_component (GameObject *go, GameComponent component) {

    if (go) return go->components[component];

}

#pragma endregion

/*** GAME STATE ***/

#pragma region GAME STATE

GameState *game_state = NULL;

// TODO: this is only for testing
Map *game_map = NULL;

static u8 game_init (void) {

    game_objects_init_all ();

    game_map = map_create (100, 50);
    game_map->cave = cave_generate (game_map, game_map->width, game_map->heigth, 100, 50);

    main_player_go = player_init ();

    return 0;

}

static void game_onEnter (void) { game_init (); }

static void game_onExit (void) {}

static void game_update (void) {

    // update every game object
    for (u32 i = 0; i < curr_max_objs; i++) {
        if (gameObjects[i]->id != -1) {
            if (gameObjects[i]->update)
                gameObjects[i]->update (NULL);
        }
    }
    
}

static void game_render (void) {

    Transform *transform = NULL;
    Graphics *graphics = NULL;
    for (u32 i = 0; i < curr_max_objs; i++) {
        transform = (Transform *) game_object_get_component (gameObjects[i], TRANSFORM_COMP);
        graphics = (Graphics *) game_object_get_component (gameObjects[i], GRAPHICS_COMP);
        if (transform && graphics) {
            if (graphics->multipleSprites)
                texture_draw_frame (main_renderer, graphics->spriteSheet, 
                transform->position.x, transform->position.y, 
                graphics->x_sprite_offset, graphics->y_sprite_offset,
                graphics->flip);
            
            else
                texture_draw (main_renderer, graphics->sprite, 
                transform->position.x, transform->position.y, 
                graphics->flip);
        }
    }

}

void game_cleanUp (void) {

    map_destroy (game_map);

    // clean up game objects
    for (u32 i = 0; i < curr_max_objs; i++) 
        if (gameObjects[i])
            game_object_delete (gameObjects[i]);

    free (gameObjects);

    #ifdef DEV
    logMsg (stdout, SUCCESS, GAME, "Done cleaning up game data!");
    #endif
    
}

GameState *game_state_new (void) {

    GameState *new_game_state = (GameState *) malloc (sizeof (GameState));
    if (new_game_state) {
        new_game_state->state = IN_GAME;

        new_game_state->update = game_update;
        new_game_state->render = game_render;

        new_game_state->onEnter = game_onEnter;
        new_game_state->onExit = game_onExit;
    }

}

#pragma endregion

/*** GAME MANAGER ***/

#pragma region GAME MANAGER

GameManager *game_manager = NULL;

GameState *menu_state = NULL;
GameState *game_over_state = NULL;

GameManager *game_manager_new (GameState *initState) {

    GameManager *new_game_manager = (GameManager *) malloc (sizeof (GameManager));
    if (new_game_manager) {
        new_game_manager->currState = initState;
        if (new_game_manager->currState->onEnter)
            new_game_manager->currState->onEnter ();
    } 

    return new_game_manager;

}

State game_state_get_current (void) { return game_manager->currState->state; }

void game_state_change_state (GameState *newState) { 
    
    if (game_manager->currState->onExit)
        game_manager->currState->onExit ();

    game_manager->currState = newState; 
    if (game_manager->currState->onEnter)
        game_manager->currState->onEnter ();
    
}

#pragma endregion