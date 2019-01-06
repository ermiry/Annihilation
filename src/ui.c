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

/*** UI ELEMENTS ***/

#pragma region UI elements

static inline UIRect ui_rect_create (u32 x, u32 y, u32 w, u32 h) {

    UIRect ret = { x, y, w, h };
    return ret;

}

static inline UIRect ui_rect_union (UIRect a, UIRect b) {

    u32 x1 = MIN (a.x, b.x);
    u32 y1 = MIN (a.y, b.y);
    u32 x2 = MAX (a.x + a.w, b.x + b.w);
    u32 y2 = MAX (a.y + a.h, b.y + b.h);

    UIRect retval = { x1, y1, MAX (0, x2 - x1), MAX (0, y2 - y1) };
    return retval;

}

#pragma endregion

#pragma region FONT 

static const char *mainFontPath = "./assets/fonts/Roboto-Regular.ttf";
Font *mainFont = NULL;

static u8 has_render_target_support = 0;

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
            retval |= (unsigned char)(*(str + 1));
            if (advancePtr) *c += 1;
        }

        else if((unsigned char)*str < 0xF0) {
            retval |= (unsigned char)(*str) << 16;
            retval |= (unsigned char)(*(str + 1)) << 8;
            retval |= (unsigned char)(*(str + 2));
            if (advancePtr) *c += 2;
        }
        else {
            retval |= (unsigned char)(*str) << 24;
            retval |= (unsigned char)(*(str + 1)) << 16;
            retval |= (unsigned char)(*(str + 2)) << 8;
            retval |= (unsigned char)(*(str + 3));
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

static GlyphData *font_map_find (FontMap *fontMap, u32 codepoint) {

    if (fontMap) {
        FontMapNode *node = NULL;
        u32 index = codepoint % fontMap->n_buckets;

        for (node = fontMap->buckets[index]; node != NULL; node = node->next)
            if (node->key == codepoint)
                return &node->value;
    }

    return NULL;    

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

static u8 glyph_get_data (Font *font, GlyphData *result, u32 codepoint) {

    GlyphData *g = font_map_find (font->glyphs, codepoint);
    if (g) {
        *result = *g;
        return 0;   // success
    } 

    // TODO: maybe add the new glyph data to the font cache? check draft code

    return 1;   // error

}


static u8 glyph_set_cache_level (Font* font, int cache_level, SDL_Texture *cache_texture) {

    if (font && cache_level >= 0) {
        if (cache_level <= font->glyph_cache_count + 1) {
            if (cache_level == font->glyph_cache_count) {
                font->glyph_cache_count++;

                if (font->glyph_cache_count > font->glyph_cache_size) {
                    font->glyph_cache = 
                        (SDL_Texture **) realloc (font->glyph_cache, sizeof (font->glyph_cache_count));
                    font->glyph_cache_size = font->glyph_cache_count;
                }
            }

            font->glyph_cache[cache_level] = cache_texture;

            return 0;
        }
    }

    return 1;   // error

}

static inline SDL_Texture *glyph_get_cache_level (Font *font, int cacheLevel) {

    if (font && cacheLevel >= 0 && cacheLevel >= font->glyph_cache_count)
        return font->glyph_cache[cacheLevel];

}

static u8 glyph_upload_cache (Font *font, int cacheLevel, SDL_Surface *dataSurface) {

    if (font && dataSurface) {
        SDL_Texture *new_level = NULL;

        if (has_render_target_support) {
            char old_filter_mode[16];  
            snprintf(old_filter_mode, 16, "%s", SDL_GetHint (SDL_HINT_RENDER_SCALE_QUALITY));

            if (font->filter == FILTER_LINEAR) SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, "1");
            else SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, "0");
            
            new_level = SDL_CreateTexture (main_renderer, dataSurface->format->format, 
                SDL_TEXTUREACCESS_TARGET, dataSurface->w, dataSurface->h);;

            SDL_SetTextureBlendMode (new_level, SDL_BLENDMODE_BLEND);

            // reset filter mode for the temp texture
            SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, "0");

            u8 r, g, b, a;
            SDL_Texture *temp = SDL_CreateTextureFromSurface (main_renderer, dataSurface);
            SDL_SetTextureBlendMode (temp, SDL_BLENDMODE_NONE);
            SDL_SetRenderTarget (main_renderer, new_level);

            SDL_GetRenderDrawColor (main_renderer, &r, &g, &b, &a);
            SDL_SetRenderDrawColor (main_renderer, 0, 0, 0, 0);
            SDL_RenderClear (main_renderer);
            SDL_SetRenderDrawColor (main_renderer, r, g, b, a);

            SDL_RenderCopy (main_renderer, temp, NULL, NULL);
            SDL_SetRenderTarget (main_renderer, NULL);

            SDL_DestroyTexture (temp);

            SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, old_filter_mode);
        }

        if (!glyph_set_cache_level (font, cacheLevel, new_level)) return 0;   // success
        
        #ifdef DEV
        logMsg (stderr, ERROR, NO_TYPE, "Font cache ran out of packing space "
        "and could not add another cache level!");
        #else
        SDL_DestroyTexture (new_level);
        #endif
    }

    return 1;   // error

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

