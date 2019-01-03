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

#pragma region FONT 

static FontMap *ui_font_map_create (u32 n_buckets) {

    FontMap *fontMap = (FontMap *) malloc (sizeof (FontMap));
    if (fontMap) {
        fontMap->n_buckets = n_buckets;
        fontMap->buckets = (FontMapNode **) calloc (n_buckets, sizeof (FontMapNode *));
        for (u32 i = 0; i < n_buckets; i++) fontMap->buckets[i] = NULL;
    }

    return fontMap;

}

static void ui_font_map_destroy (FontMap *fontMap) {

    if (fontMap) {
        FontMapNode *node = NULL, *last = NULL;;
        for (u32 i = 0; i < fontMap->n_buckets; i++) {
            node = fontMap->buckets[i];
            if (node) {
                last = node;
                node = node->next;
                free (last);
            }
        }

        free (fontMap->buckets);
        free (fontMap);
    }

}

static void ui_font_init (Font *font) {

    font->ttf_source = NULL;
    font->owns_ttf_source = 0;

    font->filter = FILTER_NEAREST;

    font->default_color.r = font->default_color.g = font->default_color.b = 0;
    font->default_color.a = 255;

    font->height = 0;
    font->maxWidth = 0;
    font->baseline = 0;
    font->ascent = 0;
    font->descent = 0;

    font->lineSpacing = 0;
    font->letterSpacing = 0;

    font->last_glyph.rect.x = font->last_glyph.rect.y = 1;
    font->last_glyph.rect.w = font->last_glyph.rect.h = 0;
    font->last_glyph.cacheLevel = 0;

    if (font->glyphs) ui_font_map_destroy (font->glyphs);
    font->glyphs = ui_font_map_create (DEFAULT_FONT_MAP_N_BUCKETS);

    font->glyph_cache_size = 3;
    font->glyph_cache_count = 0;
    font->glyph_cache = (FontImage **) calloc (font->glyph_cache_size, sizeof (FontImage *));

    font->loading_string = NULL;

}

Font *ui_font_create (void) {

    Font *font = (Font *) malloc (sizeof (Font));
    if (font) {
        memset (font, 0, sizeof (Font));
        ui_font_init (font);
    } 

    return font;

}

#pragma endregion

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