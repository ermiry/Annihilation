#ifndef PLAYER_H
#define PLAYER_H

#include "annihilation.h"

typedef struct Player {

    u32 goID;

    u32 health;

} Player;

extern Player *player_create_comp (u32 goID);
extern GameObject *player_init (void);
extern void player_update (void *data);
extern void player_destroy_comp (Player *player);

extern GameObject *main_player_go;

#endif