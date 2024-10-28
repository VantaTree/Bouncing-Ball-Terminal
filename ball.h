#include "map.h"

#ifndef _BALL

#define _BALL

// Ball struct with position x, y and velocity dx, dy type:(float)
typedef struct Ball {
    float x;
    float y;
    float w;
    float h;
    float dx;
    float dy;
} Ball;

// Create a new Ball with given position and velocity
Ball *initBall(float x, float y, float w, float h, float dx, float dy);

// Update the ball's posiiton
void updateBall(Ball *b, Map *map);

// Draw the ball as the marker on the given 2D surface
void drawBall(Ball *b, char *surface[], char marker);

// Free ball from memory
void freeBall(Ball *b);

#endif