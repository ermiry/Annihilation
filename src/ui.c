#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "annihilation.h"
#include "ui.h"

#include "engine/renderer.h"

#include "utils/log.h"
#include "utils/myUtils.h"

/*** COMMON RGBA COLORS ***/

RGBA_Color RGBA_WHITE = { 255, 255, 255, 255 };

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

/*** MISC ***/

static int u8_charsize (const char *character) {

    if (!character) return 0;

    if((unsigned char) *character <= 0x7F) return 1;
    else if((unsigned char) *character < 0xE0) return 2;
    else if((unsigned char) *character < 0xF0) return 3;
    else return 4;

    return 1;
}

static inline const char *u8_next (const char *string) {

    return string + u8_charsize (string);

}

static int u8_charcpy (char *buffer, const char *source, int buffer_size) {

    if(buffer == NULL || source == NULL || buffer_size < 1)
        return 0;

    int charsize = u8_charsize (source);
    if (charsize > buffer_size)
        return 0;

    memcpy (buffer, source, charsize);

    return charsize;

}

static u32 get_code_point_from_UTF8 (const char **c, u8 advancePtr) {

    if (c && *c) {
        u32 retval = 0;
        const char *str = *c;

        if((unsigned char)*str <= 0x7F) retval = *str;
        else if((unsigned char)*str < 0xE0) {
            retval |= (unsigned char)(*str) << 8;
            retval |= (unsigned char)(*(str+1));
            if (advancePtr) *c += 1;
        }

        else if((unsigned char)*str < 0xF0) {
            retval |= (unsigned char)(*str) << 16;
            retval |= (unsigned char)(*(str+1)) << 8;
            retval |= (unsigned char)(*(str+2));
            if (advancePtr) *c += 2;
        }
        else {
            retval |= (unsigned char)(*str) << 24;
            retval |= (unsigned char)(*(str+1)) << 16;
            retval |= (unsigned char)(*(str+2)) << 8;
            retval |= (unsigned char)(*(str+3));
            if (advancePtr) *c += 3;
        }

        return retval;
    }

    return 0;

}

static char *font_get_string_ASCII (void) {

    static char *buffer = NULL;
    if (!buffer) {
        int i;
        char c;
        buffer = (char *) calloc (512, sizeof (char));
        i = 0;
        c = 32;
        while (1) {
            buffer[i] = c;
            if (c == 126)
                break;
            ++i;
            ++c;
        }
    }

    char *retval = (char*) calloc (strlen (buffer) + 1, sizeof (char));
    if (retval) strcpy (retval, buffer);

    return retval;

}

