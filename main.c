#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct poserpent {
    int x, y, w, h;
    struct poserpent* next;
} poserpent;

typedef struct food {
    int x, y, w, h;
} food;

poserpent* creermembre(int x, int y, int w, int h)
{
    poserpent* snake = malloc(sizeof(poserpent));
    if (!snake) {
        fprintf(stderr, "Erreur d'allocation de mémoire\n");
        exit(EXIT_FAILURE);
    }
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

void maj(poserpent* tete, int dx, int dy)
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
    poserpent* temp = tete;
    int isHead = 1;  // Variable pour vérifier si c'est la tête

    while (temp != NULL) 
    {
        SDL_Rect snakebody = {temp->x, temp->y, temp->w, temp->h};

        if (isHead) {
            // Appliquer une couleur différente pour la tête (par exemple, rouge)
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            isHead = 0;  // La tête a été affichée, maintenant on affiche le corps
        } else {
            // Appliquer une couleur différente pour le corps (par exemple, vert)
            SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
        }

        SDL_RenderFillRect(renderer, &snakebody);
        temp = temp->next;
    }
}

int collision(poserpent* tete, SDL_Rect* walls, int num_walls) {
    // Vérifier si la tête du serpent touche les murs (carrés bleus)
    SDL_Rect snakeRect = { tete->x, tete->y, tete->w, tete->h };
    for (int i = 0; i < num_walls; i++) {
        if (SDL_HasIntersection(&snakeRect, &walls[i])) {
            printf("Collision avec le mur\n");
            return 1;
        }
    }

    // Vérifier la collision avec la queue du serpent
    poserpent* current = tete->next;
    while (current != NULL) {
        if (tete->x == current->x && tete->y == current->y) {
            printf("Collision avec la queue\n");
            return 1;
        }
        current = current->next;
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
    *dx = 20;
    *dy = 0;
    *running = 1;
}

int nourriture_dans_mur(food* f, SDL_Rect* walls, int num_walls) {
    SDL_Rect foodRect = { f->x, f->y, f->w, f->h };
    for (int i = 0; i < num_walls; i++) {
        if (SDL_HasIntersection(&foodRect, &walls[i])) {
            return 1;
        }
    }
    return 0;
}

void placer_nourriture(food* f, SDL_Rect* walls, int num_walls) {
    do {
        f->x = (rand() % 14 + 1) * 20; // Génère des positions comme 20, 40, 60, etc.
        f->y = (rand() % 14 + 1) * 20; // Génère des positions comme 20, 40, 60, etc.
        f->w = 20;
        f->h = 20;
    } while (nourriture_dans_mur(f, walls, num_walls));
}

void afficher_nourriture(SDL_Renderer* renderer, food* f) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_Rect nourriture = { f->x, f->y, f->w, f->h };
    SDL_RenderFillRect(renderer, &nourriture);
}

// Vérifie si les rectangles du serpent et de la nourriture se chevauchent partiellement
int manger(poserpent* tete, food* f) {
    SDL_Rect snakeRect = { tete->x, tete->y, tete->w, tete->h };
    SDL_Rect foodRect = { f->x, f->y, f->w, f->h };
    
    return SDL_HasIntersection(&snakeRect, &foodRect);
}

void agrandir_serpent(poserpent* tete) {
    poserpent* courant = tete;
    while (courant->next != NULL) {
        courant = courant->next;
    }
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
    int dx = 20;
    int dy = 0;
    int running = 1;
    int g, h, b = 0;
    int d = 1;
    int score = 0;
    food nourriture;
    SDL_Rect walls[] = {
        { 0, 0, 320, 10 },    // Top wall
        { 0, 310, 320, 10 },  // Bottom wall
        { 0, 0, 10, 320 },    // Left wall
        { 310, 0, 10, 320 }   // Right wall
    };
    int num_walls = sizeof(walls) / sizeof(walls[0]);
    placer_nourriture(&nourriture, walls, num_walls);

    SDL_Event event;

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
                        dy = -20;
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
                        dy = 20;
                        b = 1;
                        d = 0;
                        g = 0;
                        break;
                    case SDLK_LEFT:
                        if (d == 1)
                        {
                            break;
                        }
                        dx = -20;
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
                        dx = 20;
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

        if (collision(snake, walls, num_walls))
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
                            running = 0;
                            gameover_loop = 0;
                        }
                        else if (event.key.keysym.sym == SDLK_r)
                        {
                            retry(&snake, &dx, &dy, &running);
                            placer_nourriture(&nourriture, walls, num_walls);
                            score = 0; // Réinitialiser le score
                            gameover_loop = 0;
                        }
                    }
                    else if (event.type == SDL_QUIT)
                    {
                        running = 0;
                        gameover_loop = 0;
                    }
                }
            }
        }

        maj(snake, dx, dy);

        if (manger(snake, &nourriture)) {
            agrandir_serpent(snake);
            score += 10;
            placer_nourriture(&nourriture, walls, num_walls);
        }

        if (score == 2540)
        {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Vert pour l'écran de victoire
            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);

            dx = 0; // Arrête le mouvement
            dy = 0;

            printf("Bien joué! Voulez-vous rejouer (R/ESC) ?\n");

            int victory_loop = 1;
            while (victory_loop) 
            {
                while (SDL_PollEvent(&event)) 
                {
                    if (event.type == SDL_KEYDOWN)
                    {
                        if (event.key.keysym.sym == SDLK_ESCAPE)
                        {
                            running = 0;
                            victory_loop = 0;
                        }
                        else if (event.key.keysym.sym == SDLK_r)
                        {
                            retry(&snake, &dx, &dy, &running);
                            placer_nourriture(&nourriture, walls, num_walls);
                            score = 0; // Réinitialiser le score
                            victory_loop = 0;
                        }
                    }
                    else if (event.type == SDL_QUIT)
                    {
                        running = 0;
                        victory_loop = 0;
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Noir
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Bleu
        for (int i = 0; i < num_walls; i++) {
            SDL_RenderFillRect(renderer, &walls[i]);
        }

        afficher(renderer, snake);
        afficher_nourriture(renderer, &nourriture);

        SDL_RenderPresent(renderer);
        SDL_Delay(100);
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}