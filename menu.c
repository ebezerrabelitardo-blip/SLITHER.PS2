/* Menus no estilo da referencia: fundo azul/roxo com raios, mascote laranja, botoes pilula. */
#include <math.h>
#include <stdio.h>
#include "game.h"
#include "render.h"
#include "menu.h"

int g_mode = 1, g_skin = 0, g_recs[5];
static int sel = 0;
static const char *MODE_NAME[3] = {"SOBREVIVENCIA", "CONTRA IA", "RECORDE"};

void records_add(int score) {
    for (int i = 0; i < 5; i++) if (score > g_recs[i]) { for (int j = 4; j > i; j--) g_recs[j] = g_recs[j - 1]; g_recs[i] = score; return; }
}

static void pill(float cx, float cy, float w, float h, int r, int g, int b, int sl, const char *t, float ts) {
    float rad = h * 0.5f;
    if (sl) { r_circle(cx - w * 0.5f, cy, rad + 4, 255, 255, 255, 0x80); r_circle(cx + w * 0.5f, cy, rad + 4, 255, 255, 255, 0x80); r_rect(cx - w * 0.5f, cy - rad - 4, cx + w * 0.5f, cy + rad + 4, 255, 255, 255, 0x80); }
    r_circle(cx - w * 0.5f, cy, rad, r, g, b, 0x80); r_circle(cx + w * 0.5f, cy, rad, r, g, b, 0x80);
    r_rect(cx - w * 0.5f, cy - rad, cx + w * 0.5f, cy + rad, r, g, b, 0x80);
    r_rect(cx - w * 0.5f, cy - rad, cx + w * 0.5f, cy - rad * 0.2f, 255, 255, 255, 0x18);
    r_text(t, cx + 1, cy - 3.5f * ts + 2, ts, 20, 66, 12, 0x80, 1);
    r_text(t, cx, cy - 3.5f * ts, ts, 125, 255, 82, 0x80, 1);
}

static void sunburst(float cx, float cy) {
    for (int i = 0; i < 28; i++) {
        float a = i * TAU_F / 28.0f + g_frame * 0.0006f, a2 = a + TAU_F / 56.0f;
        r_tri(cx, cy, cx + cosf(a) * 800, cy + sinf(a) * 800, cx + cosf(a2) * 800, cy + sinf(a2) * 800, 255, 255, 255, 0x0C);
    }
}

static void mascot(float cx, float cy, float u) {
    static const float P[4][2] = {{0.4f, -3.4f}, {2.6f, -3.6f}, {3.4f, -0.6f}, {-0.6f, 0.2f}};
    float hx = 0, hy = 0, bob = sinf(g_frame * 0.05f) * u * 0.08f;
    for (int i = 0; i <= 40; i++) {
        float t = i / 40.0f, m = 1 - t;
        float x = m*m*m*P[0][0] + 3*m*m*t*P[1][0] + 3*m*t*t*P[2][0] + t*t*t*P[3][0];
        float y = m*m*m*P[0][1] + 3*m*m*t*P[1][1] + 3*m*t*t*P[2][1] + t*t*t*P[3][1];
        float r = u * (0.22f + 0.8f * t); hx = cx + x * u; hy = cy + y * u + bob;
        if (i == 40) r_circle(hx, hy, r, 255, 179, 71, 0x80); else if (i & 1) r_circle(hx, hy, r, 255, 138, 31, 0x80); else r_circle(hx, hy, r, 245, 122, 16, 0x80);
        r_circle(hx - r * 0.2f, hy - r * 0.25f, r * 0.58f, 255, 255, 255, 0x1C);
    }
    static const float E[2][2] = {{0.35f, 0.1f}, {0.85f, -0.2f}};
    for (int e = 0; e < 2; e++) {
        r_circle(hx + E[e][0] * u, hy + E[e][1] * u, u * 0.38f, 255, 255, 255, 0x80);
        r_circle(hx + E[e][0] * u + u * 0.08f, hy + E[e][1] * u + u * 0.04f, u * 0.2f, 15, 15, 15, 0x80);
    }
}

static void logo(float cx, float cy, float sc) {
    static const int C[4][3] = {{255,213,79},{182,255,106},{110,231,255},{255,138,230}};
    const char *t = "SLITHER.PS2"; float x0 = cx - r_text_w(t, sc) * 0.5f; char one[2] = {0, 0};
    for (int pass = 0; pass < 2; pass++)
        for (int i = 0; t[i]; i++) {
            one[0] = t[i]; float x = x0 + i * 6 * sc, y = cy - 3.5f * sc;
            if (pass == 0) { for (int dx = -1; dx <= 1; dx++) for (int dy = -1; dy <= 1; dy++) if (dx || dy) r_text(one, x + dx * sc * 0.7f, y + dy * sc * 0.7f, sc, 26, 16, 56, 0x80, 0); }
            else r_text(one, x, y, sc, C[i % 4][0], C[i % 4][1], C[i % 4][2], 0x80, 0);
        }
}

