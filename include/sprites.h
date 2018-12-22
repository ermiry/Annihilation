#ifndef SPRITES_H
#define SPRITES_H

#include <SDL2/SDL.h>

#include "annihilation.h"

typedef struct Sprite {

    u32 col, row;

} Sprite;

typedef struct SpriteSheet {

    u32 w, h;
    SDL_Texture *texture;

    SDL_Rect src_rect, dest_rect;

    u32 sprite_w, sprite_h;

    i32 scaleFactor;

    Sprite ***individualSprites;

} SpriteSheet;

extern void sprite_sheet_destroy (SpriteSheet *spriteSheet);
extern SpriteSheet *sprite_sheet_load (const char *filename, SDL_Renderer *renderer);
extern void sprite_sheet_set_sprite_size (SpriteSheet *spriteSheet, u32 w, u32 h);
extern void sprite_sheet_set_scale_factor (SpriteSheet *spriteSheet, i32 scaleFactor);
extern void sprite_sheet_crop (SpriteSheet *spriteSheet);

#endif