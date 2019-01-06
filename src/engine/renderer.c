#include <SDL2/SDL.h>

#include "game.h"
#include "ui.h"

SDL_Window *main_window = NULL;
SDL_Renderer *main_renderer = NULL;

// TODO: render by layers
void render (void) {

    SDL_RenderClear (main_renderer);

    // render gameobjects
    if (game_manager->currState->render)
        game_manager->currState->render ();

    // render game UI
    for (u32 i = 0; i < curr_max_ui_elements; i++) {
        switch (ui_elements[i]->type) {
            case UI_TEXTBOX: ui_textbox_draw ((TextBox *) ui_elements[i]->element, 100, 100); break;
            case UI_BUTTON: break;

            default: break;
        }
    }

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