void ui_font_destroy (Font *font) {

    if (font) {
        if (font->owns_ttf_source) TTF_CloseFont (font->ttf_source);

        font_map_destroy (font->glyphs);

        // delete glyph cache
        if (font->glyph_cache) {
            for (u32 i = 0; i < font->glyph_cache_count; i++) 
                if (font->glyph_cache[i])
                    SDL_DestroyTexture (font->glyph_cache[i]);

            free (font->glyph_cache);
        }

        if (font->loading_string) free (font->loading_string);

        free (font);
    }

}

static u8 ui_font_load_from_ttf (Font *font, TTF_Font *ttf, RGBA_Color color) {

    // TODO: do we need to clear up the font?
    // FC_ClearFont(font);

    SDL_RendererInfo info;
    SDL_GetRendererInfo (main_renderer, &info);
    has_render_target_support = (info.flags & SDL_RENDERER_TARGETTEXTURE);

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
        if (!packed) {
            int i = num_surfaces - 1;
            if (num_surfaces >= FONT_LOAD_MAX_SURFACES) {
                // FIXME: better handle this error - also set a retval
                #ifdef DEV
                logMsg (stderr, ERROR, NO_TYPE, "Font cache error - Could not create"
                "enough cache surfaces to fit all of the loading string!");
                #else
                logMsg (stderr, ERROR, NO_TYPE, "Failed to create font cache!");
                #endif
                SDL_FreeSurface (glyph_surf);
                break;
            }

            // upload the current surface to the glyph cache
            glyph_upload_cache (font, i, surfaces[i]);
            SDL_FreeSurface (surfaces[i]);
            font->last_glyph.cacheLevel = num_surfaces;

            surfaces[num_surfaces] = font_create_surface (w, h);
            num_surfaces++;
        }

        if (packed || glyph_data_pack (font, get_code_point_from_UTF8 (&buff_ptr, 0),
            glyph_surf->w, surfaces[num_surfaces - 1]->w, surfaces[num_surfaces - 1]->h) != NULL) {
            SDL_SetSurfaceBlendMode (glyph_surf, SDL_BLENDMODE_NONE);
            SDL_Rect srcRect = { 0, 0, glyph_surf->w, glyph_surf->h };
            SDL_Rect destRect = font->last_glyph.rect;
            SDL_BlitSurface (glyph_surf, &srcRect, surfaces[num_surfaces - 1], &destRect);
        }

        SDL_FreeSurface (glyph_surf);
    }

    int n = num_surfaces - 1;
    glyph_upload_cache (font, n, surfaces[n]);
    SDL_FreeSurface (surfaces[n]);
    SDL_SetTextureBlendMode (font->glyph_cache[n], SDL_BLENDMODE_BLEND);

    return 0;

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

// TODO: add seters for all the textbox elements

