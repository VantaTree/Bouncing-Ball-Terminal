/*
 * Bouncing Ball Simulation 2.0 - Terminal Based
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "map.h"
#include "ball.h"
#include "player.h"

// Cross platform sleep
#ifdef _WIN32
    #include <windows.h>
    void sleep_ms(int milliseconds) {
        Sleep(milliseconds);  // Windows sleep function
    }
#else
    #include <unistd.h>
    void sleep_ms(int milliseconds) {
        usleep(milliseconds * 1000);  // POSIX sleep function
    }
#endif


// cross platform khbit setup
#ifdef _WIN32
    #include <conio.h>   // Windows-specific for _kbhit and _getch
    
    // Default function to get character
    char getChar() {
        return _getch();
    }
    #else
    #include <unistd.h>
    #include <termios.h>
    #include <fcntl.h>
    
    // Default function to get character
    char getChar() {
        return getchar();
    }

    // Function to set terminal to non-canonical mode for Linux
    void enable_raw_mode() {
        struct termios term;
        tcgetattr(STDIN_FILENO, &term);
        term.c_lflag &= ~(ICANON | ECHO);  // Disable canonical mode and echo
        tcsetattr(STDIN_FILENO, TCSANOW, &term);
    }
    
    // Function to restore terminal to normal mode
    void disable_raw_mode() {
        struct termios term;
        tcgetattr(STDIN_FILENO, &term);
        term.c_lflag |= ICANON | ECHO;  // Enable canonical mode and echo
        tcsetattr(STDIN_FILENO, TCSANOW, &term);
    }
    
    // Function to check if a key has been pressed (Linux version)
    int _kbhit() {
        struct termios oldt, newt;
        int ch;
        int oldf;

        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

        ch = getchar();

        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        fcntl(STDIN_FILENO, F_SETFL, oldf);

        if(ch != EOF) {
            ungetc(ch, stdin);
            return 1;
        }

        return 0;
    }
#endif


int main(int argc, char *argv[]) {

    const int FPS = 30;

    // Argument setup
    int ball_width, ball_height;
    char *map_name;
    if (argc != 2 && argc != 3 && argc != 4) {
        printf("Usage:\n");
        printf("    ./bounce <map>\n");
        printf("    ./bounce <ball size> <map>\n");
        printf("    ./bounce <ball height> <ball width> <map>\n");
        printf("Press Backspace to reset ball position but not velocity\n");
        printf("Press Escape to exit instead of ^C\n");
        return 1;
    }
    if (argc == 2) {
        ball_width = 1.0;
        ball_height = 1.0;
        map_name = argv[1];
    } else if (argc == 3) {
        ball_width = atoi(argv[1]);
        ball_height = ball_width;
        map_name = argv[2];
    } else if (argc == 4) {
        ball_width = atoi(argv[1]);
        ball_height = atoi(argv[2]);
        map_name = argv[3];
    }

    // Initialization
    Map *map = initMap(map_name);
    if (map == NULL) {
        printf("\nExample Map.txt:\n");
        printf("\n<map width> <map height>\n5 5\n#####\n#   #\n# S #\n#   #\n#####\n");
        printf("\n# - Wall\n  - Empty\nS - Ball position\n");
        printf("\nExited with Error 1\n");
        return 1;
    }
    Player *player = initPlayer();
    if (player == NULL) {
        freeMap(map);
        printf("\nExited with Error 1\n");
        return 1;
    }
    Ball *ball = initBall(map->startx+0.5, map->starty+0.5, ball_width, ball_height, 0.707, 0.707, 'G');
    if (ball == NULL) {
        freeMap(map);
        free(player);
        printf("\nExited with Error 1\n");
        return 1;
    }

    #ifndef _WIN32
        enable_raw_mode();  // Enable non-canonical mode for Linux
    #endif

    // surface buffer
    char **surface = (char**)malloc(map->height * sizeof(char*));
    for (int i = 0; i < map->height; i++)
        surface[i] = (char*)malloc(map->width * sizeof(char));

    // Clear surface and return cursor to home position
    printf("\033[2J");

    // Infinite loop
    for(;;) {

        // Get the start time of the frame
        clock_t start_time = clock();

        for (int y = 0; y < map->height; y++) {
            for (int x = 0; x < map->width; x++) {
                if (map->grid[y][x] == 1) {
                    // Set '#' if x or y is a wall
                    surface[y][x] = '#';
                } else {
                    surface[y][x] = ' ';
                }
            }
        }

        updateBall(ball, map, player);
        drawBall(ball, surface);

        // Return cursor to home position
        printf("\033[H");

        // Draw the surface buffer to the output terminal
        for (int y = 0; y < map->height; y++) {
            for (int x = 0; x < map->width; x++) {
                putchar(surface[y][x]);
            }
            putchar('\n'); // Next row
        }

        // Calculate elapsed time
        clock_t end_time = clock();
        int elapsed_ms = (int)((end_time - start_time) * 1000 / CLOCKS_PER_SEC);

        // Calculate remaining time to maintain the frame duration
        int sleep_time = 1000 / FPS - elapsed_ms;
        if (sleep_time > 0) {
            sleep_ms(sleep_time);
        }

        // Reset player input
        player->px = 0;
        player->py = 0;

        // Check for key press
        if(_kbhit()){
            char c = getChar();
            #ifdef _WIN32
                char backspace_key = 8;
            #else
                char backspace_key = 127;
            #endif
            bool up_arr = false;
            bool down_arr = false;
            bool left_arr = false;
            bool right_arr = false;
           
            #ifdef _WIN32
                if (c == 0 || c == -32) {
                    c = getChar();
                    up_arr = c == 72;
                    down_arr = c == 80;
                    left_arr = c == 75;
                    right_arr = c == 77;
                    c = 0;
                }
                if (c == 27) { break; }
            #else
                if (c == 27) { // Escape
                    c = getChar();
                    if (c != 91) { // '['
                        // Exit if Escape key is pressed
                        break;
                    }
                    c = getChar();
                    up_arr = c == 'A';
                    down_arr = c == 'B';
                    left_arr = c == 'D';
                    right_arr = c == 'C';
                    c = 0;
                }
            #endif

            if (c == backspace_key) {
                // Return ball to start pos if Backspace key is pressed
                ball->x = map->startx + 0.5 - ball->w;
                ball->y = map->starty + 0.5 - ball->h;
            }

            // Change State
            if (c >= '1' && c <= '2') {
                if (ball->state != BOUNCE) {
                    ball->dx = 0.707;
                    ball->dy = 0.707;
                }
                ball->state = c - '1';
            }

            // Get player input
            if (c == 'W' || c == 'w' || up_arr) {
                player->py = -1;
            } else if (c == 'S' || c == 's' || down_arr) {
                player->py = 1;
            }
            if (c == 'A' || c == 'a' || left_arr) {
                player->px = -1;
            } else if (c == 'D' || c == 'd' || right_arr) {
                player->px = 1;
            }
        }
    }

    // Free surface
    for (int i = 0; i < map->height; i++)
        free(surface[i]);
    free(surface);

    freeMap(map);
    freeBall(ball);
    freePlayer(player);

    #ifndef _WIN32
        disable_raw_mode();  // Restore terminal mode on Linux
    #endif

    return 0;
}
