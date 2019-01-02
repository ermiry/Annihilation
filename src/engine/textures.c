#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "annihilation.h"

#include "engine/sprites.h"

/*** TEXTURE MANAGER ***/

SDL_Texture *texture_load (const char *filename, SDL_Renderer *renderer) {

    if (filename && renderer) {
        SDL_Surface *tmpSurface = IMG_Load (filename);
        SDL_Texture *texture = SDL_CreateTextureFromSurface (renderer, tmpSurface);
        SDL_FreeSurface (tmpSurface);

        return texture;
    }

    return NULL;

}

void texture_get_dimensions (SDL_Texture *texture, u32 *w, u32 *h) {

    if (texture) SDL_QueryTexture (texture, NULL, NULL, w, h);

}

#include "camera.h"

void texture_draw (SDL_Renderer *renderer, Sprite *sprite, i32 x, i32 y, SDL_RendererFlip flip) {

    if (renderer && sprite) {
        sprite->dest_rect.x = x;
        sprite->dest_rect.y = y;

        SDL_RenderCopyEx (renderer, sprite->texture, &sprite->src_rect, &sprite->dest_rect, 
            0, 0, flip);
    }

}

void texture_draw_frame (SDL_Renderer *renderer, SpriteSheet *spriteSheet, 
    i32 x, i32 y, u32 col, u32 row, SDL_RendererFlip flip) {

    if (renderer && spriteSheet) {
        spriteSheet->src_rect.x = spriteSheet->sprite_w * col;
        spriteSheet->src_rect.y = spriteSheet->sprite_h * row;

        spriteSheet->dest_rect.x = x;
        spriteSheet->dest_rect.y = y;

        SDL_RenderCopyEx (renderer, spriteSheet->texture, &spriteSheet->src_rect, &spriteSheet->dest_rect,
            0, 0, flip);
    }

}