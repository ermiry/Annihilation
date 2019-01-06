#ifndef UI_H
#define UI_H

#include <SDL2/SDL.h>

#include "annihilation.h"

/*** COMMON HEX COLORS ***/

#define HEX_NO_COLOR        0x00000000
#define HEX_WHITE           0xFFFFFFFF
#define HEX_BLACK           0x000000FF

#define HEX_FULL_GREEN      0x00FF00FF
#define HEX_FULL_RED        0xFF0000FF

#define HEX_YELLOW          0xFFD32AFF
#define HEX_SAPPHIRE        0x1E3799FF

#define HEX_SILVER          0xBDC3C7FF

/*** COMMON RGBA COLORS ***/

typedef SDL_Color RGBA_Color;

extern RGBA_Color RGBA_WHITE;

/*** UI ELEMENTS ***/

typedef SDL_Rect UIRect;

/*** FONTS/TEXT ***/

#pragma region FONT-TEXT

#include <SDL2/SDL_ttf.h>

typedef SDL_Texture FontImage;

#define DEFAULT_FONT_SIZE       24

#define TTF_STYLE_OUTLINE	        16
#define FONT_LOAD_MAX_SURFACES      10

typedef enum FilterEnum {

    FILTER_NEAREST,
    FILTER_LINEAR,

} FilterEnum;

typedef struct GlyphData {

    UIRect rect;
    int cacheLevel;

} GlyphData;

typedef struct FontMapNode {

    u32 key;
    GlyphData value;
    struct FontMapNode *next;

} FontMapNode;

#define DEFAULT_FONT_MAP_N_BUCKETS      300

typedef struct FontMap {

    i32 n_buckets;
    FontMapNode **buckets;

} FontMap;

typedef struct Font {

    TTF_Font *ttf_source;
    u8 owns_ttf_source;

    FilterEnum filter;

    RGBA_Color default_color;
    u16 height;
    u16 maxWidth;
    u16 baseline;
    i32 ascent;
    i32 descent;

    i32 lineSpacing;
    i32 letterSpacing;

    FontMap *glyphs;
    GlyphData last_glyph;
    i32 glyph_cache_size;
    i32 glyph_cache_count;
    FontImage **glyph_cache;
    char *loading_string;

} Font;

extern Font *ui_font_create (void);
extern void ui_font_destroy (Font *font);

/*** TEXTBOX ***/

typedef struct TextBox {

    Font *font;
    SDL_Texture *texture;
    UIRect bgrect;
    u32 bgcolor;

    RGBA_Color textColor;
    char *text;
    bool ispassword;
    char *pswd;

    // TODO:
    // u8 borderWidth;
    // u32 borderColor;

} TextBox;

extern TextBox *ui_textBox_create (u32 x, u32 y, const char *text, RGBA_Color textColor, Font *font,
    bool isPassword);
extern void ui_textbox_draw (TextBox *textbox, u32 x, u32 y);

// extern TextBox *ui_textBox_create (u32 x, u32 y, const char *text, u32 textColor, bool isPassword);

// extern TextBox *ui_textBox_create (u8 x, u8 y, u8 w, u8 h, u32 bgcolor, 
//     const char *text, bool password, u32 textColor);
// extern void ui_textBox_destroy (TextBox *textbox);
// extern void ui_textBox_setBorders (TextBox *textbox, u8 borderWidth, u32 borderColor);
// extern void ui_textBox_update_text (TextBox *textbox, const char *text);
// extern void ui_textbox_delete_text (TextBox *textbox);
// extern void ui_textBox_draw (Console *console, TextBox *textbox);

#pragma endregion

/*** PUBLIC UI FUNCS ***/

extern u8 ui_init (void);

#endif