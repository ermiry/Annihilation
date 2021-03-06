#ifndef PLAYER_H
#define PLAYER_H

#include "annihilation.h"

#include "entity.h"

typedef enum PlayerState {

    PLAYER_IDLE = 0,
    PLAYER_MOVING,
    PLAYER_ATTACK,

} PlayerState;

typedef struct PlayerProfile {

    u32 profileID;
    char *username;

    u32 kills;
    u32 gamesPlayed;
    u32 highscore;

    u32 n_friends;
    char *friends;
    // char *guild;

} PlayerProfile;

typedef struct Character {

    LivingEntity *entity;

    u16 money [3];          // gold, silver, copper
    // Item ***inventory;
    // Item **weapons;
    // Item **equipment;

} Character;

typedef struct Player {

    u32 goID;

    PlayerState currState;
    PlayerProfile *profile;
    Character *character;

} Player;

#define MAIN_HAND       0
#define OFF_HAND        1

#define EQUIPMENT_ELEMENTS      10

// head         0
// necklace     1
// shoulders    2
// cape         3
// chest        4

// hands        5
// belt         6
// legs         7
// shoes        8
// ring         9

extern Player *player_create_comp (u32 goID);
extern GameObject *player_init (void);
extern void player_update (void *data);
extern void player_destroy_comp (Player *player);

extern GameObject *main_player_go;

#endif