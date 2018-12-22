#ifndef TEXTURES_H
#define TEXTURES_H

#include <SDL2/SDL.h>

#include "annihilation.h"

extern SDL_Texture *texture_load (const char *filename, SDL_Renderer *renderer);
extern void texture_get_dimensions (SDL_Texture *texture, u32 *w, u32 *h);

#endif