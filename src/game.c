#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "annihilation.h"

#include "game.h"
#include "player.h"

#include "engine/renderer.h"
#include "engine/textures.h"
#include "engine/animation.h"

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

void graphics_set_sprite_sheet (Graphics *graphics, const char *filename, SDL_Renderer *renderer) {

    if (graphics && filename) {
        graphics->spriteSheet = sprite_sheet_load (filename, renderer);
        graphics->multipleSprites = true;
    }

}

#pragma endregion

/*** GAME OBJECTS ***/

#pragma region GAME OBJECTS

GameObject **gameObjects;
u32 max_gos;
u32 curr_max_objs;
u32 new_go_id;

// init our game objects array
static u8 game_object_init (void) {

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

// game object constructor
GameObject *game_object_new (const char *name, const char *tag) {

    GameObject *new_go = NULL;

    // first check if we have a reusable go in the array
    i32 spot = game_object_get_free_spot ();
    if (spot >= 0) new_go = gameObjects[spot];
    else new_go = (GameObject *) malloc (sizeof (GameObject));

    if (new_go) {
        if (name) {
            new_go->name = (char *) calloc (strlen (name) + 1, sizeof (char));
            strcpy (new_go->name, name);
        }

        else new_go->name = NULL;

        if (tag) {
            new_go->tag = (char *) calloc (strlen (name) + 1, sizeof (char));
            strcpy (new_go->tag, tag);
        }

        else new_go->tag = NULL;

        for (u8 i = 0; i < COMP_COUNT; i++) new_go->components[i] = NULL;

        new_go->id = new_go_id;
        new_go_id++;

        gameObjects[new_go->id] = new_go;
        curr_max_objs++;
    }

    return new_go;

}

// mark as inactive or reusable the game object
static void game_object_destroy (GameObject *go) {

    if (go) {
        go->id = -1;
        go->update = NULL;

        if (go->name) free (go->name);
        if (go->tag) free (go->tag);

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
void game_object_add_component (GameObject *go, GameComponent component) {

    if (go) {
        switch (component) {
            case TRANSFORM_COMP: go->components[component] = transform_new (go->id); break;
            case GRAPHICS_COMP: go->components[component] = graphics_new (go->id); break;

            case ANIMATOR_COMP: go->components[component] = animator_new (go->id); break;
            case PLAYER_COMP: 
                go->components[component] = player_create_comp (go->id); 
                go->update = player_update;
                break;

            default: break;
        }
    }

}

void *game_object_get_component (GameObject *go, GameComponent component) {

    if (go) return go->components[component];

}

#pragma endregion

/*** GAME STATE ***/

#pragma region GAME STATE

GameState *game_state = NULL;

static u8 game_init (void) {

    game_object_init ();

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

extern SDL_Renderer *renderer;

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

    // clean up game objects
    for (u32 i = 0; i < curr_max_objs; i++) 
        if (gameObjects[i])
            game_object_delete (gameObjects[i]);

    free (gameObjects);
    
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