static void preview(float y0, float r) {
    int nc = SKIN_N[g_skin]; float x = 0, y = 0;
    for (int i = 0; i < 36; i++) {
        x = r_w() * 0.5f + (i - 18) * r * 0.62f; y = y0 + sinf(i * 0.28f + g_frame * 0.04f) * r * 1.1f;
        const uint8_t *c = SKIN_RGB[g_skin][nc > 1 ? (i / 3) % nc : 0];
        r_circle(x, y, r, c[0], c[1], c[2], 0x80); r_circle(x - r * 0.2f, y - r * 0.25f, r * 0.58f, 255, 255, 255, 0x1C);
    }
    for (int e = -1; e <= 1; e += 2) { r_circle(x + r * 0.45f, y + e * r * 0.5f, r * 0.42f, 255, 255, 255, 0x80); r_circle(x + r * 0.6f, y + e * r * 0.5f, r * 0.22f, 0, 0, 0, 0x80); }
}

State menu_run(State st, const Pad *p) {
    float W = (float)r_w(), H = (float)r_h(); char b[48];
    int pr = p->pressed;
    switch (st) {
    case ST_MENU: {
        r_gradient(127, 176, 245, 42, 31, 143); sunburst(W * 0.5f, H * 0.3f);
        mascot(W * 0.5f, H * 0.34f, H / 13.0f); logo(W * 0.5f, H * 0.2f, 6.0f);
        r_circle(W * 0.5f - 80, H * 0.60f, 14, 255, 255, 255, 0x20); r_circle(W * 0.5f + 80, H * 0.60f, 14, 255, 255, 255, 0x20);
        r_rect(W * 0.5f - 80, H * 0.60f - 14, W * 0.5f + 80, H * 0.60f + 14, 255, 255, 255, 0x20);
        r_text("JOGADOR", W * 0.5f, H * 0.60f - 5, 1.8f, 255, 255, 255, 0x60, 1);
        pill(W * 0.5f, H * 0.72f, 190, 46, 20, 110, 200, sel == 1, "JOGAR ONLINE", 2.2f);
        pill(W * 0.5f, H * 0.85f, 110, 30, 100, 80, 220, sel == 0, "JOGAR VS IA", 1.6f);
        r_text("TROCAR COBRA", 20, H - 28, sel == 2 ? 2.2f : 1.8f, sel == 2 ? 255 : 125, 255, sel == 2 ? 255 : 82, 0x80, 0);
        r_text("CONFIG", W - 20, 18, sel == 3 ? 2.2f : 1.8f, sel == 3 ? 255 : 125, 255, sel == 3 ? 255 : 82, 0x80, 2);
        snprintf(b, sizeof b, "MODO: %s", MODE_NAME[g_mode]);
        r_text(b, W - 20, H - 28, sel == 4 ? 2.2f : 1.8f, sel == 4 ? 255 : 125, 255, sel == 4 ? 255 : 82, 0x80, 2);
        static const int order[5] = {0, 1, 2, 3, 4};
        if (pr & B_DOWN) sel = order[(sel + 1) % 5];
        if (pr & B_UP) sel = order[(sel + 4) % 5];
        if (sel == 4 && (pr & (B_LEFT | B_RIGHT))) g_mode = (g_mode + ((pr & B_RIGHT) ? 1 : 2)) % 3;
        if (pr & B_X) {
            if (sel == 0) { game_init(g_mode, g_skin, "VOCE"); return ST_PLAY; }
            if (sel == 1) return ST_ONLINE;
            if (sel == 2) return ST_SKINS;
            if (sel == 3) return ST_SETTINGS;
            if (sel == 4) g_mode = (g_mode + 1) % 3;
        }
        return ST_MENU;
    }
    case ST_SKINS: {
        r_gradient(22, 26, 40, 10, 12, 20);
        r_text("TROCAR COBRA", W * 0.5f, 24, 3.0f, 125, 255, 82, 0x80, 1);
        for (int i = 0; i < NUM_SKINS; i++) {
            float x = W * 0.5f + ((i % 6) - 2.5f) * 62.0f, y = 100.0f + (i / 6) * 62.0f; const uint8_t *c = SKIN_RGB[i][0];
            if (i == g_skin) r_circle(x, y, 29, 255, 255, 255, 0x80);
            r_circle(x, y, 24, c[0], c[1], c[2], 0x80);
            if (SKIN_N[i] > 1) { const uint8_t *d = SKIN_RGB[i][1]; r_circle(x, y, 12, d[0], d[1], d[2], 0x80); }
            r_circle(x - 6, y - 8, 14, 255, 255, 255, 0x20);
        }
        preview(H * 0.68f, 13.0f);
        pill(W * 0.5f, H - 34, 60, 30, 40, 40, 140, 1, "OK", 2.0f);
        if ((pr & B_RIGHT) && g_skin < NUM_SKINS - 1) g_skin++;
        if ((pr & B_LEFT) && g_skin > 0) g_skin--;
        if ((pr & B_DOWN) && g_skin + 6 < NUM_SKINS) g_skin += 6;
        if ((pr & B_UP) && g_skin >= 6) g_skin -= 6;
        return (pr & (B_X | B_O)) ? ST_MENU : ST_SKINS;
    }
    case ST_SETTINGS: {
        r_gradient(22, 26, 40, 10, 12, 20);
        r_text("CONFIGURACOES", W * 0.5f, 24, 3.0f, 125, 255, 82, 0x80, 1);
        r_text("SENSIBILIDADE DO ANALOGICO (ESQ/DIR)", 40, 80, 1.5f, 255, 255, 255, 0x80, 0);
        r_rect(40, 104, W - 40, 114, 255, 255, 255, 0x30);
        r_rect(40, 104, 40 + (W - 80) * (g_sens - 0.5f) / 1.3f, 114, 125, 255, 82, 0x80);
        r_text("RECORDES", 40, 150, 1.5f, 255, 255, 255, 0x80, 0);
        for (int i = 0; i < 5; i++) { snprintf(b, sizeof b, "#%d  %d", i + 1, g_recs[i]); r_text(b, 40, 176 + i * 20, 1.8f, 106, 168, 255, 0x80, 0); }
        r_text("CREDITOS: RECRIACAO INDEPENDENTE", 40, H - 66, 1.3f, 255, 255, 255, 0x60, 0);
        r_text("SEM RECURSOS DE TERCEIROS", 40, H - 50, 1.3f, 255, 255, 255, 0x60, 0);
        r_text("O: VOLTAR", W - 20, H - 28, 1.8f, 125, 255, 82, 0x80, 2);
        if ((pr & B_RIGHT) && g_sens < 1.79f) g_sens += 0.1f;
        if ((pr & B_LEFT) && g_sens > 0.51f) g_sens -= 0.1f;
        return (pr & B_O) ? ST_MENU : ST_SETTINGS;
    }
    case ST_ONLINE:
        r_gradient(22, 26, 40, 10, 12, 20);
        r_text("JOGAR ONLINE", W * 0.5f, 90, 3.0f, 125, 255, 82, 0x80, 1);
        r_text("REQUER ADAPTADOR DE REDE E SERVIDOR", W * 0.5f, 170, 1.6f, 255, 255, 255, 0x80, 1);
        r_text("DISPONIVEL EM UMA VERSAO FUTURA", W * 0.5f, 196, 1.6f, 255, 255, 255, 0x80, 1);
        r_text("O: VOLTAR", W * 0.5f, H - 40, 2.0f, 125, 255, 82, 0x80, 1);
        return (pr & B_O) ? ST_MENU : ST_ONLINE;
    case ST_PAUSE:
        r_rect(0, 0, W, H, 10, 8, 40, 0x50);
        r_text("PAUSADO", W * 0.5f, H * 0.35f, 4.0f, 125, 255, 82, 0x80, 1);
        r_text("START: CONTINUAR", W * 0.5f, H * 0.52f, 2.0f, 255, 255, 255, 0x80, 1);
        r_text("O: VOLTAR AO MENU", W * 0.5f, H * 0.60f, 2.0f, 255, 255, 255, 0x80, 1);
        r_text("SELECT: INFORMACOES", W * 0.5f, H * 0.68f, 2.0f, 255, 255, 255, 0x80, 1);
        if (pr & B_START) return ST_PLAY;
        return (pr & B_O) ? ST_MENU : ST_PAUSE;
    case ST_DEAD:
        r_rect(0, 0, W, H, 10, 8, 40, 0x60);
        r_text("VOCE FOI ELIMINADO", W * 0.5f, H * 0.2f, 3.0f, 255, 90, 90, 0x80, 1);
        snprintf(b, sizeof b, "PONTUACAO: %d", (int)G.s[0].mass); r_text(b, W * 0.5f, H * 0.38f, 2.2f, 255, 255, 255, 0x80, 1);
        snprintf(b, sizeof b, "MAIOR RECORDE: %d", g_recs[0]); r_text(b, W * 0.5f, H * 0.47f, 2.2f, 106, 168, 255, 0x80, 1);
        snprintf(b, sizeof b, "POSICAO: %d DE %d", G.fr, G.tot); r_text(b, W * 0.5f, H * 0.56f, 2.2f, 255, 255, 255, 0x80, 1);
        r_text("X: JOGAR NOVAMENTE", W * 0.5f, H * 0.72f, 2.0f, 125, 255, 82, 0x80, 1);
        r_text("O: VOLTAR AO MENU", W * 0.5f, H * 0.80f, 2.0f, 125, 255, 82, 0x80, 1);
        if (pr & B_X) { game_init(g_mode, g_skin, "VOCE"); return ST_PLAY; }
        return (pr & B_O) ? ST_MENU : ST_DEAD;
    default: return st;
    }
}
