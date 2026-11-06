#include "game.h"
#include <math.h>
static const uint8_t FP[7][3] = {{255,82,82},{255,235,59},{105,240,174},{64,196,255},{224,64,251},{255,171,64},{179,136,255}};
void food_add(float x, float y, float v, int r, int g, int b) {
    if (G.nf >= MAX_FOOD) return;
    Food *f = &G.f[G.nf++];
    f->x = x; f->y = y; f->v = v; f->r = (uint8_t)r; f->g = (uint8_t)g; f->b = (uint8_t)b;
    f->kind = v >= 10.0f ? 3 : v >= 4.0f ? 2 : v >= 2.0f ? 1 : 0;
    f->ph = frand() * 6.0f;
}
float food_radius(const Food *f) { static const float R[4] = {3.0f, 4.5f, 6.5f, 9.0f}; return R[f->kind]; }
void food_spawn_one(void) {
    const uint8_t *c = FP[(int)(frand() * 7.0f) % 7];
    if (frand() < 0.5f) {                                   /* regioes de concentracao */
        int k = (int)(frand() * 7.0f) % 7;
        float a = rr(0.0f, TAU_F), d = frand() * G.clr[k];
        float x = G.cl[k].x + cosf(a) * d, y = G.cl[k].y + sinf(a) * d;
        if (x * x + y * y < (ARENA_R - 30.0f) * (ARENA_R - 30.0f)) { food_add(x, y, frand() < 0.2f ? 2.0f : 1.0f, c[0], c[1], c[2]); return; }
    }
    float a = rr(0.0f, TAU_F), d = sqrtf(frand()) * (ARENA_R - 40.0f), q = frand();
    food_add(cosf(a) * d, sinf(a) * d, q < 0.65f ? 1.0f : q < 0.88f ? 2.0f : q < 0.98f ? 4.0f : 10.0f, c[0], c[1], c[2]);
}