static inline SDL_Surface *font_create_surface (u32 width, u32 height) {

    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        return SDL_CreateRGBSurface (SDL_SWSURFACE, width, height, 32, 
            0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
    #else
        return SDL_CreateRGBSurface (SDL_SWSURFACE, width, height, 32, 
            0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
    #endif

}

/*** FONT MAP ***/

static FontMap *font_map_create (u32 n_buckets) {

    FontMap *fontMap = (FontMap *) malloc (sizeof (FontMap));
    if (fontMap) {
        fontMap->n_buckets = n_buckets;
        fontMap->buckets = (FontMapNode **) calloc (n_buckets, sizeof (FontMapNode *));
        for (u32 i = 0; i < n_buckets; i++) fontMap->buckets[i] = NULL;
    }

    return fontMap;

}

static void font_map_destroy (FontMap *fontMap) {

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

static GlyphData *font_map_insert (FontMap *map, u32 codepoint, GlyphData glyph) {

    if (map) {
        FontMapNode *node = NULL;
        u32 index = codepoint % map->n_buckets;

        if (!map->buckets[index]) {
            node = map->buckets[index] = (FontMapNode *) malloc (sizeof (FontMapNode));
            node->key = codepoint;
            node->value = glyph;
            node->next = NULL;
            return &node->value;
        }

        for(node = map->buckets[index]; node != NULL; node = node->next) {
            if(!node->next) {
                node->next = (FontMapNode*) malloc (sizeof (FontMapNode));
                node = node->next;

                node->key = codepoint;
                node->value = glyph;
                node->next = NULL;
                return &node->value;
            }
        }
    }

    return NULL;

}

/*** GLYPH ***/

static GlyphData glyph_data_make (int cacheLevel, i16 x, i16 y, u16 w, u16 h) {

    GlyphData gd;

    gd.rect.x = x;
    gd.rect.y = y;
    gd.rect.w = w;
    gd.rect.h = h;
    gd.cacheLevel = cacheLevel;

    return gd;

}

static GlyphData *glyph_data_pack (Font *font, u32 codepoint, u16 width, 
    u16 maxWidth, u16 maxHeight) {

    FontMap *glyphs = font->glyphs;
    GlyphData *last_glyph = &font->last_glyph;
    u16 height = font->height + 1;

    if (last_glyph->rect.x + last_glyph->rect.w + width >= maxWidth - 1) {
        if(last_glyph->rect.y + height + height >= maxHeight - 1) {
            last_glyph->cacheLevel = font->glyph_cache_count;
            last_glyph->rect.x = 1;
            last_glyph->rect.y = 1;
            last_glyph->rect.w = 0;
            return NULL;
        }

        else {
            last_glyph->rect.x = 1;
            last_glyph->rect.y += height;
            last_glyph->rect.w = 0;
        }
    }

    // move to next space
    last_glyph->rect.x += last_glyph->rect.w + 1 + 1;
    last_glyph->rect.w = width;

    return font_map_insert (glyphs, codepoint, 
        glyph_data_make (last_glyph->cacheLevel, last_glyph->rect.x, last_glyph->rect.y,
                        last_glyph->rect.w, last_glyph->rect.h));
}

/*** FONT ***/

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

    if (font->glyphs) font_map_destroy (font->glyphs);
    font->glyphs = font_map_create (DEFAULT_FONT_MAP_N_BUCKETS);

    font->glyph_cache_size = 3;
    font->glyph_cache_count = 0;
    font->glyph_cache = (FontImage **) calloc (font->glyph_cache_size, sizeof (FontImage *));

    font->loading_string = font_get_string_ASCII ();

}

Font *ui_font_create (void) {

    Font *font = (Font *) malloc (sizeof (Font));
    if (font) {
        memset (font, 0, sizeof (Font));
        ui_font_init (font);
    } 

    return font;

}

// TODO:
void ui_font_destroy (Font *font) {}

static u8 ui_font_load_from_ttf (Font *font, TTF_Font *ttf, RGBA_Color color) {

    // TODO: do we need to clear up the font?
    // FC_ClearFont(font);

    SDL_RendererInfo info;
    SDL_GetRendererInfo (main_renderer, &info);
    u8 has_render_target_support = (info.flags & SDL_RENDERER_TARGETTEXTURE);

    font->ttf_source = ttf;

    font->height = TTF_FontHeight(ttf);
    font->ascent = TTF_FontAscent(ttf);
    font->descent = -TTF_FontDescent(ttf);
    
    // Some bug for certain fonts can result in an incorrect height.
    if (font->height < font->ascent - font->descent)
        font->height = font->ascent - font->descent;

    font->baseline = font->height - font->descent;

    font->default_color = color;

    SDL_Surface *glyph_surf = NULL;
    char buff[5];
    memset (buff, 0, 5);
    const char *buff_ptr = buff;
    const char *source_string = font->loading_string;
    u8 packed = 0;

    u32 w = font->height * 12;
    u32 h = font->height * 12;

    SDL_Surface *surfaces[FONT_LOAD_MAX_SURFACES];
    int num_surfaces = 1;
    surfaces[0] = font_create_surface (w, h);

    font->last_glyph.rect.x = font->last_glyph.rect.y = 1;
    font->last_glyph.rect.w = 0;
    font->last_glyph.rect.h = font->height;

    for (; *source_string != '\0'; source_string = u8_next (source_string)) {
        if (!u8_charcpy (buff, source_string, 5)) continue;

        glyph_surf = TTF_RenderUTF8_Blended (ttf, buff, RGBA_WHITE);
        if (!glyph_surf) continue;

        packed = (glyph_data_pack (font, get_code_point_from_UTF8 (&buff_ptr, 0),
            glyph_surf->w, surfaces[num_surfaces - 1]->w, surfaces[num_surfaces-1]->h) != NULL);

        // Try packing.  If it fails, create a new surface for the next cache level.
        // packed = (FC_PackGlyphData(font, 
        //     FC_GetCodepointFromUTF8(&buff_ptr, 0), glyph_surf->w, 
        //     surfaces[num_surfaces-1]->w, surfaces[num_surfaces-1]->h) != NULL);
        // if (!packed) {

        // }
    }

    // FIXME:
    // int i = num_surfaces-1;
    // FC_UploadGlyphCache(font, i, surfaces[i]);
    // SDL_FreeSurface(surfaces[i]);
    // SDL_SetTextureBlendMode(font->glyph_cache[i], SDL_BLENDMODE_BLEND);

    // FIXME: retval
    return 1;   // error

}

static u8 ui_font_load_rw (Font *font, SDL_RWops *file_rwops_ttf,
    u8 own_rwops, u32 pointSize, RGBA_Color color, int style) {

    u8 retval, outline;

    TTF_Font *ttf = TTF_OpenFontRW (file_rwops_ttf, own_rwops, pointSize);
    if (!ttf) {
        logMsg (stderr, ERROR, NO_TYPE, "Unable to load ttf!");
        return 1;
    }

    outline = (style & TTF_STYLE_OUTLINE);
    if (outline) {
        style &= ~TTF_STYLE_OUTLINE;
        TTF_SetFontOutline (ttf, 1);
    }

    TTF_SetFontStyle (ttf, style);

    retval = ui_font_load_from_ttf (font, ttf, color);

    font->owns_ttf_source = own_rwops;
    if (!own_rwops) {
        TTF_CloseFont (font->ttf_source);
        font->ttf_source = NULL;
    }

    return retval;

}

u8 ui_font_load (Font *font, const char *filename, u32 pointSize, 
    RGBA_Color color, int style) {

    if (font) {
        SDL_RWops *rwops = SDL_RWFromFile (filename, "rb");
        if (rwops) return ui_font_load_rw (font, rwops, 1, pointSize, color, style);

        else logMsg (stderr, ERROR, NO_TYPE, 
            createString ("Unable to open file for reading: %s", filename));
    }

    return 1;

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