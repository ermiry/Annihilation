#include <stdlib.h>
#include <stdarg.h>

#include "annihilation.h"

#include "sprites.h"
#include "animation.h"

/*** ANIMATION ***/

Animation *animation_create (SpriteSheet *spriteSheet, u8 n_frames, ...) {

    if (spriteSheet) {
        va_list valist;
        va_start (valist, n_frames);

        Animation *animation = (Animation *) malloc (sizeof (Animation));
        if (animation) {
            animation->spriteSheet = spriteSheet;
            animation->speed = DEFAULT_ANIM_SPEED;
            animation->n_frames = n_frames;
            animation->frames = (Sprite **) calloc (n_frames, sizeof (Sprite *));

            for (u8 i = 0; i < n_frames; i++)
                animation->frames[i] = va_arg (valist, Sprite *);

            va_end (valist);

            return animation;
        }
    }

    return NULL;

}

void animation_destroy (Animation *animation) {

    if (animation) {
        animation->spriteSheet = NULL;
        if (animation->frames) free (animation->frames);

        free (animation);
    }

}

void animation_set_speed (Animation *animation, u32 speed) {

    if (animation) animation->speed = speed;

}