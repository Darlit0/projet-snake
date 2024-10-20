#include "struct.h"

Mix_Chunk* sonPerte = NULL;
Mix_Chunk* sonVictoire = NULL;
Mix_Chunk* sonManger = NULL;
Mix_Chunk* sonJeu = NULL;
Mix_Chunk* sonDebut = NULL;

void initialiserSons() {
    sonPerte = Mix_LoadWAV("partie_perdu.wav");
    sonDebut = Mix_LoadWAV("partie_lancer.wav");
    sonManger = Mix_LoadWAV("cube_recup.wav");
    sonVictoire = Mix_LoadWAV("partie_gagné.wav");
    sonJeu = Mix_LoadWAV("bande_son.wav");
    if (!sonPerte || !sonDebut || !sonManger || !sonVictoire || !sonJeu) {
        fprintf(stderr, "Erreur lors du chargement des sons: %s\n", Mix_GetError());
        exit(EXIT_FAILURE);
    }
}

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
    int isHead = 1;

    while (temp != NULL) 
    {
        SDL_Rect snakebody = {temp->x, temp->y, temp->w, temp->h};

        if (isHead) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            isHead = 0;
        } else {
            SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
        }

        SDL_RenderFillRect(renderer, &snakebody);
        temp = temp->next;
    }
}

int collision(poserpent* tete, SDL_Rect* walls, int num_walls) {
    SDL_Rect snakeRect = { tete->x, tete->y, tete->w, tete->h };
    for (int i = 0; i < num_walls; i++) {
        if (SDL_HasIntersection(&snakeRect, &walls[i])) {
            printf("Collision avec le mur\n");
            return 1;
        }
    }

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

int nourriture_dans_serpent(food* f, poserpent* tete) {
    poserpent* current = tete;
    while (current != NULL) {
        if (f->x == current->x && f->y == current->y) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

void placer_nourriture(food* f, SDL_Rect* walls, int num_walls, poserpent* tete) {
    do {
        f->x = (rand() % 14 + 1) * 20;
        f->y = (rand() % 14 + 1) * 20;
        f->w = 20;
        f->h = 20;
    } while (nourriture_dans_mur(f, walls, num_walls) || nourriture_dans_serpent(f, tete));
}

void afficher_nourriture(SDL_Renderer* renderer, food* f) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_Rect nourriture = { f->x, f->y, f->w, f->h };
    SDL_RenderFillRect(renderer, &nourriture);
}

void placer_malus(malus* m, SDL_Rect* walls, int num_walls, poserpent* tete) {
    do {
        m->x = (rand() % 14 + 1) * 20; // Génère des positions comme 20, 40, 60, etc.
        m->y = (rand() % 14 + 1) * 20; // Génère des positions comme 20, 40, 60, etc.
        m->w = 20;
        m->h = 20;
    } while (nourriture_dans_mur((food*)m, walls, num_walls) || nourriture_dans_serpent((food*)m, tete));
}

void placer_bonus(bonus* b, SDL_Rect* walls, int num_walls, poserpent* tete) {
    do {
        b->x = (rand() % 14 + 1) * 20; // Génère des positions comme 20, 40, 60, etc.
        b->y = (rand() % 14 + 1) * 20; // Génère des positions comme 20, 40, 60, etc.
        b->w = 20;
        b->h = 20;
    } while (nourriture_dans_mur((food*)b, walls, num_walls) || nourriture_dans_serpent((food*)b, tete));
}

void afficher_malus(SDL_Renderer* renderer, malus* m) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect malus = { m->x, m->y, m->w, m->h };
    SDL_RenderFillRect(renderer, &malus);
}

void afficher_bonus(SDL_Renderer* renderer, bonus* b) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_Rect bonus = { b->x, b->y, b->w, b->h };
    SDL_RenderFillRect(renderer, &bonus);
}

int manger(poserpent* tete, food* f) {
    SDL_Rect snakeRect = { tete->x, tete->y, tete->w, tete->h };
    SDL_Rect foodRect = { f->x, f->y, f->w, f->h };
    
    return SDL_HasIntersection(&snakeRect, &foodRect);
}

int bonus_collision(poserpent* tete, bonus* b) {
    SDL_Rect snakeRect = { tete->x, tete->y, tete->w, tete->h };
    SDL_Rect bonusRect = { b->x, b->y, b->w, b->h };
    
    return SDL_HasIntersection(&snakeRect, &bonusRect);
}

