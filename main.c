/*
 * Bouncing Ball Simulation 2.0 - Terminal Based
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "map.h"
#include "ball.h"

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
#else
    #include <unistd.h>
    #include <termios.h>
    #include <fcntl.h>
    
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
    Ball *ball = initBall(map->startx+0.5, map->starty+0.5, ball_width, ball_height, 0.707, 0.707);
    if (ball == NULL) {
        freeMap(map);
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

        updateBall(ball, map);
        drawBall(ball, surface, 'G');

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

        // Check for key press
        if(_kbhit()){
            #ifdef _WIN32
                char c = _getch();
                char backspace_key = 8;
            #else
                char c = getchar();
                char backspace_key = 127;
            #endif
            if (c == 27) {
                // Exit if Escape key is pressed
                break;
            }
            if (c == backspace_key) {
                // Return ball to start pos if Backspace key is pressed
                ball->x = map->startx + 0.5 - ball->w;
                ball->y = map->starty + 0.5 - ball->h;
            }
        }
    }

    // Free surface
    for (int i = 0; i < map->height; i++)
        free(surface[i]);
    free(surface);

    freeMap(map);
    freeBall(ball);

    #ifndef _WIN32
        disable_raw_mode();  // Restore terminal mode on Linux
    #endif

    return 0;
}
