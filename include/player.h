#ifndef PLAYER_H
#define PLAYER_H

#include "annihilation.h"

#include "entity.h"

typedef enum PlayerState {

    PLAYER_IDLE = 0,
    PLAYER_MOVING,
    PLAYER_ATTACK,

} PlayerState;

typedef struct Player {

    u32 goID;

    PlayerState currState;
    LivingEntity entity;

} Player;

extern Player *player_create_comp (u32 goID);
extern GameObject *player_init (void);
extern void player_update (void *data);
extern void player_destroy_comp (Player *player);

extern GameObject *main_player_go;

#endif