// TODO: add bg color
// FIXME: handle password logic
TextBox *ui_textBox_create (u32 x, u32 y, const char *text, RGBA_Color textColor, Font *font,
    bool isPassword) {

    TextBox *textBox = (TextBox *) malloc (sizeof (TextBox));
    if (textBox) {
        textBox->texture = NULL;
        textBox->font = font;

        textBox->bgrect.x = x;
        textBox->bgrect.y = y;
        textBox->bgrect.w = textBox->bgrect.h = 0;

        textBox->textColor = textColor;
        if (text) {
            textBox->text = (char *) calloc (strlen (text) + 1, sizeof (char));
            strcpy (textBox->text, text);
        }

        textBox->ispassword = isPassword;
        textBox->pswd = NULL;
    }

    return textBox;

}

// FIXME: create a function to just render the textbox and another to change the text 
// we don't want to be generating surfaces each time... that is the point of all of these!!

// FC_Default_RenderCallback
UIRect ui_rect_render (SDL_Texture *srcTexture, UIRect *srcRect, u32 x, u32 y) {

    UIRect retval;

    SDL_RendererFlip flip = SDL_FLIP_NONE;
    UIRect r = *srcRect;
    UIRect dr = { x, y, r.w, r.h };
    SDL_RenderCopyEx (main_renderer, srcTexture, &r, &dr, 0, NULL, flip);

    retval.x = x;
    retval.y = y;
    retval.w = srcRect->w;
    retval.h = srcRect->h;

    return retval;

}

// TODO: handle flip
// FIXME: handle password logic
// TODO: maybe add scale
// this was FC_RenderLeft...
// FIXME: movw x and y values inside the textbox! -> can they go inside the src rect?
void ui_textbox_draw (TextBox *textbox, u32 x, u32 y) {

    if (textbox) {
        const char *c = textbox->text;

        // TODO: do we want this inside the textbox?
        UIRect srcRect, destRect, dirtyRect = ui_rect_create (x, y, 0, 0);

        GlyphData glyph;
        u32 codepoint;

        u32 destX = x;
        u32 destY = y;
        float destH, destLineSpacing, destLetterSpacing;

        // TODO: add scale here!
        destH = textbox->font->height;
        destLineSpacing = textbox->font->lineSpacing;
        destLetterSpacing = textbox->font->letterSpacing;

        int newLineX = x;

        for (; *c != '\0'; c++) {
            if (*c == '\n') {
                destX = newLineX;
                destY += destH + destLineSpacing;
                continue;
            }

            codepoint = get_code_point_from_UTF8 (&c, 1);
            if (glyph_get_data (textbox->font, &glyph, codepoint)) {
                // FIXME: handle bas caharcters
            }

            if (codepoint == ' ') {
                destX += glyph.rect.w + destLetterSpacing;
                continue;
            }

            srcRect = glyph.rect;

            destRect = ui_rect_render (glyph_get_cache_level (textbox->font, glyph.cacheLevel),
                &srcRect, destX, destY);

            if (dirtyRect.w == 0 || dirtyRect.h == 0) dirtyRect = destRect;
            else dirtyRect = ui_rect_union (dirtyRect, destRect);

            destX += glyph.rect.w + destLetterSpacing;

            // FIXME:
            // return dirtyRect;
        }
    }

}


// extern void ui_textBox_destroy (TextBox *textbox);
// extern void ui_textBox_setBorders (TextBox *textbox, u8 borderWidth, u32 borderColor);
// extern void ui_textBox_update_text (TextBox *textbox, const char *text);
// extern void ui_textbox_delete_text (TextBox *textbox);
// extern void ui_textBox_draw (Console *console, TextBox *textbox);

#pragma endregion

/*** PUBLIC UI FUNCS ***/

u8 ui_init (void) {

    // init and load fonts
    TTF_Init ();
    mainFont = ui_font_create ();
    if (!mainFont) {
        #ifdef DEV
        logMsg (stderr, ERROR, NO_TYPE, "Failed to allocate space for new font!");
        #endif
        return 1;
    }

    if (ui_font_load (mainFont, mainFontPath, DEFAULT_FONT_SIZE, RGBA_WHITE, TTF_STYLE_NORMAL)) {
        #ifdef DEV
        logMsg (stderr, ERROR, NO_TYPE, "Failed to load font and create font cache!");
        #endif
        return 1;
    }

    return 0;   // success

}