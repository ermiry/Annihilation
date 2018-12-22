#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include <SDL2/SDL.h>

#include "annihilation.h"

/*** RENDERING ***/

void render (SDL_Renderer *renderer) {

    // SDL_SetRenderDrawColor (renderer, 0, 0, 0, 255);
    SDL_RenderClear (renderer);
    SDL_RenderPresent(renderer);

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

    running = true;
    while (running) {
        frameStart = SDL_GetTicks ();
        
        while (SDL_PollEvent (&event) != 0) {
            if (event.type == SDL_QUIT) {
                running = false;
                inGame = false;
            } 
        }

        render (renderer);

        // limit the FPS
        sleepTime = timePerFrame - (SDL_GetTicks () - frameStart);
        if (sleepTime > 0) SDL_Delay (sleepTime);
    }

    cleanUp (window, renderer);

    return 0;

}