#include "annihilation.h"

#include "player.h"

Player *player_create (void) {

    Player *new_player = (Player *) malloc (sizeof (Player));
    if (new_player) {
        new_player->health = 0;
    }

    return new_player;

}

void player_update (void *data) {



}