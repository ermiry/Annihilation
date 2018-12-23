#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "annihilation.h"

#include "game.h"
#include "player.h"

#include "animation.h"

/*** GAME COMPONENTS ***/

#pragma region GAME COMPONENTS

Graphics *graphics_new (u32 objectID) {

    Graphics *new_graphics = (Graphics *) malloc (sizeof (Graphics));
    if (new_graphics) {
        new_graphics->goID = objectID;
        new_graphics->sprite = NULL;
        new_graphics->spriteSheet = NULL;
        new_graphics->multipleSprites = false;
        new_graphics->x_sprite_offset = 0;
        new_graphics->y_sprite_offset = 0;
    }

    return new_graphics;

}

void graphics_destroy (Graphics *graphics) {

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

        else new_go->tag = NULL;

        for (u8 i = 0; i < COMP_COUNT; i++) new_go->components[i] = NULL;

        new_go->id = new_go_id;
        new_go_id++;

        gameObjects[new_go->id] = new_go;
        curr_max_objs++;
    }

    return new_go;

}

// TODO: implement object pooling
// TODO: mark as free its space in the gos array
void game_object_delete (GameObject *go) {

    if (go) {
        go->update = NULL;

        // individually destroy each component
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
            case POSITION_COMP: break;
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

/*** GAME MANAGER ***/

extern GameObject *main_player_go;

#pragma region GAME MANAGER

u8 game_init (void) {

    game_object_init ();

    main_player_go = player_init ();

    return 0;

}

// TODO: check for inactive game objects with id of -1;
void game_update (void) {

    // update every game object
    for (u32 i = 0; i < curr_max_objs; i++) 
        if (gameObjects[i]->update)
            gameObjects[i]->update (NULL);

}

void game_cleanUp (void) {

    // clean up game objects
    for (u32 i = 0; i < curr_max_objs; i++) 
        if (gameObjects[i])
            game_object_delete (gameObjects[i]);

    free (gameObjects);
    
}

#pragma endregion