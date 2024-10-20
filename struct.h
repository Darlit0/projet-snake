#ifndef TESTS_STRUCT_H
#define TESTS_STRUCT_H

#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <time.h>
#include <SDL_mixer.h>

typedef struct poserpent {
    int x, y, w, h;
    struct poserpent* next;
} poserpent;

typedef struct food {
    int x, y, w, h;
} food;

struct s_mur {
    int x, y, w, h;
};
typedef struct s_mur mur;

struct s_malus {
    int x, y, w, h;
};
typedef struct s_malus malus;

struct s_bonus {
    int x, y, w, h;
};
typedef struct s_bonus bonus;

void jouerSon(Mix_Chunk* son);

#endif