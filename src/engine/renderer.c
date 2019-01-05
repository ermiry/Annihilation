#include <SDL2/SDL.h>

#include "game.h"

SDL_Window *main_window = NULL;
SDL_Renderer *main_renderer = NULL;

// FIXME: better render the ui
#include "ui.h"
extern TextBox *fpsText;

// TODO: render by layers
void render (void) {

    SDL_RenderClear (main_renderer);

    if (game_manager->currState->render)
        game_manager->currState->render ();

    // SDL_RenderCopy (main_renderer, fpsText->texture, NULL, &fpsText->bgrect);

    SDL_RenderPresent (main_renderer);

}

static void render_init_main (void) {

    main_renderer = SDL_CreateRenderer (main_window, 0, SDL_RENDERER_SOFTWARE | SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor (main_renderer, 0, 0, 0, 255);
    SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, "0");
    SDL_RenderSetLogicalSize (main_renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

}

static void window_init_main (const char *title) {

    main_window = SDL_CreateWindow (title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

}

void video_init_main (const char *title) {

    window_init_main (title);
    render_init_main ();

}

void video_destroy_main (void) {

    SDL_DestroyRenderer (main_renderer);
    SDL_DestroyWindow (main_window);

}