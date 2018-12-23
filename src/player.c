#include "annihilation.h"

#include "game.h"
#include "player.h"

#include "sprites.h"
#include "animation.h"

GameObject *main_player_go = NULL;

Player *player_create_comp (u32 goID) {

    Player *new_player = (Player *) malloc (sizeof (Player));
    if (new_player) {
        new_player->goID = goID;

        new_player->health = 0;
    }

    return new_player;

}

// FIXME: wehre do we want to put this?
extern SDL_Renderer *renderer;

GameObject *player_init (void) {

    GameObject *new_player_go = game_object_new ("player", NULL);
    if (new_player_go) {
        game_object_add_component (new_player_go, GRAPHICS_COMP);
        game_object_add_component (new_player_go, ANIMATOR_COMP);
        game_object_add_component (new_player_go, PLAYER_COMP);

        Graphics *player_graphics = game_object_get_component (new_player_go, GRAPHICS_COMP);
        graphics_set_sprite_sheet (player_graphics, "./assets/adventurer-sprites.png", renderer);
        sprite_sheet_set_sprite_size (player_graphics->spriteSheet, 50, 37);
        sprite_sheet_set_scale_factor (player_graphics->spriteSheet, 6);
        sprite_sheet_crop (player_graphics->spriteSheet);

        // set up animations
        Animation *player_idle_anim = animation_create (4,
            player_graphics->spriteSheet->individualSprites[0][0], player_graphics->spriteSheet->individualSprites[1][0], 
            player_graphics->spriteSheet->individualSprites[2][0], player_graphics->spriteSheet->individualSprites[3][0]);
        animation_set_speed (player_idle_anim, 300);  

        Animation *player_run_anim = animation_create (6, 
            player_graphics->spriteSheet->individualSprites[0][1], player_graphics->spriteSheet->individualSprites[1][1], 
            player_graphics->spriteSheet->individualSprites[2][1], player_graphics->spriteSheet->individualSprites[3][1], 
            player_graphics->spriteSheet->individualSprites[4][1], player_graphics->spriteSheet->individualSprites[5][1]);
        animation_set_speed (player_run_anim, 150);

        animator_set_current_animation ((Animator *) game_object_get_component (new_player_go, ANIMATOR_COMP),
            player_run_anim);
    }

    return new_player_go;

}

void player_update (void *data) {

    // update player animation
    Graphics *graphics = (Graphics *) game_object_get_component (main_player_go, GRAPHICS_COMP);
    Animator *animator = (Animator *) game_object_get_component (main_player_go, ANIMATOR_COMP);
    if (animator) {
        i32 currFrame = (int) (((SDL_GetTicks () / animator->currAnimation->speed) % 
            animator->currAnimation->n_frames));    

        graphics->x_sprite_offset = animator->currAnimation->frames[currFrame]->col;
        graphics->y_sprite_offset = animator->currAnimation->frames[currFrame]->row;
    }

}

void player_destroy_comp (Player *player) {

    if (player) {
        free (player);
    }

}