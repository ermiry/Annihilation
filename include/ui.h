#ifndef UI_H
#define UI_H

#include "annihilation.h"

/*** COMMON COLORS ***/

#define NO_COLOR        0x00000000
#define WHITE           0xFFFFFFFF
#define BLACK           0x000000FF

#define FULL_GREEN      0x00FF00FF
#define FULL_RED        0xFF0000FF

#define YELLOW          0xFFD32AFF
#define SAPPHIRE        0x1E3799FF

#define SILVER          0xBDC3C7FF

/*** UI ***/

typedef SDL_Rect UIRect;

/*** TEXT/FONTS ***/

#define DEFAULT_FONT_SIZE       24

typedef struct TextBox {

    SDL_Texture *texture;
    UIRect bgrect;
    u32 bgcolor;

    u32 textColor;
    char *text;
    bool ispassword;
    char *pswd;

    // TODO:
    // u8 borderWidth;
    // u32 borderColor;

} TextBox;

extern TextBox *ui_textBox_create (u32 x, u32 y, const char *text, u32 textColor, bool isPassword);

// extern TextBox *ui_textBox_create (u8 x, u8 y, u8 w, u8 h, u32 bgcolor, 
//     const char *text, bool password, u32 textColor);
// extern void ui_textBox_destroy (TextBox *textbox);
// extern void ui_textBox_setBorders (TextBox *textbox, u8 borderWidth, u32 borderColor);
// extern void ui_textBox_update_text (TextBox *textbox, const char *text);
// extern void ui_textbox_delete_text (TextBox *textbox);
// extern void ui_textBox_draw (Console *console, TextBox *textbox);

/*** PUBLIC UI FUNCS ***/

extern u8 ui_init (void);

#endif