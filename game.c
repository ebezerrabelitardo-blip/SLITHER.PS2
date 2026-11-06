#include "game.h"
#include <math.h>
#include <string.h>

Game G;
float g_sens = 1.0f;
static uint32_t rs = 0x1234567u;
void rng_seed(uint32_t s) { rs = s ? s : 1u; }
static uint32_t rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
float frand(void) { return (float)(rnd() & 0xFFFFFF) / 16777216.0f; }
float rr(float a, float b) { return a + frand() * (b - a); }

const int SKIN_N[NUM_SKINS] = {1,1,1,1,1,1,1,1,2,4,6,2};
const uint8_t SKIN_RGB[NUM_SKINS][6][3] = {
    {{229,57,53}}, {{67,160,71}}, {{47,107,255}}, {{253,216,53}},
    {{156,39,176}}, {{38,198,218}}, {{251,140,0}}, {{236,64,122}},
    {{229,57,53},{255,255,255}},
    {{47,107,255},{253,216,53},{229,57,53},{255,255,255}},
    {{229,57,53},{251,140,0},{253,216,53},{67,160,71},{47,107,255},{156,39,176}},
    {{251,140,0},{255,255,255}}
};
static const char *BOT_NAMES[18] = {"MANCOW","CROSSFADE","BLIP","RUSTY","ZENO","AMEK","NINE","PIXEL",
    "SOMBRA","NAJA","JIBOIA","COBRA","LUME","ORBE","VICIO","TUPA","NEON","RAIO"};

static void rand_pos(float *x, float *y) {
    float d = rr(0.0f, ARENA_R * 0.75f), a = rr(0.0f, TAU_F);
    *x = cosf(a) * d; *y = sinf(a) * d;
}

void game_init(int mode, int skin, const char *nick) {
    static const int bots[3] = {8, 15, 4};
    memset(&G, 0, sizeof(G));
    G.mode = mode;
    for (int i = 0; i < 7; i++) {
        float a = rr(0.0f, TAU_F), d = rr(300.0f, ARENA_R - 500.0f);
        G.cl[i].x = cosf(a) * d; G.cl[i].y = sinf(a) * d; G.clr[i] = rr(200.0f, 380.0f);
    }
    for (int i = 0; i < TARGET_FOOD; i++) food_spawn_one();
    snake_init(&G.s[0], 0, skin, nick, 10.0f, rr(-300.0f, 300.0f), rr(-300.0f, 300.0f));
    int nb = bots[mode % 3];
    for (int i = 0; i < nb; i++) {
        float x, y; rand_pos(&x, &y);
        snake_init(&G.s[i + 1], 1, (i + 1 + skin) % NUM_SKINS, BOT_NAMES[i % 18], rr(10.0f, 60.0f), x, y);
    }
    G.ns = nb + 1;
    G.cam.x = G.s[0].seg[0].x; G.cam.y = G.s[0].seg[0].y; G.cam.z = 1.0f;
}

void game_update(float dt, float ta, int has_ta, int boost) {
    Snake *p = &G.s[0];
    if (!p->dead) { if (has_ta) p->ta = ta; p->boost = boost; }
    for (int i = 0; i < G.ns; i++) {
        Snake *s = &G.s[i];
        if (s->dead) {
            if (s->ai && (s->rt -= dt) < 0.0f) {
                char nm[16]; strcpy(nm, s->name);
                float x, y; rand_pos(&x, &y);
                snake_init(s, 1, s->skin, nm, rr(10.0f, 40.0f), x, y);
            }
            continue;
        }
        if (s->ai && (s->th -= dt) < 0.0f) { ai_think(i); s->th = 0.12f + frand() * 0.1f; }
        snake_step(s, dt);
    }
    collisions_run();
    for (int i = 0; i < 5 && G.nf < TARGET_FOOD; i++) food_spawn_one();
    if ((G.rkt -= dt) < 0.0f) {
        G.rkt = 0.3f; G.nrank = 0;
        for (int i = 0; i < G.ns; i++) {
            if (G.s[i].dead) continue;
            int j = G.nrank++;
            while (j > 0 && G.s[G.rank[j - 1]].mass < G.s[i].mass) { G.rank[j] = G.rank[j - 1]; j--; }
            G.rank[j] = i;
        }
    }
    camera_update(dt);
}
