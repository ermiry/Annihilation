#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "annihilation.h"

#include "game.h"

#include "textures.h"
#include "input.h"

/*** RENDERING ***/

// TODO: render by layers
void render (SDL_Renderer *renderer, GameManager *game_manager) {

    SDL_RenderClear (renderer);

    if (game_manager->currState->render)
        game_manager->currState->render ();

    SDL_RenderPresent (renderer);

}

/*** SET UP ***/

void sdl_setUp (SDL_Window **window, SDL_Renderer **renderer) {

    SDL_Init (SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    *window = SDL_CreateWindow ("Annihilation",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

    *renderer = SDL_CreateRenderer (*window, 0, SDL_RENDERER_SOFTWARE | SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor (*renderer, 0, 0, 0, 255);
    SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, "0");
    SDL_RenderSetLogicalSize (*renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

}

/*** CLEAN UP ***/

void cleanUp (SDL_Window *window, SDL_Renderer *renderer) {

    game_cleanUp ();

    SDL_DestroyRenderer (renderer);
    SDL_DestroyWindow (window);

    SDL_Quit ();

}

/*** MAIN THREAD ***/

bool running = false;
bool inGame = false;
bool wasInGame = false;

void quit (void) {

    running = false;
    inGame = false;

}

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

int main (void) {

    srand ((unsigned) time (NULL));

    sdl_setUp (&window, &renderer);

    SDL_Event event;

    game_state = game_state_new ();
    game_manager = game_manager_new (game_state);

    u32 timePerFrame = 1000 / FPS_LIMIT;
    u32 frameStart;
    i32 sleepTime;

    running = true;
    while (running) {
        frameStart = SDL_GetTicks ();
        
        // TODO: create a thread to handle events?
        input_handle (event);

        // TODO: create a separte thread
        if (game_manager->currState->update)
            game_manager->currState->update ();

        render (renderer, game_manager);

        // limit the FPS
        sleepTime = timePerFrame - (SDL_GetTicks () - frameStart);
        if (sleepTime > 0) SDL_Delay (sleepTime);
    }

    cleanUp (window, renderer);

    return 0;

}