#include <stdlib.h>
#include <stdarg.h>

#include "annihilation.h"

#include "sprites.h"
#include "animation.h"

/*** ANIMATION ***/

Animation *animation_create (u8 n_frames, ...) {

    va_list valist;
    va_start (valist, n_frames);

    Animation *animation = (Animation *) malloc (sizeof (Animation));
    if (animation) {
        animation->speed = DEFAULT_ANIM_SPEED;
        animation->n_frames = n_frames;
        animation->frames = (IndividualSprite **) calloc (n_frames, sizeof (IndividualSprite *));

        for (u8 i = 0; i < n_frames; i++)
            animation->frames[i] = va_arg (valist, IndividualSprite *);

        va_end (valist);
    }

    return animation;

}

void animation_destroy (Animation *animation) {

    if (animation) {
        if (animation->frames) free (animation->frames);

        free (animation);
    }

}

void animation_set_speed (Animation *animation, u32 speed) {

    if (animation) animation->speed = speed;

}

/*** ANIMATOR ***/

