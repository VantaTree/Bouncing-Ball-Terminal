#include "map.h"
#include "player.h"

#ifndef _BALL

#define _BALL

typedef enum BallState {
    BOUNCE = 0,
    PLAYER = 1
} BallState;

// Ball struct with position x, y and velocity dx, dy type:(float)
typedef struct Ball {
    float x;
    float y;
    float w;
    float h;
    float dx;
    float dy;
    char marker;
    BallState state;
} Ball;

// Create a new Ball with given position and velocity
Ball *initBall(float x, float y, float w, float h, float dx, float dy, char marker);

// Update the ball's posiiton
void updateBall(Ball *b, Map *map, Player *player);

// Draw the ball as the marker on the given 2D surface
void drawBall(Ball *b, char *surface[]);

// Free ball from memory
void freeBall(Ball *b);

#endif