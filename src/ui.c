#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "annihilation.h"
#include "ui.h"

#include "engine/renderer.h"

#include "utils/log.h"

static const char *fontPath = "./assets/fonts/Roboto-Regular.ttf";

static TTF_Font *font = NULL;

u8 ui_init (void) {

    // init fonts
    TTF_Init ();
    font = TTF_OpenFont (fontPath, 32);
    if (!font) {
        logMsg (stderr, ERROR, NO_TYPE, "Failed to load font!");
        return 1;
    }

    return 0;

}

#pragma region TEXTBOX

// extern TextBox *ui_textBox_create (u8 x, u8 y, u8 w, u8 h, u32 bgcolor, 
//     const char *text, bool password, u32 textColor);

// SDL_Texture *texture;
// UIRect *bgrect;
// u32 bgcolor;

// u32 textColor;
// char *text;
// bool ispassword;
// char *pswd;

// FIXME: handle colors as rgba
// FIXME: handle password logic
TextBox *ui_textBox_create (u32 x, u32 y, const char *text, u32 textColor, bool isPassword) {

    TextBox *textBox = (TextBox *) malloc (sizeof (TextBox));
    if (textBox) {
        int text_width;
        int text_height;
        SDL_Color color = {255, 255, 255, 0};
        SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
        
        textBox->texture = SDL_CreateTextureFromSurface (main_renderer, surface);

        text_width = surface->w;
        text_height = surface->h;

        textBox->bgrect.x = x;
        textBox->bgrect.y = y;
        textBox->bgrect.w = text_width;
        textBox->bgrect.h = text_height;

        SDL_FreeSurface(surface);
    }

    return textBox;

}


// extern void ui_textBox_destroy (TextBox *textbox);
// extern void ui_textBox_setBorders (TextBox *textbox, u8 borderWidth, u32 borderColor);
// extern void ui_textBox_update_text (TextBox *textbox, const char *text);
// extern void ui_textbox_delete_text (TextBox *textbox);
// extern void ui_textBox_draw (Console *console, TextBox *textbox);

#pragma endregion