#ifndef TESTS_STRUCT_H
#define TESTS_STRUCT_H

#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <time.h>

struct s_snake {
    int x, y, w, h;
    struct s_snake *next;
};

typedef struct s_snake poserpent;

struct s_mur {
    int x, y, w, h;
};
typedef struct s_mur mur;

typedef struct s_food {
    int x, y, w, h;
} food;

#endif //MAIN_H