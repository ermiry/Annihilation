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

    bool isFollwing;
    Transform *target;

} Camera;

#endif