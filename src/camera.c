#include <stdlib.h>

#include "annihilation.h"
#include "game.h"
#include "camera.h"

#include "vector2d.h"

#include "utils/myUtils.h"

static Camera *mainCam = NULL;

void camera_new (u32 x, u32 y) {

    mainCam = (Camera *) malloc (sizeof (Camera));
    if (mainCam) {
        mainCam->transform.position.x = x;
        mainCam->transform.position.y = y;

        mainCam->orthoSize = DEFAULT_CAM_ORTHO_SIZE;

        mainCam->min.x = mainCam->min.y = 0;
        mainCam->max.x = mainCam->max.y = 0;

        mainCam->isFollwing = false;
        mainCam->target = NULL;
    }

}

void camera_destroy (void) {

    if (mainCam) {
        mainCam->target = NULL;

        free (mainCam);
    }

}

// TODO: set camera bounds

void camera_set_target (Transform *target) { 
    
    if (target) {
        mainCam->target = target;
        mainCam->isFollwing = true;
    }  
    
}

// FIXME:
void camera_update (void) {

    u32 x = mainCam->transform.position.x;
    u32 y = mainCam->transform.position.y;

    if (mainCam->isFollwing) {

    }

    float cameraHalfWidth = mainCam->orthoSize * ((float) SCREEN_WIDTH / SCREEN_HEIGHT);

    x = clamp_int (x, mainCam->min.x + cameraHalfWidth, mainCam->max.x - cameraHalfWidth);
    y = clamp_int (y, mainCam->min.y + mainCam->orthoSize, mainCam->max.y - mainCam->orthoSize);

    mainCam->transform.position.x = x;
    mainCam->transform.position.y = y;

}