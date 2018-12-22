#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "annihilation.h"

#include "textures.h"
#include "sprites.h"
#include "animation.h"

/*** RENDERING ***/

void render (SDL_Renderer *renderer) {

    // SDL_SetRenderDrawColor (renderer, 0, 0, 0, 255);
    // SDL_RenderClear (renderer);
    // SDL_RenderPresent (renderer);

}

/*** SET UP ***/

void sdl_setUp (SDL_Window **window, SDL_Renderer **renderer) {

    SDL_Init (SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    *window = SDL_CreateWindow ("Annihilation",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

    *renderer = SDL_CreateRenderer (*window, 0, SDL_RENDERER_SOFTWARE | SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor (*renderer, 0, 0, 0, 255);
    SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, "0");
    SDL_RenderSetLogicalSize (*renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

}

/*** CLEAN UP ***/

void cleanUp (SDL_Window *window, SDL_Renderer *renderer) {

    SDL_DestroyRenderer (renderer);
    SDL_DestroyWindow (window);

    SDL_Quit ();

}

/*** MAIN THREAD ***/

bool running = false;
bool inGame = false;
bool wasInGame = false;

int main (void) {

    srand ((unsigned) time (NULL));

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    sdl_setUp (&window, &renderer);

    SDL_Event event;

    u32 timePerFrame = 1000 / FPS_LIMIT;
    u32 frameStart;
    i32 sleepTime;

    SpriteSheet *player_sprite_sheet = sprite_sheet_load ("./assets/adventurer-sprites.png", renderer);
    sprite_sheet_set_sprite_size (player_sprite_sheet, 50, 37);
    sprite_sheet_set_scale_factor (player_sprite_sheet, 4);
    sprite_sheet_crop (player_sprite_sheet);

    Animation *player_idle_anim = animation_create (player_sprite_sheet, 4, player_sprite_sheet->individualSprites[0][0],
        player_sprite_sheet->individualSprites[1][0], player_sprite_sheet->individualSprites[2][0],
        player_sprite_sheet->individualSprites[3][0]);
    animation_set_speed (player_idle_anim, 300);

    Animation *player_run_anim = animation_create (player_sprite_sheet, 6, player_sprite_sheet->individualSprites[0][1],
        player_sprite_sheet->individualSprites[1][1], player_sprite_sheet->individualSprites[2][1],
        player_sprite_sheet->individualSprites[3][1], player_sprite_sheet->individualSprites[4][1],
        player_sprite_sheet->individualSprites[5][1]);
    animation_set_speed (player_run_anim, 150);

    running = true;
    while (running) {
        frameStart = SDL_GetTicks ();
        
        while (SDL_PollEvent (&event) != 0) {
            if (event.type == SDL_QUIT) {
                running = false;
                inGame = false;
            } 
        }

        // select player animation
        int currFrame = (int) (((SDL_GetTicks () / player_run_anim->speed) % player_run_anim->n_frames));
        int x_offset = player_run_anim->frames[currFrame]->col;
        int y_offset = player_run_anim->frames[currFrame]->row;

        // render (renderer);
        {
            SDL_RenderClear (renderer);
            texture_draw_frame (renderer, player_sprite_sheet, 0, 0, x_offset, y_offset, SDL_FLIP_NONE);
            SDL_RenderPresent (renderer);
        }

        // limit the FPS
        sleepTime = timePerFrame - (SDL_GetTicks () - frameStart);
        if (sleepTime > 0) SDL_Delay (sleepTime);
    }

    animation_destroy (player_idle_anim);
    animation_destroy (player_run_anim);

    sprite_sheet_destroy (player_sprite_sheet);

    cleanUp (window, renderer);

    return 0;

}