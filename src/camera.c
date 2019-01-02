#include <stdlib.h>

#include "annihilation.h"
#include "game.h"
#include "camera.h"

#include "vector2d.h"

#include "utils/myUtils.h"

// FIXME: set all initial parameters to default values
Camera *camera_new (u32 windowWidth, u32 windowHeight) {

    Camera *cam = (Camera *) malloc (sizeof (Camera));
    if (cam) {
        cam->windowWidth = windowWidth;
        cam->windowHeight = windowHeight;

        cam->bounds.x = cam->bounds.y = 0;
        cam->bounds.w = windowWidth;
        cam->bounds.h = windowHeight;
    }

    return cam;

}

void camera_destroy (Camera *cam) {

    if (cam) {
        cam->target = NULL;

        free (cam);
    }

}

void camera_set_center (Camera *cam, u32 x, u32 y) {

    if (cam) {
        cam->center.x = x;
        cam->center.y = y;
        cam->bounds.x = x - (cam->bounds.w * 0.5);
        cam->bounds.y = y - (cam->bounds.h * 0.5);
    }

}

// TODO:
void camera_set_size (Camera *cam, u32 width, u32 height) {

    if (cam) {

    }

}

static Point point_world_to_screen (Camera *cam, const Point p, 
    float xScale, float yScale) {

    Point retPoint = p;

    retPoint.x -= cam->bounds.x;
    retPoint.y -= cam->bounds.y;

    retPoint.x *= xScale;
    retPoint.y *= yScale;

    return retPoint;

}

CamRect camera_world_to_screen (Camera *cam, const CamRect destRect) {

    if (cam) {
        CamRect screenRect = destRect;

        float xScale = (float) cam->windowWidth / cam->bounds.w;
        float yScale = (float) cam->windowHeight / cam->bounds.h;

        Point screenPoint = { screenRect.x, screenRect.y };
        screenPoint = point_world_to_screen (cam, screenPoint, xScale, yScale);

        screenRect.x = screenPoint.x;
        screenRect.y = screenPoint.y;
        screenRect.w = (int) (screenRect.w * xScale);
        screenRect.h = (int) (screenRect.h * yScale);

        return screenRect;
    }

}

// FIXME:
CamRect *camera_screen_to_world (Camera *cam, const CamRect *sr) {

    // CamRect *rect = sr;

    // float xScale = (float) cam->bounds.w / cam->windowWidth;
    // float yScale = (float) cam->bounds.h / cam->windowHeight;

    // Point p = { rect->x, rect->y };
    // FIXME:

    // rect->x = 

}