#include <stdio.h>
#include <stdbool.h>
#include <time.h>

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

/*** SPRITES MANAGER ***/

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

SpriteSheet *sprite_sheet_load (const char *filename, SDL_Renderer *renderer) {

    if (filename && renderer) {
        SpriteSheet *new_sprite_sheet = (SpriteSheet *) malloc (sizeof (SpriteSheet));
        if (new_sprite_sheet) {
            new_sprite_sheet->texture = texture_load (filename, renderer);
            texture_get_dimensions (new_sprite_sheet->texture, &new_sprite_sheet->w,
                &new_sprite_sheet->h);

            return new_sprite_sheet;
        }
    }

    return NULL;

}

void sprite_sheet_set_sprite_size (SpriteSheet *spriteSheet, u32 w, u32 h) {

    if (spriteSheet) {
        spriteSheet->sprite_w = w;
        spriteSheet->sprite_h = h;
    }

}

void sprite_sheet_set_scale_factor (SpriteSheet *spriteSheet, i32 scaleFactor) {

    if (spriteSheet) spriteSheet->scaleFactor = scaleFactor;

}

void sprite_sheet_crop (SpriteSheet *spriteSheet) {

    if (spriteSheet) {
        u8 x_num_sprites = spriteSheet->w / spriteSheet->sprite_w;
        u8 y_num_sprites = spriteSheet->h / spriteSheet->sprite_h;

        u32 max_sprites_expected = x_num_sprites * y_num_sprites;

        spriteSheet->individualSprites = (Sprite ***) calloc (x_num_sprites, sizeof (Sprite **));
        for (u8 i = 0; i < x_num_sprites; i++)
            spriteSheet->individualSprites[i] = (Sprite **) calloc (y_num_sprites, sizeof (Sprite *));

        for (u8 y = 0; y < y_num_sprites; y++) {
            for (u8 x = 0; x < x_num_sprites; x++) {
                Sprite *new_sprite = (Sprite *) malloc (sizeof (Sprite));
                new_sprite->col = x;
                new_sprite->row = y;
                spriteSheet->individualSprites[x][y] = new_sprite;
            }
        }
    }

}

/*** ANIMATION ***/

#include <stdarg.h>

typedef struct Animation {

    SpriteSheet *spriteSheet;
    Sprite **frames;

    u8 n_frames;
    u32 speed;

} Animation;

Animation *animation_create (SpriteSheet *spriteSheet, u8 n_frames, ...) {

    if (spriteSheet) {
        va_list valist;
        va_start (valist, n_frames);

        Animation *animation = (Animation *) malloc (sizeof (Animation));
        animation->spriteSheet = spriteSheet;
        animation->n_frames = n_frames;
        animation->frames = (Sprite **) calloc (n_frames, sizeof (Sprite *));

        for (u8 i = 0; i < n_frames; i++)
            animation->frames[i] = va_arg (valist, Sprite *);

        va_end (valist);

        return animation;
    }

    return NULL;

}

void animation_set_speed (Animation *animation, u32 speed) {

    if (animation) animation->speed = speed;

}

/*** RENDERING ***/

void render (SDL_Renderer *renderer) {

    // SDL_SetRenderDrawColor (renderer, 0, 0, 0, 255);
    // SDL_RenderClear (renderer);
    // SDL_RenderPresent (renderer);

}

/*** SET UP ***/

void sdl_setUp (SDL_Window **window, SDL_Renderer **renderer) {

    SDL_Init (SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    *window = SDL_CreateWindow ("Annihilation",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

    *renderer = SDL_CreateRenderer (*window, 0, SDL_RENDERER_SOFTWARE | SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor (*renderer, 0, 0, 0, 255);
    SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, "0");
    SDL_RenderSetLogicalSize (*renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

}

/*** CLEAN UP ***/

void cleanUp (SDL_Window *window, SDL_Renderer *renderer) {

    SDL_DestroyRenderer (renderer);
    SDL_DestroyWindow (window);

    SDL_Quit ();

}

/*** MAIN THREAD ***/

bool running = false;
bool inGame = false;
bool wasInGame = false;

int main (void) {

    srand ((unsigned) time (NULL));

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    sdl_setUp (&window, &renderer);

    SDL_Event event;

    u32 timePerFrame = 1000 / FPS_LIMIT;
    u32 frameStart;
    i32 sleepTime;

    SpriteSheet *player_sprite_sheet = sprite_sheet_load ("./assets/adventurer-sprites.png", renderer);
    sprite_sheet_set_sprite_size (player_sprite_sheet, 50, 37);
    sprite_sheet_set_scale_factor (player_sprite_sheet, 4);
    sprite_sheet_crop (player_sprite_sheet);

    Animation *player_idle_anim = animation_create (player_sprite_sheet, 4, player_sprite_sheet->individualSprites[0][0],
        player_sprite_sheet->individualSprites[1][0], player_sprite_sheet->individualSprites[2][0],
        player_sprite_sheet->individualSprites[3][0]);
    animation_set_speed (player_idle_anim, 300);

    // TODO: move all this to the animation init
    player_sprite_sheet->src_rect.w = player_sprite_sheet->sprite_w;
    player_sprite_sheet->src_rect.h = player_sprite_sheet->sprite_h;

    // scale up our player
    player_sprite_sheet->dest_rect.w = player_sprite_sheet->src_rect.w * player_sprite_sheet->scaleFactor;
    player_sprite_sheet->dest_rect.h = player_sprite_sheet->src_rect.h * player_sprite_sheet->scaleFactor;

    player_sprite_sheet->src_rect.x = player_sprite_sheet->dest_rect.x = 0;
    player_sprite_sheet->src_rect.y = player_sprite_sheet->dest_rect.y = 0;

    running = true;
    while (running) {
        frameStart = SDL_GetTicks ();
        
        while (SDL_PollEvent (&event) != 0) {
            if (event.type == SDL_QUIT) {
                running = false;
                inGame = false;
            } 
        }

        // select player animation
        int currFrame = (int) (((SDL_GetTicks () / player_idle_anim->speed) % player_idle_anim->n_frames));
        int x_offset = player_idle_anim->frames[currFrame]->col;
        int y_offset = player_idle_anim->frames[currFrame]->row;

        player_sprite_sheet->src_rect.x = player_sprite_sheet->sprite_w * x_offset;
        player_sprite_sheet->src_rect.y = player_sprite_sheet->sprite_h * y_offset;

        // render (renderer);
        {
            SDL_RenderClear (renderer);
            SDL_RenderCopy (renderer, player_sprite_sheet->texture, 
                &player_sprite_sheet->src_rect, &player_sprite_sheet->dest_rect);
            SDL_RenderPresent (renderer);
        }

        // limit the FPS
        sleepTime = timePerFrame - (SDL_GetTicks () - frameStart);
        if (sleepTime > 0) SDL_Delay (sleepTime);
    }

    cleanUp (window, renderer);

    return 0;

}