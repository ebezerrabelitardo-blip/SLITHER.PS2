#include "game.h"
#include <math.h>
#include <string.h>

float snake_radius(float m) { return 6.0f + sqrtf(m) * 0.6f; }

void snake_init(Snake *s, int ai, int skin, const char *name, float mass, float x, float y) {
    memset(s, 0, sizeof(*s));
    s->a = s->ta = s->wa = rr(0.0f, TAU_F);
    s->mass = mass; s->skin = skin; s->ai = ai; s->pers = (int)(frand() * 3.0f) % 3; s->by = -1;
    strncpy(s->name, name, 15); s->name[15] = 0;
    s->n = 14;
    for (int i = 0; i < 14; i++) { s->seg[i].x = x - cosf(s->a) * i * 5.0f; s->seg[i].y = y - sinf(s->a) * i * 5.0f; }
}

void snake_step(Snake *s, float dt) {
    float r = snake_radius(s->mass);
    Vec2 *h = &s->seg[0];
    float da = s->ta - s->a;
    while (da > PI_F) da -= TAU_F;
    while (da < -PI_F) da += TAU_F;
    float mt = (s->ai ? 5.0f : 5.2f * g_sens) / (1.0f + r / 16.0f) * dt;
    s->a += da > mt ? mt : da < -mt ? -mt : da;
    s->on = (s->boost && s->mass > 12.0f) ? 1 : 0;
    if (s->on) {                                   /* acelerar consome massa e solta particulas */
        s->mass -= 7.0f * dt; s->bt += dt;
        if (s->bt > 0.14f) {
            s->bt = 0.0f; Vec2 *t = &s->seg[s->n - 1];
            const uint8_t *c = SKIN_RGB[s->skin][0];
            food_add(t->x, t->y, 1.0f, c[0], c[1], c[2]);
        }
    }
    float v = s->on ? 250.0f : 130.0f;
    h->x += cosf(s->a) * v * dt; h->y += sinf(s->a) * v * dt;
    float sp = r * 0.55f;
    for (int i = 1; i < s->n; i++) {
        float dx = s->seg[i-1].x - s->seg[i].x, dy = s->seg[i-1].y - s->seg[i].y;
        float d = sqrtf(dx * dx + dy * dy);
        if (d > sp) { float k = (d - sp) / d; s->seg[i].x += dx * k; s->seg[i].y += dy * k; }
    }
    int want = (int)(10.0f + s->mass * 0.45f); if (want > MAX_SEGS) want = MAX_SEGS;
    while (s->n < want) { s->seg[s->n] = s->seg[s->n - 1]; s->n++; }
    while (s->n > want && s->n > 8) s->n--;
    /* coleta com leve atracao magnetica */
    float er = (r + 14.0f) * (r + 14.0f), mg = (r + 50.0f) * (r + 50.0f);
    for (int i = G.nf - 1; i >= 0; i--) {
        Food *f = &G.f[i];
        float dx = h->x - f->x, dy = h->y - f->y, d2 = dx * dx + dy * dy;
        if (d2 < mg) {
            float k = 9.0f * dt > 1.0f ? 1.0f : 9.0f * dt;
            f->x += dx * k; f->y += dy * k;
            if (d2 < er) { s->mass += f->v; G.f[i] = G.f[G.nf - 1]; G.nf--; }
        }
    }
}

void snake_kill(int idx) {
    Snake *s = &G.s[idx];
    if (s->dead) return;
    if (idx == 0) {
        int better = 0, alive = 0;
        for (int i = 0; i < G.ns; i++) if (!G.s[i].dead) { alive++; if (G.s[i].mass > s->mass) better++; }
        G.fr = better + 1; G.tot = alive;
    }
    float v = s->mass / (s->n / 2.0f) * 0.9f; if (v < 1.0f) v = 1.0f; if (v > 6.0f) v = 6.0f;
    int nc = SKIN_N[s->skin];
    for (int i = 0; i < s->n; i += 2) {
        const uint8_t *c = SKIN_RGB[s->skin][i % nc];
        food_add(s->seg[i].x + rr(-6.0f, 6.0f), s->seg[i].y + rr(-6.0f, 6.0f), v, c[0], c[1], c[2]);
    }
    s->dead = 1; s->rt = 3.0f;
}
