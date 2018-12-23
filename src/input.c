#include <stdbool.h>

#include <SDL2/SDL.h>

#include "annihilation.h"

#include "input.h"

extern void quit (void);

bool mouseButtonStates[N_MOUSE_BUTTONS];

void input_init (void) {

    for (u8 i = 0; i < N_MOUSE_BUTTONS; i++) mouseButtonStates[i] = false;

}

void input_handle (SDL_Event event) {

    switch (event.type) {
        case SDL_QUIT: quit (); break;

        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT) mouseButtonStates[MOUSE_LEFT] = true;
            else if (event.button.button == SDL_BUTTON_MIDDLE) mouseButtonStates[MOUSE_MIDDLE] = true;
            else if (event.button.button == SDL_BUTTON_RIGHT) mouseButtonStates[MOUSE_RIGHT] = true;
        break;
        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT) mouseButtonStates[MOUSE_LEFT] = false;
            else if (event.button.button == SDL_BUTTON_MIDDLE) mouseButtonStates[MOUSE_MIDDLE] = false;
            else if (event.button.button == SDL_BUTTON_RIGHT) mouseButtonStates[MOUSE_RIGHT] = false;
        break;

        default: break;
    }

}

bool input_get_mouse_button_state (MouseButton button) { return mouseButtonStates[button]; }