int malus_collision(poserpent* tete, malus* m) {
    SDL_Rect snakeRect = { tete->x, tete->y, tete->w, tete->h };
    SDL_Rect malusRect = { m->x, m->y, m->w, m->h };
    
    return SDL_HasIntersection(&snakeRect, &malusRect);
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

    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
        printf("Erreur d'initialisation de SDL_mixer: %s\n", Mix_GetError());
        return 1;
    }

    initialiserSons();

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
    int score = 0;
    int speed = 200;
    food nourriture;
    bonus bonus_item;  // Déclaration correcte de bonus_item
    malus malus_item;  // Déclaration correcte de malus_item
    SDL_Rect walls[] = {
        { 0, 0, 320, 10 },
        { 0, 310, 320, 10 },
        { 0, 0, 10, 320 },
        { 310, 0, 10, 320 }
    };
    int num_walls = sizeof(walls) / sizeof(walls[0]);
    
    placer_nourriture(&nourriture, walls, num_walls, snake);
    placer_bonus(&bonus_item, walls, num_walls, snake);
    placer_malus(&malus_item, walls, num_walls, snake);

    SDL_Event event;

    Mix_PlayChannel(-1, sonDebut, 0); // Jouer le son de démarrage
    int jeuChannel = Mix_PlayChannel(-1, sonJeu, -1); // Jouer le son de jeu en boucle

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
                        if (dy == 20) {
                            break;
                        }
                        dx = 0;
                        dy = -20;
                        break;
                    case SDLK_DOWN:
                        if (dy == -20) {
                            break;
                        }
                        dx = 0;
                        dy = 20;
                        break;
                    case SDLK_LEFT:
                        if (dx == 20) {
                            break;
                        }
                        dx = -20;
                        dy = 0;
                        break;
                    case SDLK_RIGHT:
                        if (dx == -20) {
                            break;
                        }
                        dx = 20;
                        dy = 0;
                        break;
                    default:
                        break;
                }
            }
        }

        if (collision(snake, walls, num_walls))
        {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);

            Mix_HaltChannel(jeuChannel); // Arrêter le son de jeu
            Mix_PlayChannel(-1, sonPerte, 0); // Jouer le son de perte

            dx = 0;
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
                            placer_nourriture(&nourriture, walls, num_walls, snake);
                            placer_bonus(&bonus_item, walls, num_walls, snake);
                            placer_malus(&malus_item, walls, num_walls, snake);
                            Mix_PlayChannel(-1, sonDebut, 0); // Jouer le son de démarrage
                            jeuChannel = Mix_PlayChannel(-1, sonJeu, -1); // Rejouer le son de jeu en boucle
                            score = 0;
                            speed = 200;
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
            if (speed > 10) {
                speed -= 10;
            }
            score += 10;
            Mix_PlayChannel(-1, sonManger, 0); // Jouer le son de récupération de cube
            system("clear");
            printf("Score: %d\n", score);
            placer_nourriture(&nourriture, walls, num_walls, snake);
        }
        if (bonus_collision(snake, &bonus_item)) 
        {
            agrandir_serpent(snake);
            speed += 10;
            score += 20;
            system("clear");
            printf("Score: %d\n", score);
            placer_bonus(&bonus_item, walls, num_walls, snake);
        }

        if (malus_collision(snake, &malus_item))
        {
            speed -= 40;
            score -= 20;
            system("clear");
            printf("Score: %d\n", score);
            placer_malus(&malus_item, walls, num_walls, snake);
        }
        

        if (score == 2540)
        {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);

            Mix_HaltChannel(jeuChannel); // Arrêter le son de jeu
            Mix_PlayChannel(-1, sonVictoire, 0); // Jouer le son de victoire

            dx = 0;
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
                            placer_nourriture(&nourriture, walls, num_walls, snake);
                            placer_bonus(&bonus_item, walls, num_walls, snake);
                            placer_malus(&malus_item, walls, num_walls, snake);
                            score = 0;
                            victory_loop = 0;
                            Mix_PlayChannel(-1, sonDebut, 0); // Jouer le son de démarrage
                            jeuChannel = Mix_PlayChannel(-1, sonJeu, -1); // Rejouer le son de jeu en boucle
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

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        for (int i = 0; i < num_walls; i++) {
            SDL_RenderFillRect(renderer, &walls[i]);
        }

        afficher(renderer, snake);
        afficher_nourriture(renderer, &nourriture);
        afficher_bonus(renderer, &bonus_item);
        afficher_malus(renderer, &malus_item);

        SDL_RenderPresent(renderer);
        SDL_Delay(speed);
    }

    Mix_FreeChunk(sonManger);
    Mix_FreeChunk(sonDebut);
    Mix_FreeChunk(sonVictoire);
#include "struct.h"

