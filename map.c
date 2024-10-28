#include <stdio.h>
#include <stdlib.h>
#include "map.h"

// Create new map from given file
Map *initMap(char *filename) {

    char **grid;
    char c;
    char widthstr[10]; // Width stack
    char heightstr[10]; // Height stack
    int width;
    int height;
    
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Could not open file %s\n", filename);
        return NULL;
    }

    // Get width integer
    int top = 0;
    do {
        if(fread(&c, 1, 1, file) == 0) {
            printf("ERROR: Incorrect map format %s\n", filename);
            fclose(file);
            return NULL;
        }
        widthstr[top++] = c;
        if (top > 10) {
            printf("ERROR: Width too large in %s\n", filename);
            fclose(file);
            return NULL;
        }

    } while (c != '\n' && c != ' ');
    if (top == 0) {
        printf("ERROR: Incorrect width in %s\n", filename);
        fclose(file);
        return NULL;
    }
    // Convert width str to int
    width = atoi(widthstr);

    // Get height integer
    top = 0;
    do {
        if(fread(&c, 1, 1, file) == 0) {
            printf("ERROR: Incorrect map format %s\n", filename);
            fclose(file);
            return NULL;
        }
        heightstr[top++] = c;
        if (top > 10) {
            printf("ERROR: Incorrect height in %s\n", filename);
            fclose(file);
            return NULL;
        }

    } while (c != '\n');

    if (top == 0) {
        printf("ERROR: Height in %s\n", filename);
        fclose(file);
        return NULL;
    }
    // Convert height str to int
    height = atoi(heightstr);

    Map *map = malloc(sizeof(Map));
    // Allocate 2D array
    grid = (char**)malloc(height * sizeof(char*));
    for (int i = 0; i < height; i++)
        grid[i] = (char*)malloc(width * sizeof(char));
    map->grid = grid;
    map->width = width;
    map->height = height;
    map->startx = -1;
    map->starty = -1;

    // Read map grid
    int x = 0, y = 0;
    while (fread(&c, 1, 1, file)) {
        if (c == '\r') {
            continue; // F**k you windows text editors
        }
        if (c == '\n') {
            // Next row
            if (x != width) {
                printf("ERROR: Map has incorrect no. of colums(%d) in row:%d in %s\n",x , y+1, filename);
                freeMap(map);
                fclose(file);
                return NULL;
            }
            if (y >= height) {
                printf("ERROR: Map has too many rows in %s\n", filename);
                freeMap(map);
                fclose(file);
                return NULL;
            }
            y++;
            x = 0; // Reset column
        } else {
            if (c != '0' && c != '1' && c != 'S') {
                printf("ERROR invelid character %d in map %s\n", c, filename);
                freeMap(map);
                fclose(file);
                return NULL;
            }
            if (c == 'S') {
                map->startx = x;
                map->starty = y;
                c = '0';
            }
            grid[y][x++] = c - '0';
        }
    }
    if (y != height-1) {
        printf("ERROR: Map has less no. of rows in %s\n", filename);
        freeMap(map);
        fclose(file);
        return NULL;
    }

    if (map->startx == -1 || map->starty == -1) {
        printf("ERROR: No start pos given in map: %s\n", filename);
        freeMap(map);
        fclose(file);
        return NULL;
    }

    fclose(file);

    return map;
}

// Free map from memory
void freeMap(Map *map) {
    for (int i = 0; i < map->height; i++)
        free(map->grid[i]);

    free(map->grid);
    free(map);
}