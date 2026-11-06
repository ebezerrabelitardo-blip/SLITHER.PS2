#include "game.h"
#include <math.h>
/* Cabeca de A vs corpo de B. Colidir com o proprio corpo NAO elimina. */
void collisions_run(void) {
    int victim[MAX_SNAKES], killer[MAX_SNAKES], nv = 0;
    for (int i = 0; i < G.ns; i++) {
        Snake *a = &G.s[i]; if (a->dead) continue;
        Vec2 h = a->seg[0]; float ra = snake_radius(a->mass);
        if (sqrtf(h.x * h.x + h.y * h.y) > ARENA_R - ra * 0.5f) { victim[nv] = i; killer[nv++] = -1; continue; }
        for (int j = 0; j < G.ns; j++) {
            if (j == i) continue;
            Snake *b = &G.s[j]; if (b->dead) continue;
            float rb = snake_radius(b->mass);
            float reach = b->n * rb * 0.55f + (ra + rb) * 2.0f;       /* descarte rapido */
            float hx = b->seg[0].x - h.x, hy = b->seg[0].y - h.y;
            if (hx * hx + hy * hy > reach * reach) continue;
            float lim = ra * 0.7f + rb * 0.85f; lim *= lim;
            int hit = 0;
            for (int k = 0; k < b->n; k++) {
                float dx = b->seg[k].x - h.x, dy = b->seg[k].y - h.y;
                if (dx * dx + dy * dy < lim) { hit = 1; break; }
            }
            if (hit) { victim[nv] = i; killer[nv++] = j; break; }
        }
    }
    for (int k = 0; k < nv; k++) {
        if (killer[k] >= 0) G.s[killer[k]].kills++;
        snake_kill(victim[k]);
    }
}
