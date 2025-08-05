// dipendenza: libsdl1-dev (libreria grafica)
// compilazione: gcc -o dungeon-c dungeon.c -lSDL2 -lSDL2main -lm

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

// Costanti del gioco
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define MAP_WIDTH 80
#define MAP_HEIGHT 60
#define CELL_SIZE 10
#define MAX_BULLETS 50
#define MAX_ENEMIES 20

// Tipi di celle della mappa
typedef enum {
    WALL = 0,
    FLOOR = 1,
    ROOM = 2
} CellType;

// Struttura per posizioni
typedef struct {
    int x, y;
} Position;

// Struttura per i proiettili
typedef struct {
    Position pos;
    int dx, dy;  // direzione
    bool active;
} Bullet;

// Struttura per i nemici
typedef struct {
    Position pos;
    bool alive;
    int move_timer;
} Enemy;

// Variabili globali del gioco
CellType map[MAP_HEIGHT][MAP_WIDTH];
Position player;
Bullet bullets[MAX_BULLETS];
Enemy enemies[MAX_ENEMIES];
int enemy_count = 0;
Uint32 last_move_time = 0; // Per limitare la velocità di movimento

// Inizializza la mappa con muri
void init_map() {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            map[y][x] = WALL;
        }
    }
}

// Crea una stanza rettangolare
void create_room(int x, int y, int width, int height) {
    for (int ry = y; ry < y + height && ry < MAP_HEIGHT; ry++) {
        for (int rx = x; rx < x + width && rx < MAP_WIDTH; rx++) {
            if (rx >= 0 && ry >= 0) {
                map[ry][rx] = ROOM;
            }
        }
    }
}

// Crea un corridoio orizzontale
void create_h_corridor(int x1, int x2, int y) {
    int start = (x1 < x2) ? x1 : x2;
    int end = (x1 > x2) ? x1 : x2;
    for (int x = start; x <= end && x < MAP_WIDTH; x++) {
        if (x >= 0 && y >= 0 && y < MAP_HEIGHT) {
            map[y][x] = FLOOR;
        }
    }
}

// Crea un corridoio verticale
void create_v_corridor(int y1, int y2, int x) {
    int start = (y1 < y2) ? y1 : y2;
    int end = (y1 > y2) ? y1 : y2;
    for (int y = start; y <= end && y < MAP_HEIGHT; y++) {
        if (x >= 0 && y >= 0 && x < MAP_WIDTH) {
            map[y][x] = FLOOR;
        }
    }
}

// Genera la mappa procedurale
void generate_map() {
    init_map();
    
    // Inizializza i proiettili
    for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active = false;
    }
    
    // Inizializza i nemici
    enemy_count = 0;
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].alive = false;
    }
    
    // Crea stanze casuali
    int rooms[6][4]; // x, y, width, height
    int room_count = 4 + rand() % 3; // 4-6 stanze
    
    for (int i = 0; i < room_count; i++) {
        int width = 8 + rand() % 12;
        int height = 6 + rand() % 10;
        int x = 2 + rand() % (MAP_WIDTH - width - 4);
        int y = 2 + rand() % (MAP_HEIGHT - height - 4);
        
        rooms[i][0] = x;
        rooms[i][1] = y;
        rooms[i][2] = width;
        rooms[i][3] = height;
        
        create_room(x, y, width, height);
        
        // Aggiungi nemici nella stanza (evita i bordi)
        if (i > 0) { // Non nella prima stanza (dove spawna il player)
            int enemy_num = 1 + rand() % 3;
            for (int e = 0; e < enemy_num && enemy_count < MAX_ENEMIES; e++) {
                enemies[enemy_count].pos.x = x + 2 + rand() % (width - 4);
                enemies[enemy_count].pos.y = y + 2 + rand() % (height - 4);
                enemies[enemy_count].alive = true;
                enemies[enemy_count].move_timer = 0;
                enemy_count++;
            }
        }
    }
    
    // Connetti le stanze con corridoi
    for (int i = 0; i < room_count - 1; i++) {
        int x1 = rooms[i][0] + rooms[i][2] / 2;
        int y1 = rooms[i][1] + rooms[i][3] / 2;
        int x2 = rooms[i+1][0] + rooms[i+1][2] / 2;
        int y2 = rooms[i+1][1] + rooms[i+1][3] / 2;
        
        // Corridoio a L
        create_h_corridor(x1, x2, y1);
        create_v_corridor(y1, y2, x2);
    }
    
    // Posiziona il player nella prima stanza
    player.x = rooms[0][0] + rooms[0][2] / 2;
    player.y = rooms[0][1] + rooms[0][3] / 2;
}

// Controlla se una posizione è camminabile
bool is_walkable(int x, int y) {
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) {
        return false;
    }
    return map[y][x] != WALL;
}

// Aggiorna i proiettili
void update_bullets() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) continue;
        
        // Muovi il proiettile
        bullets[i].pos.x += bullets[i].dx;
        bullets[i].pos.y += bullets[i].dy;
        
        // Controlla collisioni con muri
        if (!is_walkable(bullets[i].pos.x, bullets[i].pos.y)) {
            bullets[i].active = false;
            continue;
        }
        
        // Controlla collisioni con nemici
        for (int e = 0; e < enemy_count; e++) {
            if (enemies[e].alive && 
                enemies[e].pos.x == bullets[i].pos.x && 
                enemies[e].pos.y == bullets[i].pos.y) {
                enemies[e].alive = false;
                bullets[i].active = false;
                break;
            }
        }
    }
}

