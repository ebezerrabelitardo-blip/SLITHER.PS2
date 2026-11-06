#ifndef GAME_H
#define GAME_H
#include <stdint.h>
/* Logica do jogo: sem dependencias de PS2 (testavel no PC). */
#define ARENA_R 3000.0f
#define MAX_SNAKES 16
#define MAX_SEGS 420
#define MAX_FOOD 1800
#define TARGET_FOOD 900
#define NUM_SKINS 12
#define PI_F 3.14159265f
#define TAU_F 6.2831853f

typedef struct { float x, y; } Vec2;
typedef struct { float x, y, v, ph; uint8_t r, g, b, kind; } Food;
typedef struct {
    Vec2 seg[MAX_SEGS]; int n;
    float a, ta, mass, bt, rt, th, wa;
    int skin, ai, pers, boost, on, dead, kills, by;
    char name[16];
} Snake;
typedef struct { float x, y, z; } Cam;
typedef struct {
    Snake s[MAX_SNAKES]; int ns;
    Food f[MAX_FOOD]; int nf;
    Vec2 cl[7]; float clr[7];
    int mode; Cam cam;
    int rank[MAX_SNAKES]; int nrank; float rkt;
    int fr, tot;                 /* posicao final do jogador ao morrer */
} Game;

extern Game G;
extern float g_sens;             /* sensibilidade do analogico (0.5 - 1.8) */
extern const uint8_t SKIN_RGB[NUM_SKINS][6][3];
extern const int SKIN_N[NUM_SKINS];

void  rng_seed(uint32_t s);
float frand(void);
float rr(float a, float b);

float snake_radius(float mass);
void  snake_init(Snake *s, int ai, int skin, const char *name, float mass, float x, float y);
void  snake_step(Snake *s, float dt);
void  snake_kill(int idx);

void  food_add(float x, float y, float v, int r, int g, int b);
void  food_spawn_one(void);
float food_radius(const Food *f);

void  ai_think(int idx);
void  collisions_run(void);
void  camera_update(float dt);

void  game_init(int mode, int skin, const char *nick);
void  game_update(float dt, float ta, int has_ta, int boost);
#endif
