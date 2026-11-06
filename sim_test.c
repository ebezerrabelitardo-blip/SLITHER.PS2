/* Teste headless da logica (roda no PC): gcc -Iinclude tests/sim_test.c src/{game,snake,ai,collision,particles,camera}.c -lm */
#include "game.h"
#include <stdio.h>
#include <math.h>
static int fails = 0;
#define CHECK(c, msg) do { if (c) printf("  [OK]   %s\n", msg); else { printf("  [FALHA] %s\n", msg); fails++; } } while (0)
int main(void) {
    rng_seed(42);
    printf("1) Crescimento\n");
    game_init(1, 0, "TESTE");
    Snake *p = &G.s[0]; float m0 = p->mass; int n0 = G.nf;
    food_add(p->seg[0].x, p->seg[0].y, 4.0f, 255, 0, 0);
    game_update(1.0f / 60.0f, 0, 0, 0);
    CHECK(p->mass > m0 + 3.0f, "comer particula aumenta a massa");
    CHECK(G.nf <= MAX_FOOD, "limite de particulas respeitado");
    (void)n0;
    printf("2) Aceleracao\n");
    for (int i = 1; i < G.ns; i++) G.s[i].dead = 1, G.s[i].ai = 0;
    G.nf = 0; p->mass = 100.0f; p->boost = 1; float mm = p->mass;
    for (int i = 0; i < 120; i++) snake_step(p, 1.0f / 60.0f);
    CHECK(p->mass < mm - 10.0f, "aceleracao reduz a massa (~7/s)");
    CHECK(G.nf >= 10, "massa perdida vira particulas coletaveis");
    printf("3) Colisoes\n");
    game_init(0, 0, "TESTE");
    for (int i = 1; i < G.ns; i++) G.s[i].dead = 1, G.s[i].ai = 0;
    p = &G.s[0]; p->boost = 0;
    Snake *b = &G.s[1]; b->dead = 0; b->mass = 50.0f;
    for (int i = 0; i < b->n; i++) { b->seg[i].x = 500.0f + i * 5.0f; b->seg[i].y = 500.0f; }
    p->seg[0].x = 500.0f; p->seg[0].y = 500.0f; p->mass = 20.0f;
    int nf2 = G.nf; collisions_run();
    CHECK(p->dead == 1, "cabeca tocando corpo de outra cobra elimina");
    CHECK(G.nf > nf2, "corpo eliminado vira particulas");
    game_init(0, 0, "TESTE"); p = &G.s[0];
    for (int i = 1; i < G.ns; i++) G.s[i].dead = 1, G.s[i].ai = 0;
    for (int i = 0; i < p->n; i++) { p->seg[i].x = 100.0f; p->seg[i].y = 100.0f; }
    collisions_run();
    CHECK(p->dead == 0, "colidir com o proprio corpo NAO elimina");
    p->seg[0].x = ARENA_R + 10.0f; p->seg[0].y = 0.0f; collisions_run();
    CHECK(p->dead == 1, "limite da arena elimina");
    printf("4) Simulacao longa com IA (16 cobras, 5 min de jogo)\n");
    game_init(1, 3, "TESTE");
    int bad = 0, maxn = 0; int respawns = 0;
    for (int f = 0; f < 18000; f++) {
        float ta = 0.5f * sinf(f * 0.01f);
        game_update(1.0f / 60.0f, ta, 1, (f / 300) % 2);
        if (G.s[0].dead) G.s[0].dead = 0, G.s[0].mass = 10.0f;
        for (int i = 0; i < G.ns; i++) {
            Snake *s = &G.s[i];
            if (!isfinite(s->mass) || !isfinite(s->seg[0].x) || !isfinite(s->seg[0].y) || s->mass < -1.0f) bad++;
            if (s->n > maxn) maxn = s->n;
            if (s->n > MAX_SEGS) bad++;
            if (s->ai && s->dead && s->rt < 0.0f) respawns++;
        }
        if (G.nf > MAX_FOOD || G.nf < 0) bad++;
    }
    int alive = 0; for (int i = 1; i < G.ns; i++) if (!G.s[i].dead) alive++;
    float top = 0; for (int i = 0; i < G.ns; i++) if (G.s[i].mass > top) top = G.s[i].mass;
    printf("  bots vivos: %d/%d, maior massa: %.0f, maior corpo: %d seg, particulas: %d\n", alive, G.ns - 1, top, maxn, G.nf);
    CHECK(bad == 0, "sem NaN, estouro de segmentos ou de particulas");
    CHECK(alive >= G.ns / 2, "IA sobrevive e renasce (maioria viva ao final)");
    CHECK(top > 40.0f, "cobras crescem durante a partida");
    printf("5) Memoria: Game = %u KB (RAM do PS2: 32768 KB)\n", (unsigned)(sizeof(Game) / 1024));
    printf(fails ? "\nRESULTADO: %d FALHA(S)\n" : "\nRESULTADO: TODOS OS TESTES PASSARAM\n", fails);
    return fails != 0;
}
