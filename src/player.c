#include "annihilation.h"

#include "game.h"
#include "player.h"

#include "input.h"
#include "sprites.h"
#include "animation.h"

GameObject *main_player_go = NULL;

static Transform *my_trans = NULL;
static Graphics *my_graphics = NULL;
static Animator *my_anim = NULL;

static Animation *player_idle_anim = NULL;
static Animation *player_run_anim = NULL;

static Vector2D my_vel = { 0, 0 };

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
        game_object_add_component (new_player_go, TRANSFORM_COMP);
        game_object_add_component (new_player_go, GRAPHICS_COMP);
        game_object_add_component (new_player_go, ANIMATOR_COMP);
        game_object_add_component (new_player_go, PLAYER_COMP);

        my_trans = game_object_get_component (new_player_go, TRANSFORM_COMP);

        my_graphics = game_object_get_component (new_player_go, GRAPHICS_COMP);
        graphics_set_sprite_sheet (my_graphics, "./assets/adventurer-sheet.png", renderer);
        sprite_sheet_set_sprite_size (my_graphics->spriteSheet, 50, 37);
        sprite_sheet_set_scale_factor (my_graphics->spriteSheet, 6);
        sprite_sheet_crop (my_graphics->spriteSheet);

        // set up animations
        player_idle_anim = animation_create (4,
            my_graphics->spriteSheet->individualSprites[0][0], my_graphics->spriteSheet->individualSprites[1][0], 
            my_graphics->spriteSheet->individualSprites[2][0], my_graphics->spriteSheet->individualSprites[3][0]);
        animation_set_speed (player_idle_anim, 300);  

        player_run_anim = animation_create (6, 
            my_graphics->spriteSheet->individualSprites[0][1], my_graphics->spriteSheet->individualSprites[1][1], 
            my_graphics->spriteSheet->individualSprites[2][1], my_graphics->spriteSheet->individualSprites[3][1], 
            my_graphics->spriteSheet->individualSprites[4][1], my_graphics->spriteSheet->individualSprites[5][1]);
        animation_set_speed (player_run_anim, 150);

        my_anim = (Animator *) game_object_get_component (new_player_go, ANIMATOR_COMP);
        animator_set_current_animation (my_anim, player_idle_anim);
    }

    return new_player_go;

}

// FIXME: normalize the vector when moving in diagonal!!
void player_update (void *data) {

    // update player position
    Vector2D new_vel = { 0, 0 };
    bool moving = false;
    if (input_is_key_down (SDL_SCANCODE_D)) {
        new_vel.x = 8;
        my_graphics->flip = NO_FLIP;
        moving = true;
    }

    if (input_is_key_down (SDL_SCANCODE_A)) {
        new_vel.x = -8;
        my_graphics->flip = FLIP_HORIZONTAL;
        moving = true;
    }

    if (input_is_key_down (SDL_SCANCODE_S)) {
        new_vel.y = 8;
        moving = true;
    } 

    if (input_is_key_down (SDL_SCANCODE_W)) {
        new_vel.y = -8;
        moving = true;
    } 

    if (moving) {
        vector_add_equal (&my_trans->position, new_vel);
        animator_set_current_animation (my_anim, player_run_anim);
    }

    else animator_set_current_animation (my_anim, player_idle_anim);

    // update player animation
    i32 currFrame = (int) (((SDL_GetTicks () / my_anim->currAnimation->speed) % 
        my_anim->currAnimation->n_frames));    

    my_graphics->x_sprite_offset = my_anim->currAnimation->frames[currFrame]->col;
    my_graphics->y_sprite_offset = my_anim->currAnimation->frames[currFrame]->row;

}

void player_destroy_comp (Player *player) {

    if (player) {
        free (player);
    }

}