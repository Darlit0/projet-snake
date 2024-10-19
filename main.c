#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <time.h>
#include "struct.h"


poserpent* creermembre(int x, int y, int w, int h)
{
    poserpent* snake = malloc(sizeof(poserpent));
    snake->x = x;
    snake->y = y;
    snake->w = w;
    snake->h = h;
    snake->next = NULL;
    return snake;
}

poserpent* creersnake()
{
    poserpent* tete = creermembre(160, 160, 20, 20);
    tete->next = creermembre(140, 160, 20, 20);
    return tete;
}

void maj(poserpent*tete, int dx, int dy)
{
    int prev_x = tete->x;
    int prev_y = tete->y;
    tete->x += dx;
    tete->y += dy;

    poserpent* current = tete->next;
    while (current != NULL)
    {
        int temp_x = current->x;
        int temp_y = current->y;
        current->x = prev_x;
        current->y = prev_y;
        prev_x = temp_x;
        prev_y = temp_y;
        current = current->next;
    }
}

void afficher(SDL_Renderer* renderer, poserpent* tete)
{
    SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
    poserpent* temp = tete;
    while (temp != NULL)
    {
        SDL_Rect snakebody = {temp->x, temp->y, temp->w, temp->h};
        SDL_RenderFillRect(renderer, &snakebody);
        temp = temp->next;
    }
}

int collision(poserpent* tete)
{
    if (tete->x < 0 || tete->x > 300 || tete->y < 0 || tete->y > 300)
    {
        printf("Collision avec le mur\n");
        return 1;
    }
    return 0;
}

void retry(poserpent** tete, int* dx, int* dy, int* running) 
{
    poserpent* courrant = *tete;
    while(courrant != NULL) 
    {
        poserpent *suiv = courrant->next;
        free(courrant);
        courrant = suiv;
    }

    *tete = creersnake();
    *dx = 10;
    *dy = 0;
    *running = 1;
}

void placer_nourriture(food* f) {
    f->x = (rand() % 16) * 20;
    f->y = (rand() % 16) * 20;
    f->w = 20;
    f->h = 20;
}

void afficher_nourriture(SDL_Renderer* renderer, food* f) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_Rect nourriture = { f->x, f->y, f->w, f->h };
    SDL_RenderFillRect(renderer, &nourriture);
}

int manger(poserpent* tete, food* f) {
    return tete->x == f->x && tete->y == f->y;
}

void agrandir_serpent(poserpent* tete) {
    poserpent* courant = tete;
    while (courant->next != NULL) {
        courant = courant->next;
    }
    // Ajouter un nouveau segment au bout
    courant->next = creermembre(courant->x, courant->y, courant->w, courant->h);
}

int main() {
    srand(time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) 
    {
        printf("Error SDL2 Init : %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 320, 320, SDL_WINDOW_OPENGL);

    if (window == NULL) 
    {
        printf("Error while creating a window : %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) 
    {
        printf("Error while creating renderer : %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    poserpent* snake = creersnake();
    int dx = 10;
    int dy = 0;
    int running = 1;
    int g, h, b = 0;
    int d = 1;
    food nourriture;
    placer_nourriture(&nourriture);

    SDL_Event event;
    SDL_Rect topWall = { 0, 0, 320, 10 };
    SDL_Rect bottomWall = { 0, 310, 320, 10 };
    SDL_Rect leftWall = { 0, 0, 10, 320 };
    SDL_Rect rightWall = { 310, 0, 10, 320 };

    while (running) 
    {
        while (SDL_PollEvent(&event)) 
        {
            if (event.type == SDL_QUIT) 
            {
                running = 0;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_UP:
                        if (b == 1)
                        {
                            break;
                        }
                        dx = 0;
                        dy = -10;
                        h = 1;
                        d = 0;
                        g = 0;
                        break;
                    case SDLK_DOWN:
                        if (h == 1)
                        {
                            break;
                        }
                        dx = 0;
                        dy = 10;
                        b = 1;
                        d = 0;
                        g = 0;
                        break;
                    case SDLK_LEFT:
                        if (d == 1)
                        {
                            break;
                        }
                        dx = -10;
                        dy = 0;
                        g = 1;
                        h = 0;
                        b = 0;
                        break;
                    case SDLK_RIGHT:
                        if (g == 1)
                        {
                            break;
                        }
                        dx = 10;
                        dy = 0;
                        d = 1;
                        h = 0;
                        b = 0;
                        break;
                    default:
                        break;
                }
            }
        }

        if (collision(snake))
        {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Rouge pour l'écran "Game Over"
            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);

            dx = 0; // Arrête le mouvement
            dy = 0;

            printf("Game Over! Voulez-vous rejouer (R/ESC) ?\n");

            int gameover_loop = 1;
            while (gameover_loop) 
            {
                while (SDL_PollEvent(&event)) 
                {
                    if (event.type == SDL_KEYDOWN)
                    {
                        if (event.key.keysym.sym == SDLK_ESCAPE)
                        {
                            running = 0;  // Quitter le jeu
                            gameover_loop = 0;  // Sortir de la boucle de Game Over
                        }
                        else if (event.key.keysym.sym == SDLK_r)
                        {
                            retry(&snake, &dx, &dy, &running);  // Relance du jeu
                            placer_nourriture(&nourriture);  // Réinitialiser la nourriture
                            gameover_loop = 0;  // Sortir de la boucle de Game Over
                        }
                    }
                    else if (event.type == SDL_QUIT)
                    {
                        running = 0;  // Si la fenêtre est fermée
                        gameover_loop = 0;
                    }
                }
            }
        }

        maj(snake, dx, dy);

        // Si le serpent mange la nourriture
        if (manger(snake, &nourriture)) {
            agrandir_serpent(snake);  // Agrandir le serpent
            placer_nourriture(&nourriture);  // Placer une nouvelle nourriture
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Noir
        SDL_RenderClear(renderer);

        // Redessiner les murs
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Bleu
        SDL_RenderFillRect(renderer, &topWall);
        SDL_RenderFillRect(renderer, &bottomWall);
        SDL_RenderFillRect(renderer, &leftWall);
        SDL_RenderFillRect(renderer, &rightWall);

        afficher(renderer, snake);
        afficher_nourriture(renderer, &nourriture);  // Afficher la nourriture

        SDL_RenderPresent(renderer);
        SDL_Delay(100);
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
