#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include <SDL2/SDL.h>

#include "annihilation.h"

#include "game.h"

#include "engine/renderer.h"
#include "engine/input.h"

bool running = false;
bool inGame = false;
bool wasInGame = false;

void quit (void) {

    running = false;
    inGame = false;

}

int main (void) {

    srand ((unsigned) time (NULL));

    SDL_Init (SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    video_init_main ("Annihilation");

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

        render ();

        // limit the FPS
        sleepTime = timePerFrame - (SDL_GetTicks () - frameStart);
        if (sleepTime > 0) SDL_Delay (sleepTime);
    }

    // FIXME: i dont want this here!
    game_cleanUp ();
    video_destroy_main ();
    SDL_Quit ();

    return 0;

}