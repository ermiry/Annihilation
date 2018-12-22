#ifndef PLAYER_H
#define PLAYER_H

#include "annihilation.h"

typedef struct Player {

    u32 health;

} Player;

extern Player *player_create (void);
extern void player_update (void *data);

#endif