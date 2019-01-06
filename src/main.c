#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include <SDL2/SDL.h>

#include "annihilation.h"

#include "game.h"
#include "ui.h"

#include "engine/renderer.h"
#include "engine/input.h"

#include "utils/myUtils.h"

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

    ui_init ();

    u32 timePerFrame = 1000 / FPS_LIMIT;
    u32 frameStart = 0;
    i32 sleepTime = 0;
    float deltaTime = 0;

    u32 deltaTicks = 0;
    u32 fps = 0;

    // #ifdef DEV
    TextBox *static_text = ui_textBox_create_static (100, 100, ui_rgba_color_create (100, 45, 67, 255),
        "this is a static text!", RGBA_WHITE, NULL, false);
    TextBox *volatile_text = ui_textBox_create_volatile (200, 200, ui_rgba_color_create (100, 45, 67, 255),
        "this is a volatile text!", RGBA_WHITE, NULL, false);
    // #endif

    #ifdef DEV
    TextBox *fpsText = ui_textBox_create_volatile (500, 500, RGBA_NO_COLOR, "Fps: 0", RGBA_WHITE, NULL, false);
    #endif

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

        deltaTime = SDL_GetTicks () - frameStart;
        deltaTicks += deltaTime;
        fps++;
        if (deltaTicks > 1000) {
            // printf ("fps: %i\n", fps);
            #ifdef DEV
            ui_textBox_set_text (fpsText, createString ("FPS: %i", fps));
            #endif
            deltaTicks = 0;
            fps = 0;
        }
            
    }

    // FIXME: i dont want this here!
    game_cleanUp ();
    ui_destroy ();
    video_destroy_main ();
    SDL_Quit ();

    return 0;

}