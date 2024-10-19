#ifndef TESTS_STRUCT_H
#define TESTS_STRUCT_H

struct s_snake {
    int x, y, w, h;
    struct s_snake *next;
};

typedef struct s_snake poserpent;

struct s_mur {
    int x, y, w, h;
};

typedef struct {
    int x, y, w, h;
} food;


#endif //MAIN_H