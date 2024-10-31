#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "ball.h"
#include "map.h"

void collision(Ball *b, char axis, Map *map);
bool rectcollide(float r1x, float r1y, float r1w, float r1h, float r2x, float r2y, float r2w, float r2h);

// Update the ball's posiiton
void updateBall(Ball *b, Map *map) {

    // Move the ball in x axis
    b->x += b->dx;
    collision(b, 'x', map);
    // Move the ball in y axis
    b->y += b->dy;
    collision(b, 'y', map);

}

// Draw the ball as the marker on the given 2D surface
void drawBall(Ball *b, char *surface[], char marker) {
    int ax = b->x;
    int ay = b->y;
    int bx = b->x + b->w;
    int by = b->y + b->h;
    for (int y = ay; y < by; y++){
        for (int x = ax; x < bx; x++){
            surface[y][x] = marker;
        }
    }
    // surface[(int) b->y][(int) b->x] = marker;
}

// Create a new Ball with given position and velocity
Ball *initBall(float x, float y, float w, float h, float dx, float dy) {
    Ball *b = malloc(sizeof(Ball));
    if (b == NULL) {
        printf("Ball couldn't be malloced\n");
        return NULL;
    }
    b->x = x - w;
    b->y = y - h;
    b->w = w;
    b->h = h;
    b->dx = dx;
    b->dy = dy;
    return b;
}

// Free ball from memory
void freeBall(Ball *b) {
    free(b);
}

// Check collision with 2D map axis = 'x'|'y'
void collision(Ball *b, char axis, Map *map) {

    // Get the center pos of the bal
    int ax = b->x;
    int ay = b->y;
    int bx = b->x + b->w;
    int by = b->y + b->h;

    for (int y = ay; y <= by; y++){
        for (int x = ax; x <= bx; x++){
    
            // out of map bounds check
            if (x < 0 || y < 0 || x >= map->width || y >= map->height) {
                continue;
            }

            char cell = map->grid[y][x];
            // No wall
            if (cell == 0) {
                continue;
            }

            // Each map cell is of 1.0 size
            if (!rectcollide(b->x, b->y, b->w, b->h, (float) x, (float) y, 1.0, 1.0)) {
                continue;
            }

            // collided on the x axis
            if (axis == 'x') {
                if (b->dx > 0) { // ball was going right
                    b->x = x - b->w;
                } else if (b->dx < 0) { // ball was going left
                    b->x = x + 1.0;
                }
                // Reverse dx when colliding with a wall (bounce)
                b->dx *= -1.0;
                return;
            }
            // collided on the y axis
            else if (axis == 'y') {
                if (b->dy > 0) { // ball was going down
                    b->y = y - b->h;
                } else if (b->dy < 0) {  // ball was going up
                    b->y = y + 1.0;
                }
                // Reverse dy when colliding with a wall (bounce)
                b->dy *= -1.0;
                return;
            }
        }
    }
}

// rectancgle collision check
bool rectcollide(float r1x, float r1y, float r1w, float r1h,
                float r2x, float r2y, float r2w, float r2h) {
    return (
        r1x + r1w > r2x &&     // r1 right edge past r2 left
        r1x < r2x + r2w &&     // r1 left edge past r2 right
        r1y + r1h > r2y &&     // r1 top edge past r2 bottom
        r1y < r2y + r2h
    );
}

