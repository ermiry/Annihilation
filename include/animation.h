#ifndef ANIMATION_H
#define ANIMATION_H

#include "animation.h"

#include "sprites.h"

#define DEFAULT_ANIM_SPEED      100

typedef struct Animation {

    SpriteSheet *spriteSheet;
    Sprite **frames;

    u8 n_frames;
    u32 speed;

} Animation;

typedef struct Animator {

    Animation *currAnimation;
    // TODO: how do we want to store the animations?

} Animator;

extern Animation *animation_create (SpriteSheet *spriteSheet, u8 n_frames, ...);
extern void animation_destroy (Animation *animation);
extern void animation_set_speed (Animation *animation, u32 speed);

#endif