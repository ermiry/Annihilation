#ifndef CAMERA_H
#define CAMERA_H

#include <stdbool.h>

#include "game.h"

#include "vector2d.h"

#define DEFAULT_CAM_ORTHO_SIZE      1

typedef struct Camera {

    Transform transform;
    float orthoSize;

    Vector2D min, max;
    Vector2D margin, smoothing;

    bool isFollwing;
    Transform *target;

} Camera;

extern void camera_new (u32 posX, u32 posY);
extern void camera_destroy (void);

extern void camera_set_target (Transform *target);

extern void camera_update (void);

#endif