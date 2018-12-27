#include <SDL2/SDL.h>

#include "game.h"

SDL_Renderer *renderer = NULL;

// TODO: render by layers
void render (void) {

    SDL_RenderClear (renderer);

    if (game_manager->currState->render)
        game_manager->currState->render ();

    SDL_RenderPresent (renderer);

}