Mix_Chunk* sonPerte = NULL;
Mix_Chunk* sonVictoire = NULL;
Mix_Chunk* sonManger = NULL;
Mix_Chunk* sonJeu = NULL;
Mix_Chunk* sonDebut = NULL;

void initialiserSons() {
    sonPerte = Mix_LoadWAV("partie_perdu.wav");
    sonDebut = Mix_LoadWAV("partie_lancer.wav");
    sonManger = Mix_LoadWAV("cube_recup.wav");
    sonVictoire = Mix_LoadWAV("partie_gagné.wav");
    sonJeu = Mix_LoadWAV("bande_son.wav");
    if (!sonPerte || !sonDebut || !sonManger || !sonVictoire || !sonJeu) {
        fprintf(stderr, "Erreur lors du chargement des sons: %s\n", Mix_GetError());
        exit(EXIT_FAILURE);
    }
}

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
    int isHead = 1;

    while (temp != NULL) 
    {
        SDL_Rect snakebody = {temp->x, temp->y, temp->w, temp->h};

        if (isHead) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            isHead = 0;
        } else {
            SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
        }

        SDL_RenderFillRect(renderer, &snakebody);
        temp = temp->next;
    }
}

int collision(poserpent* tete, SDL_Rect* walls, int num_walls) {
    SDL_Rect snakeRect = { tete->x, tete->y, tete->w, tete->h };
    for (int i = 0; i < num_walls; i++) {
        if (SDL_HasIntersection(&snakeRect, &walls[i])) {
            printf("Collision avec le mur\n");
            return 1;
        }
    }

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

int nourriture_dans_serpent(food* f, poserpent* tete) {
    poserpent* current = tete;
    while (current != NULL) {
        if (f->x == current->x && f->y == current->y) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

void placer_nourriture(food* f, SDL_Rect* walls, int num_walls, poserpent* tete) {
    do {
        f->x = (rand() % 14 + 1) * 20;
        f->y = (rand() % 14 + 1) * 20;
        f->w = 20;
        f->h = 20;
    } while (nourriture_dans_mur(f, walls, num_walls) || nourriture_dans_serpent(f, tete));
}

void afficher_nourriture(SDL_Renderer* renderer, food* f) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_Rect nourriture = { f->x, f->y, f->w, f->h };
    SDL_RenderFillRect(renderer, &nourriture);
}

void placer_malus(malus* m, SDL_Rect* walls, int num_walls, poserpent* tete) {
    do {
        m->x = (rand() % 14 + 1) * 20; // Génère des positions comme 20, 40, 60, etc.
        m->y = (rand() % 14 + 1) * 20; // Génère des positions comme 20, 40, 60, etc.
        m->w = 20;
        m->h = 20;
    } while (nourriture_dans_mur((food*)m, walls, num_walls) || nourriture_dans_serpent((food*)m, tete));
}

void placer_bonus(bonus* b, SDL_Rect* walls, int num_walls, poserpent* tete) {
    do {
        b->x = (rand() % 14 + 1) * 20; // Génère des positions comme 20, 40, 60, etc.
        b->y = (rand() % 14 + 1) * 20; // Génère des positions comme 20, 40, 60, etc.
        b->w = 20;
        b->h = 20;
    } while (nourriture_dans_mur((food*)b, walls, num_walls) || nourriture_dans_serpent((food*)b, tete));
}

void afficher_malus(SDL_Renderer* renderer, malus* m) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect malus = { m->x, m->y, m->w, m->h };
    SDL_RenderFillRect(renderer, &malus);
}

void afficher_bonus(SDL_Renderer* renderer, bonus* b) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_Rect bonus = { b->x, b->y, b->w, b->h };
    SDL_RenderFillRect(renderer, &bonus);
}

int manger(poserpent* tete, food* f) {
    SDL_Rect snakeRect = { tete->x, tete->y, tete->w, tete->h };
    SDL_Rect foodRect = { f->x, f->y, f->w, f->h };
    
    return SDL_HasIntersection(&snakeRect, &foodRect);
}

int bonus_collision(poserpent* tete, bonus* b) {
    SDL_Rect snakeRect = { tete->x, tete->y, tete->w, tete->h };
    SDL_Rect bonusRect = { b->x, b->y, b->w, b->h };
    
    return SDL_HasIntersection(&snakeRect, &bonusRect);
}

