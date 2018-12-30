#include <stdlib.h>

#include "game.h"
#include "map.h"

#include "utils/myUtils.h"

#pragma region CAVE ROOM

// TODO:
// FIXME: we need to fix how to free the tiles that are in multiple lists at a time
static void room_destroy (void *data) {

    if (data) {
        Room *room = (Room *) data;
        free (room);
    }

}

static Room *room_create (LList *roomTiles, u32 **map) {

    Room *room = (Room *) malloc (sizeof (Room));
    if (room) {
        room->tiles = roomTiles;
        room->roomSize = llist_size (room->tiles);
        room->connectedRooms = llist_init (room_destroy);
        room->edgeTiles = llist_init (free);

        Coord *tile = NULL;
        for (Node *n = llist_start (room->tiles); n != NULL; n = n->next) {
            tile = llist_data (n);
            for (u8 x = tile->x - 1; x <= tile->x + 1; x++) {
                for (u8 y = tile->y - 1; y <= tile->y + 1; y++) {
                    if (x == tile->x || y == tile->y)
                        if (map[x][y] == 1)
                            llist_insert_next (room->edgeTiles, llist_end (room->edgeTiles), tile);
                }
            }
        }
    }

    return room;

}

#pragma endregion

#pragma region CAVE

static void cave_random_fill_map (Cave *cave) {

    random_set_seed (cave->seed);

    for (u32 x = 0; x < cave->width; x++) {
        for (u32 y = 0; y < cave->heigth; y++) {
            if (x == 0 || x == cave->width - 1 || y == 0 || y == cave->heigth - 1)
                cave->map[x][y] = 1;

            else 
                cave->map[x][y] = (random_int_in_range (0, 100) < cave->fillPercent) ? 1 : 0;
            
        }
    }

}

static bool cave_is_in_map_range (Cave *cave, i32 x, i32 y) {

    return (x >= 0 && x < cave->width && y >= 0 && y < cave->heigth);

}

static u8 cave_get_surrounding_wall_count (Cave *cave, i32 gridX, i32 gridY) {

    u8 wallCount = 0;
    for (i32 neighbourX = gridX - 1; neighbourX <= gridX + 1; neighbourX++) {
        for (i32 neighbourY = gridY - 1; neighbourY <= gridY + 1; neighbourY++) {
            if (cave_is_in_map_range (cave, neighbourX, neighbourY)) {
                if (neighbourX != gridX || neighbourY != gridY)
                    wallCount += cave->map[neighbourX][neighbourY];
            }

            else wallCount++;
                
        }
    }

    return wallCount;

}

static void cave_smooth_map (Cave *cave) {

    for (u32 x = 0; x < cave->width - 1; x++){
        for (u32 y = 0; y < cave->heigth - 1; y++) {
            u8 neighbourWallTiles = cave_get_surrounding_wall_count (cave, x, y);

            if (neighbourWallTiles > 4) cave->map[x][y] = 1;
            else if (neighbourWallTiles < 4) cave->map[x][y] = 0;
        }
    }

}

// TODO: create a more advanced system based on c sharp accent resources manager
// TODO: also create a more advance system for multiple sprites and gameobjects for
// destroying the map
// TODO: create a parent gameobejct and names based on position
// create a game object for each cave
static void cave_draw (Map *map, Cave *cave) {

    GameObject *go = NULL;
    Transform *transform = NULL;
    Graphics *graphics = NULL;
    for (u32 y = 0; y < cave->heigth; y++) {
        for (u32 x = 0; x < cave->width; x++) {
            if (cave->map[x][y] == 1) {
                go = game_object_new (NULL, NULL);
                game_object_add_component (go, TRANSFORM_COMP);
                game_object_add_component (go, GRAPHICS_COMP);

                graphics = game_object_get_component (go, GRAPHICS_COMP);
                graphics_set_sprite (graphics, "./assets/artwork/mapTile_087.png");

                transform = game_object_get_component (go, TRANSFORM_COMP);
                transform->position.x = graphics->sprite->w * x;
                transform->position.y = graphics->sprite->h * y;

                map->go_map[x][y] = go;
            }
        }
    }

}

Cave *cave_generate (Map *map, u32 width, u32 heigth, u32 seed, u32 fillPercent) {

    Cave *cave = (Cave *) malloc (sizeof (Cave));
    if (cave) {
        cave->width = width;
        cave->heigth = heigth;

        cave->map = (u8 **) calloc (width, sizeof (u8 *));
        for (u16 i = 0; i < width; i++) cave->map[i] = (u8 *) calloc (heigth, sizeof (u8));

        cave->fillPercent = fillPercent;

        // FIXME: use a random seed
        if (seed <= 0) {
            cave->seed = 7;
            cave->useRandomSeed = true;
        } 
        else cave->seed = seed;
        
        cave_random_fill_map (cave);

        for (u8 i = 0; i < 3; i++) cave_smooth_map (cave);

        cave_draw (map, cave);
    }

    return cave;

}

void cave_destroy (Cave *cave) {

    if (cave) {
        if (cave->map) free (cave->map);
        free (cave);
    }

}

#pragma endregion

#pragma region MAP

Map *map_create (u32 width, u32 heigth) {

    Map *new_map = (Map *) malloc (sizeof (Map));
    if (new_map) {
        new_map->width = width;
        new_map->heigth = heigth;

        new_map->go_map = (GameObject ***) calloc (width, sizeof (GameObject **));
        for (u16 i = 0; i < new_map->width; i++) 
            new_map->go_map[i] = (GameObject **) calloc (new_map->heigth, sizeof (GameObject *));

        new_map->cave = NULL;
    }

    return new_map;

}

void map_destroy (Map *map) {

    if (map) {
        if (map->go_map) free (map->go_map);
        if (map->cave) cave_destroy (map->cave);

        free (map);
    }

}

#pragma endregion