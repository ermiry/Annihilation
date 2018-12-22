#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "annihilation.h"

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