#include "game.h"
#include <math.h>
/* IA: fugir de maiores, interceptar menores (cacador), buscar comida, explorar,
   evitar corpos/borda avaliando 9 direcoes candidatas. */
static float nb[3 * 800];
void ai_think(int idx) {
    Snake *s = &G.s[idx]; Vec2 h = s->seg[0]; float r = snake_radius(s->mass);
    int thr = -1, prey = -1; float thrd = 1e9f, preyd = 1e9f;
    for (int i = 0; i < G.ns; i++) {
        if (i == idx || G.s[i].dead) continue;
        Snake *o = &G.s[i];
        float dx = o->seg[0].x - h.x, dy = o->seg[0].y - h.y, d = sqrtf(dx * dx + dy * dy);
        if (o->mass > s->mass * 1.15f && d < 260.0f && d < thrd) { thr = i; thrd = d; }
        if (s->pers == 1 && o->mass < s->mass * 0.85f && d < 520.0f && d < preyd) { prey = i; preyd = d; }
    }
    float want = s->a; s->boost = 0;
    if (thr >= 0) {
        want = atan2f(h.y - G.s[thr].seg[0].y, h.x - G.s[thr].seg[0].x);
        if (thrd < 150.0f && s->mass > 30.0f) s->boost = 1;
    } else if (prey >= 0) {
        Snake *o = &G.s[prey]; float k = preyd * 0.5f + 70.0f;
        want = atan2f(o->seg[0].y + sinf(o->a) * k - h.y, o->seg[0].x + cosf(o->a) * k - h.x);
        if (preyd < 320.0f && s->mass > 45.0f) s->boost = 1;
    } else {
        float bs = 0.0f; int bf = -1;
        for (int i = 0; i < G.nf; i++) {
            float dx = G.f[i].x - h.x, dy = G.f[i].y - h.y;
            if (dx > 700.0f || dx < -700.0f || dy > 700.0f || dy < -700.0f) continue;
            float d = sqrtf(dx * dx + dy * dy); if (d > 700.0f) continue;
            float sc = (G.f[i].v + (G.f[i].v >= 10.0f ? 6.0f : 0.0f)) / (d + 40.0f);
            if (sc > bs) { bs = sc; bf = i; }
        }
        if (bf >= 0 && (s->pers != 2 || hypotf(G.f[bf].x - h.x, G.f[bf].y - h.y) < 160.0f))
            want = atan2f(G.f[bf].y - h.y, G.f[bf].x - h.x);
        else { s->wa += rr(-0.7f, 0.7f); want = s->wa; }
    }
    if (sqrtf(h.x * h.x + h.y * h.y) > ARENA_R - 320.0f) want = atan2f(-h.y, -h.x);
    int nn = 0;
    for (int i = 0; i < G.ns; i++) {
        if (i == idx || G.s[i].dead) continue;
        float rb = snake_radius(G.s[i].mass);
        for (int k = 0; k < G.s[i].n && nn < 800; k++) {
            float dx = G.s[i].seg[k].x - h.x, dy = G.s[i].seg[k].y - h.y;
            if (dx < 300.0f && dx > -300.0f && dy < 300.0f && dy > -300.0f) { nb[nn * 3] = G.s[i].seg[k].x; nb[nn * 3 + 1] = G.s[i].seg[k].y; nb[nn * 3 + 2] = rb; nn++; }
        }
    }
    float best = s->a, bsc = 1e9f, L = 70.0f + r * 3.0f;
    for (int k = -4; k <= 4; k++) {
        float c = s->a + k * 0.4f, pen = 0.0f;
        for (int j = 1; j <= 2; j++) {
            float px = h.x + cosf(c) * L * j * 0.5f, py = h.y + sinf(c) * L * j * 0.5f;
            if (px * px + py * py > (ARENA_R - 50.0f) * (ARENA_R - 50.0f)) pen += 900.0f;
            for (int i = 0; i < nn; i++) {
                float dx = nb[i * 3] - px, dy = nb[i * 3 + 1] - py, lim = r * 0.8f + nb[i * 3 + 2] + 12.0f;
                if (dx * dx + dy * dy < lim * lim) { pen += 1000.0f; break; }
            }
        }
        float da = c - want; while (da > PI_F) da -= TAU_F; while (da < -PI_F) da += TAU_F;
        float sc = pen + fabsf(da) * 40.0f + fabsf((float)k) * 2.0f;
        if (sc < bsc) { bsc = sc; best = c; }
    }
    s->ta = best;
}
