#ifndef _PLAYER
#define _PLAYER

typedef struct Player {
    int px;
    int py;
} Player;

Player *initPlayer();

void freePlayer(Player *p);

#endif