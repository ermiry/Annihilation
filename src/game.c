#include <stdlib.h>
#include <string.h>

#include "annihilation.h"

#include "game.h"
#include "player.h"

#include "animation.h"

/*** GAME OBJECTS ***/

#pragma region GAME OBJECTS

// TODO: implement custom object pooling to search for free spaces
GameObject **gameObjects;
u32 max_gos;
u32 curr_max_objs;
u32 new_go_id;

// init our game objects array
u8 game_object_init (void) {

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

// game object constructor
GameObject *game_object_new (const char *name, const char *tag) {

    GameObject *new_go = (GameObject *) malloc (sizeof (GameObject));
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

        for (u8 i = 0; i < COMP_COUNT; i++) new_go->components[i] = NULL;

        new_go->id = new_go_id;
        new_go_id++;

        gameObjects[new_go->id] = new_go;
        curr_max_objs++;
    }

    return new_go;

}

// TODO: mark as free its space in the gos array
// TODO: destroy each component
void game_object_destroy (GameObject *go) {

    if (go) {
        if (go->name) free (go->name);
        if (go->tag) free (go->tag);

        free (go);
    }

}

// TODO: implement object pooling for components
void game_object_add_component (GameObject *go, GameComponent component) {

    if (go) {
        switch (component) {
            case POSITION_COMP: break;
            case GRAPHICS_COMP: {
                Graphics *graphics = (Graphics *) malloc (sizeof (Graphics));
                graphics->goID = go->id;
                graphics->sprite = NULL;
                graphics->spriteSheet = NULL;
                graphics->multipleSprites = false;
                graphics->x_sprite_offset = 0;
                graphics->y_sprite_offset = 0;
                go->components[component] = graphics;
            } break;

            case ANIMATOR_COMP: {
                Animator *animator = (Animator *) malloc (sizeof (Animator));
                animator->goID = go->id;
                animator->currAnimation = NULL;
                go->components[component] = animator;
            } break;
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

/*** GAME MANAGER ***/

extern GameObject *main_player_go;

#pragma region GAME MANAGER

u8 game_init (void) {

    game_object_init ();

    main_player_go = player_init ();

    return 0;

}

// TODO: check of inactive game objects with id of -1;
void game_update (void) {

    // update every game object
    for (u32 i = 0; i < curr_max_objs; i++) 
        if (gameObjects[i]->update)
            gameObjects[i]->update (NULL);

}

// FIXME: we need to clean up the game and game objects!!

#pragma endregion