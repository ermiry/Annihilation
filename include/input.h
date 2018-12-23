#ifndef INPUT_H
#define INPUT_H

#define N_MOUSE_BUTTONS     3

typedef enum MouseButton {

    MOUSE_LEFT = 0,
    MOUSE_MIDDLE = 1,
    MOUSE_RIGHT = 2

} MouseButton;

extern void input_init (void);
extern void input_handle (SDL_Event event);

#endif