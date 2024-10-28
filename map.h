
#ifndef _MAP

#define _MAP

// Map struct as grid of chars
typedef struct Map {
    int width;
    int height;
    int startx;
    int starty;
    char **grid;
} Map;


// Create new map from given file
Map *initMap(char *filename);


// Free map from memory
void freeMap(Map *map);

#endif