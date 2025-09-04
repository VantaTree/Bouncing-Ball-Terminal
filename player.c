#include <stdio.h>
#include <stdlib.h>

#include "player.h"

Player *initPlayer() {
    Player *player = malloc(sizeof(Player));

    if (player == NULL) {
        printf("Player couldn't be mallocated\n");
        return NULL;
    }

    player->px = 0;
    player->py = 0;

    return player;
}

void freePlayer(Player *p) {
    free(p);
}