int malus_collision(poserpent* tete, malus* m) {
    SDL_Rect snakeRect = { tete->x, tete->y, tete->w, tete->h };
    SDL_Rect malusRect = { m->x, m->y, m->w, m->h };
    
    return SDL_HasIntersection(&snakeRect, &malusRect);
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

    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
        printf("Erreur d'initialisation de SDL_mixer: %s\n", Mix_GetError());
        return 1;
    }

    initialiserSons();

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
    int score = 0;
    int speed = 200;
    food nourriture;
    bonus bonus_item;  // Déclaration correcte de bonus_item
    malus malus_item;  // Déclaration correcte de malus_item
    SDL_Rect walls[] = {
        { 0, 0, 320, 10 },
        { 0, 310, 320, 10 },
        { 0, 0, 10, 320 },
        { 310, 0, 10, 320 }
    };
    int num_walls = sizeof(walls) / sizeof(walls[0]);
    
    placer_nourriture(&nourriture, walls, num_walls, snake);
    placer_bonus(&bonus_item, walls, num_walls, snake);
    placer_malus(&malus_item, walls, num_walls, snake);

    SDL_Event event;

    Mix_PlayChannel(-1, sonDebut, 0); // Jouer le son de démarrage
    int jeuChannel = Mix_PlayChannel(-1, sonJeu, -1); // Jouer le son de jeu en boucle

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
                        if (dy == 20) {
                            break;
                        }
                        dx = 0;
                        dy = -20;
                        break;
                    case SDLK_DOWN:
                        if (dy == -20) {
                            break;
                        }
                        dx = 0;
                        dy = 20;
                        break;
                    case SDLK_LEFT:
                        if (dx == 20) {
                            break;
                        }
                        dx = -20;
                        dy = 0;
                        break;
                    case SDLK_RIGHT:
                        if (dx == -20) {
                            break;
                        }
                        dx = 20;
                        dy = 0;
                        break;
                    default:
                        break;
                }
            }
        }

        if (collision(snake, walls, num_walls))
        {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);

            Mix_HaltChannel(jeuChannel); // Arrêter le son de jeu
            Mix_PlayChannel(-1, sonPerte, 0); // Jouer le son de perte

            dx = 0;
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
                            placer_nourriture(&nourriture, walls, num_walls, snake);
                            placer_bonus(&bonus_item, walls, num_walls, snake);
                            placer_malus(&malus_item, walls, num_walls, snake);
                            Mix_PlayChannel(-1, sonDebut, 0); // Jouer le son de démarrage
                            jeuChannel = Mix_PlayChannel(-1, sonJeu, -1); // Rejouer le son de jeu en boucle
                            score = 0;
                            speed = 200;
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
            if (speed > 10) {
                speed -= 10;
            }
            score += 10;
            Mix_PlayChannel(-1, sonManger, 0); // Jouer le son de récupération de cube
            system("clear");
            printf("Score: %d\n", score);
            placer_nourriture(&nourriture, walls, num_walls, snake);
        }
        if (bonus_collision(snake, &bonus_item)) 
        {
            agrandir_serpent(snake);
            speed += 10;
            score += 20;
            system("clear");
            printf("Score: %d\n", score);
            placer_bonus(&bonus_item, walls, num_walls, snake);
        }

        if (malus_collision(snake, &malus_item))
        {
            speed -= 40;
            score -= 20;
            system("clear");
            printf("Score: %d\n", score);
            placer_malus(&malus_item, walls, num_walls, snake);
        }
        

        if (score == 2540)
        {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);

            Mix_HaltChannel(jeuChannel); // Arrêter le son de jeu
            Mix_PlayChannel(-1, sonVictoire, 0); // Jouer le son de victoire

            dx = 0;
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
                            placer_nourriture(&nourriture, walls, num_walls, snake);
                            placer_bonus(&bonus_item, walls, num_walls, snake);
                            placer_malus(&malus_item, walls, num_walls, snake);
                            score = 0;
                            victory_loop = 0;
                            Mix_PlayChannel(-1, sonDebut, 0); // Jouer le son de démarrage
                            jeuChannel = Mix_PlayChannel(-1, sonJeu, -1); // Rejouer le son de jeu en boucle
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

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        for (int i = 0; i < num_walls; i++) {
            SDL_RenderFillRect(renderer, &walls[i]);
        }

        afficher(renderer, snake);
        afficher_nourriture(renderer, &nourriture);
        afficher_bonus(renderer, &bonus_item);
        afficher_malus(renderer, &malus_item);

        SDL_RenderPresent(renderer);
        SDL_Delay(speed);
    }

    Mix_FreeChunk(sonManger);
    Mix_FreeChunk(sonDebut);
    Mix_FreeChunk(sonVictoire);
    Mix_FreeChunk(sonPerte);
    Mix_FreeChunk(sonJeu);
    Mix_CloseAudio();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
 }
}