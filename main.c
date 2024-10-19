#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

int main() {
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

    // Boucle d'événements pour garder la fenêtre ouverte
    int running = 1;
    SDL_Event event;

    while (running) 
    {
        while (SDL_PollEvent(&event)) 
        {
            if (event.type == SDL_QUIT) 
            {
                running = 0;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Noir
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Rouge

        SDL_Rect topWall = { 0, 0, 320, 10 };         // Mur du haut
        SDL_Rect bottomWall = { 0, 310, 320, 10 };    // Mur du bas
        SDL_Rect leftWall = { 0, 0, 10, 320 };        // Mur de gauche
        SDL_Rect rightWall = { 310, 0, 10, 320 };     // Mur de droite
        SDL_Rect snakehead = { 10, 10, 10, 10 };      // Tête du serpent
        SDL_Rect snakebody = { 10, 10, 10, 10 };      // Corps du serpent

        SDL_RenderFillRect(renderer, &topWall);
        SDL_RenderFillRect(renderer, &bottomWall);
        SDL_RenderFillRect(renderer, &leftWall);
        SDL_RenderFillRect(renderer, &rightWall);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