// Spara un proiettile
void shoot_bullet(int dx, int dy) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            bullets[i].pos.x = player.x;
            bullets[i].pos.y = player.y;
            bullets[i].dx = dx;
            bullets[i].dy = dy;
            bullets[i].active = true;
            break;
        }
    }
}

// Aggiorna i nemici (movimento semplice)
void update_enemies() {
    for (int i = 0; i < enemy_count; i++) {
        if (!enemies[i].alive) continue;
        
        enemies[i].move_timer++;
        if (enemies[i].move_timer > 60) { // Muovi ogni 60 frame (~1 secondo)
            enemies[i].move_timer = 0;
            
            // Movimento casuale
            int dx = (rand() % 3) - 1; // -1, 0, 1
            int dy = (rand() % 3) - 1;
            
            int new_x = enemies[i].pos.x + dx;
            int new_y = enemies[i].pos.y + dy;
            
            if (is_walkable(new_x, new_y)) {
                // Controlla che non ci sia il player nella nuova posizione
                if (!(new_x == player.x && new_y == player.y)) {
                    enemies[i].pos.x = new_x;
                    enemies[i].pos.y = new_y;
                }
            }
        }
    }
}

// Gestisce l'input
void handle_input(SDL_Event* e, bool* running) {
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    
    while (SDL_PollEvent(e)) {
        if (e->type == SDL_QUIT) {
            *running = false;
        }
        
        // Sparo con frecce direzionali (solo al keydown per evitare spam)
        if (e->type == SDL_KEYDOWN) {
            switch (e->key.keysym.sym) {
                case SDLK_UP:    shoot_bullet(0, -1); break;
                case SDLK_DOWN:  shoot_bullet(0, 1);  break;
                case SDLK_LEFT:  shoot_bullet(-1, 0); break;
                case SDLK_RIGHT: shoot_bullet(1, 0);  break;
                case SDLK_r:     // Tasto R per rigenerare la mappa
                    generate_map();
                    break;
            }
        }
    }
    
    // Movimento continuo con WASD ma limitato nel tempo
    Uint32 current_time = SDL_GetTicks();
    if (current_time - last_move_time > 100) { // Muovi ogni 100ms
        int new_x = player.x;
        int new_y = player.y;
        bool moved = false;
        
        if (keys[SDL_SCANCODE_W]) { new_y--; moved = true; }
        else if (keys[SDL_SCANCODE_S]) { new_y++; moved = true; }
        else if (keys[SDL_SCANCODE_A]) { new_x--; moved = true; }
        else if (keys[SDL_SCANCODE_D]) { new_x++; moved = true; }
        
        if (moved && is_walkable(new_x, new_y)) {
            // Controlla collisioni con nemici
            bool can_move = true;
            for (int i = 0; i < enemy_count; i++) {
                if (enemies[i].alive && enemies[i].pos.x == new_x && enemies[i].pos.y == new_y) {
                    can_move = false;
                    break;
                }
            }
            
            if (can_move) {
                player.x = new_x;
                player.y = new_y;
                last_move_time = current_time;
            }
        }
    }
}

// Rendering della mappa e oggetti
void render(SDL_Renderer* renderer) {
    // Sfondo nero
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    // Disegna la mappa
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            SDL_Rect rect = {x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
            
            switch (map[y][x]) {
                case WALL:
                    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // Grigio
                    break;
                case FLOOR:
                    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255); // Grigio chiaro
                    break;
                case ROOM:
                    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // Grigio molto chiaro
                    break;
            }
            SDL_RenderFillRect(renderer, &rect);
        }
    }
    
    // Disegna i nemici
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Rosso
    for (int i = 0; i < enemy_count; i++) {
        if (enemies[i].alive) {
            SDL_Rect rect = {
                enemies[i].pos.x * CELL_SIZE, 
                enemies[i].pos.y * CELL_SIZE, 
                CELL_SIZE, 
                CELL_SIZE
            };
            SDL_RenderFillRect(renderer, &rect);
        }
    }
    
    // Disegna i proiettili
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Giallo
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            SDL_Rect rect = {
                bullets[i].pos.x * CELL_SIZE + CELL_SIZE/3, 
                bullets[i].pos.y * CELL_SIZE + CELL_SIZE/3, 
                CELL_SIZE/3, 
                CELL_SIZE/3
            };
            SDL_RenderFillRect(renderer, &rect);
        }
    }
    
    // Disegna il player
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Verde
    SDL_Rect player_rect = {
        player.x * CELL_SIZE, 
        player.y * CELL_SIZE, 
        CELL_SIZE, 
        CELL_SIZE
    };
    SDL_RenderFillRect(renderer, &player_rect);
    
    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
    // Inizializza SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Errore SDL: %s\n", SDL_GetError());
        return 1;
    }
    
    // Crea finestra
    SDL_Window* window = SDL_CreateWindow(
        "Dungeon Explorer 2D",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    
    if (!window) {
        printf("Errore creazione finestra: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    
    // Crea renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Errore creazione renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    // Inizializza il generatore di numeri casuali
    srand(time(NULL));
    
    // Genera la mappa
    generate_map();
    
    // Game loop principale
    bool running = true;
    SDL_Event e;
    Uint32 last_time = SDL_GetTicks();
    
    while (running) {
        Uint32 current_time = SDL_GetTicks();
        
        // Limita a ~60 FPS
        if (current_time - last_time < 16) {
            SDL_Delay(16 - (current_time - last_time));
        }
        last_time = current_time;
        
        // Input
        handle_input(&e, &running);
        
        // Aggiornamenti
        update_bullets();
        update_enemies();
        
        // Rendering
        render(renderer);
    }
    
    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}
