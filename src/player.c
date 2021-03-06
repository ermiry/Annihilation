#include "annihilation.h"

#include "game.h"
#include "player.h"

#include "engine/input.h"
#include "engine/sprites.h"
#include "engine/animation.h"

// TODO: we need a better way to reference the players we know about
GameObject *main_player_go = NULL;

static Player *mainPlayer = NULL;

static Transform *my_trans = NULL;
static Graphics *my_graphics = NULL;
static Animator *my_anim = NULL;

static Animation *player_idle_anim = NULL;
static Animation *player_run_anim = NULL;
static Animation *player_attack_anim = NULL;

static u8 moveSpeed = 8;

#pragma region CHARACTER

// TODO: do we want this only when creating a new character or also when we are 
// loading a saved profile?
Character *character_new (void) {

    Character *new_character = (Character *) malloc (sizeof (Character));
    if (new_character) {
        new_character->entity = entity_new ();
        // TODO: money
    }

    return new_character;

}

#pragma endregion

#pragma region PLAYER PROFILE

#pragma endregion

#pragma region PLAYER COMPONENT

Player *player_create_comp (u32 goID) {

    Player *new_player = (Player *) malloc (sizeof (Player));
    if (new_player) {
        new_player->goID = goID;

        new_player->currState = PLAYER_IDLE;
        new_player->profile = NULL;
        new_player->character = NULL;
    }

    return new_player;

}

GameObject *player_init (void) {

    GameObject *new_player_go = game_object_new ("player", NULL);
    if (new_player_go) {
        my_trans = (Transform *) game_object_add_component (new_player_go, TRANSFORM_COMP);
        my_graphics = (Graphics *) game_object_add_component (new_player_go, GRAPHICS_COMP);
        my_anim = (Animator *) game_object_add_component (new_player_go, ANIMATOR_COMP);
        mainPlayer = (Player *) game_object_add_component (new_player_go, PLAYER_COMP);

        graphics_set_sprite_sheet (my_graphics, "./assets/artwork/adventurer-sheet.png");
        sprite_sheet_set_sprite_size (my_graphics->spriteSheet, 50, 37);
        sprite_sheet_set_scale_factor (my_graphics->spriteSheet, 6);
        sprite_sheet_crop (my_graphics->spriteSheet);

        // set up animations
        // player idel without sword
        // player_idle_anim = animation_create (4,
        //     my_graphics->spriteSheet->individualSprites[0][0], my_graphics->spriteSheet->individualSprites[1][0], 
        //     my_graphics->spriteSheet->individualSprites[2][0], my_graphics->spriteSheet->individualSprites[3][0]);
        // animation_set_speed (player_idle_anim, 300);  

        // idle with handed sword
        player_idle_anim = animation_create (4, 
            my_graphics->spriteSheet->individualSprites[6][4], my_graphics->spriteSheet->individualSprites[7][4],
            my_graphics->spriteSheet->individualSprites[0][5], my_graphics->spriteSheet->individualSprites[1][5]);
        animation_set_speed (player_idle_anim, 250);  

        player_run_anim = animation_create (6, 
            my_graphics->spriteSheet->individualSprites[0][1], my_graphics->spriteSheet->individualSprites[1][1], 
            my_graphics->spriteSheet->individualSprites[2][1], my_graphics->spriteSheet->individualSprites[3][1], 
            my_graphics->spriteSheet->individualSprites[4][1], my_graphics->spriteSheet->individualSprites[5][1]);
        animation_set_speed (player_run_anim, 150);

        player_attack_anim = animation_create (6,
            my_graphics->spriteSheet->individualSprites[2][5], my_graphics->spriteSheet->individualSprites[3][5],
            my_graphics->spriteSheet->individualSprites[4][5], my_graphics->spriteSheet->individualSprites[5][5],
            my_graphics->spriteSheet->individualSprites[6][5], my_graphics->spriteSheet->individualSprites[7][5]);
        animation_set_speed (player_attack_anim, 100);  

        animator_set_current_animation (my_anim, player_idle_anim);
    }

    return new_player_go;

}

// FIXME: normalize the vector when moving in diagonal!!
void player_update (void *data) {

    mainPlayer->currState = PLAYER_IDLE;

    // update player position
    Vector2D new_vel = { 0, 0 };
    if (input_is_key_down (SDL_SCANCODE_D)) {
        new_vel.x = moveSpeed;
        my_graphics->flip = NO_FLIP;
        mainPlayer->currState = PLAYER_MOVING;
    }

    if (input_is_key_down (SDL_SCANCODE_A)) {
        new_vel.x = -moveSpeed;
        my_graphics->flip = FLIP_HORIZONTAL;
        mainPlayer->currState = PLAYER_MOVING;
    }

    if (input_is_key_down (SDL_SCANCODE_S)) {
        new_vel.y = moveSpeed;
        mainPlayer->currState = PLAYER_MOVING;
    } 

    if (input_is_key_down (SDL_SCANCODE_W)) {
        new_vel.y = -moveSpeed;
        mainPlayer->currState = PLAYER_MOVING;
    } 

    if (input_is_key_down (SDL_SCANCODE_F)) mainPlayer->currState = PLAYER_ATTACK;

    switch (mainPlayer->currState) {
        case PLAYER_IDLE: animator_set_current_animation (my_anim, player_idle_anim); break;
        case PLAYER_MOVING: 
            vector_add_equal (&my_trans->position, new_vel);
            animator_set_current_animation (my_anim, player_run_anim);
            break;
        case PLAYER_ATTACK: animator_play_animation (my_anim, player_attack_anim); break;

        default: break;
    }

    // update player animation 
    my_anim->currFrame = (int) (((SDL_GetTicks () / my_anim->currAnimation->speed) % 
        my_anim->currAnimation->n_frames));

    // my_anim->currFrame = (int) (SDL_GetTicks () / my_anim->currAnimation->speed);

    // if (my_anim->currFrame < my_anim->currAnimation->n_frames) {
    //     my_graphics->x_sprite_offset = my_anim->currAnimation->frames[my_anim->currFrame]->col;
    //     my_graphics->y_sprite_offset = my_anim->currAnimation->frames[my_anim->currFrame]->row;
    // }

    my_graphics->x_sprite_offset = my_anim->currAnimation->frames[my_anim->currFrame]->col;
    my_graphics->y_sprite_offset = my_anim->currAnimation->frames[my_anim->currFrame]->row;

}

// FIXME: correctly clean up the player component
void player_destroy_comp (Player *player) {

    if (player) {
        free (player);
    }

}

#pragma endregion