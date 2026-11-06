/* Renderizacao 2D com gsKit. Circulos = sprite texturizado (textura 32x32 gerada em runtime). */
#include <gsKit.h>
#include <dmaKit.h>
#include <malloc.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "game.h"
#include "render.h"

static GSGLOBAL *gs;
static GSTEXTURE circ;
int g_frame = 0;
#define COL(r, g, b, a) GS_SETREG_RGBAQ((r), (g), (b), (a), 0x00)

int r_w(void) { return gs->Width; }
int r_h(void) { return gs->Height; }
int r_pal(void) { return gs->Mode == GS_MODE_PAL; }

void r_init(void) {
    gs = gsKit_init_global();
    gs->PSM = GS_PSM_CT24;
    gs->DoubleBuffering = GS_SETTING_ON;
    gs->ZBuffering = GS_SETTING_OFF;
    gs->PrimAlphaEnable = GS_SETTING_ON;
    dmaKit_init(D_CTRL_RELE_OFF, D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC, D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);
    dmaKit_chan_init(DMA_CHANNEL_GIF);
    gsKit_init_screen(gs);

    /* textura de circulo suave (branco com alfa): 0..0x80 */
    circ.Width = 32; circ.Height = 32; circ.PSM = GS_PSM_CT32; circ.Filter = GS_FILTER_LINEAR;
    circ.Mem = memalign(128, gsKit_texture_size_ee(32, 32, GS_PSM_CT32));
    unsigned int *px = (unsigned int *)circ.Mem;
    for (int y = 0; y < 32; y++) for (int x = 0; x < 32; x++) {
        float dx = x - 15.5f, dy = y - 15.5f, d = sqrtf(dx * dx + dy * dy) / 15.5f;
        float a = (1.0f - d) * 6.0f; if (a < 0) a = 0; if (a > 1) a = 1;
        px[y * 32 + x] = 0x00FFFFFFu | ((unsigned int)(a * 128.0f) << 24);
    }
    circ.Vram = gsKit_vram_alloc(gs, gsKit_texture_size(32, 32, GS_PSM_CT32), GSKIT_ALLOC_USERBUFFER);
    gsKit_texture_upload(gs, &circ);
    gsKit_mode_switch(gs, GS_ONESHOT);
}

void r_begin(void) { gsKit_clear(gs, COL(27, 32, 48, 0x80)); }
void r_end(void) { gsKit_queue_exec(gs); gsKit_sync_flip(gs); g_frame++; }

static int A(int a) { return a > 0x80 ? 0x80 : a < 0 ? 0 : a; }
void r_rect(float x1, float y1, float x2, float y2, int r, int g, int b, int a) { gsKit_prim_sprite(gs, x1, y1, x2, y2, 1, COL(r, g, b, A(a))); }
void r_circle(float x, float y, float rad, int r, int g, int b, int a) {
    gsKit_prim_sprite_texture(gs, &circ, x - rad, y - rad, 0.0f, 0.0f, x + rad, y + rad, 32.0f, 32.0f, 1, COL(r, g, b, A(a)));
}
void r_line(float x1, float y1, float x2, float y2, int r, int g, int b, int a) { gsKit_prim_line(gs, x1, y1, x2, y2, 1, COL(r, g, b, A(a))); }
void r_tri(float x1, float y1, float x2, float y2, float x3, float y3, int r, int g, int b, int a) { gsKit_prim_triangle(gs, x1, y1, x2, y2, x3, y3, 1, COL(r, g, b, A(a))); }
void r_quad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, int r, int g, int b, int a) {
    gsKit_prim_quad(gs, x1, y1, x2, y2, x3, y3, x4, y4, 1, COL(r, g, b, A(a)));
}
void r_gradient(int r1, int g1, int b1, int r2, int g2, int b2) {
    float W = (float)gs->Width, H = (float)gs->Height;
    gsKit_prim_quad_gouraud(gs, 0, 0, W, 0, 0, H, W, H, 1, COL(r1, g1, b1, 0x80), COL(r1, g1, b1, 0x80), COL(r2, g2, b2, 0x80), COL(r2, g2, b2, 0x80));
}

/* ---------- fonte 5x7 embutida (A-Z 0-9 : . # -) ---------- */
static const unsigned char F5[41][5] = {
 {0x3E,0x51,0x49,0x45,0x3E},{0x00,0x42,0x7F,0x40,0x00},{0x42,0x61,0x51,0x49,0x46},{0x21,0x41,0x45,0x4B,0x31},{0x18,0x14,0x12,0x7F,0x10},
 {0x27,0x45,0x45,0x45,0x39},{0x3C,0x4A,0x49,0x49,0x30},{0x01,0x71,0x09,0x05,0x03},{0x36,0x49,0x49,0x49,0x36},{0x06,0x49,0x49,0x29,0x1E},
 {0x7E,0x11,0x11,0x11,0x7E},{0x7F,0x49,0x49,0x49,0x36},{0x3E,0x41,0x41,0x41,0x22},{0x7F,0x41,0x41,0x22,0x1C},{0x7F,0x49,0x49,0x49,0x41},
 {0x7F,0x09,0x09,0x09,0x01},{0x3E,0x41,0x49,0x49,0x7A},{0x7F,0x08,0x08,0x08,0x7F},{0x00,0x41,0x7F,0x41,0x00},{0x20,0x40,0x41,0x3F,0x01},
 {0x7F,0x08,0x14,0x22,0x41},{0x7F,0x40,0x40,0x40,0x40},{0x7F,0x02,0x0C,0x02,0x7F},{0x7F,0x04,0x08,0x10,0x7F},{0x3E,0x41,0x41,0x41,0x3E},
 {0x7F,0x09,0x09,0x09,0x06},{0x3E,0x41,0x51,0x21,0x5E},{0x7F,0x09,0x19,0x29,0x46},{0x46,0x49,0x49,0x49,0x31},{0x01,0x01,0x7F,0x01,0x01},
 {0x3F,0x40,0x40,0x40,0x3F},{0x1F,0x20,0x40,0x20,0x1F},{0x3F,0x40,0x38,0x40,0x3F},{0x63,0x14,0x08,0x14,0x63},{0x07,0x08,0x70,0x08,0x07},
 {0x61,0x51,0x49,0x45,0x43},{0x00,0x36,0x36,0x00,0x00},{0x00,0x60,0x60,0x00,0x00},{0x14,0x7F,0x14,0x7F,0x14},{0x08,0x08,0x08,0x08,0x08},
 {0x00,0x00,0x00,0x00,0x00}};
static int gidx(char c) {
    if (c >= 'a' && c <= 'z') c -= 32;
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'Z') return 10 + c - 'A';
    if (c == ':') return 36;
    if (c == '.') return 37;
    if (c == '#') return 38;
    if (c == '-') return 39;
    return 40;
}
float r_text_w(const char *s, float sc) { int n = (int)strlen(s); return n ? (n * 6 - 1) * sc : 0.0f; }
void r_text(const char *s, float x, float y, float sc, int r, int g, int b, int a, int align) {
    if (align == 1) x -= r_text_w(s, sc) * 0.5f; else if (align == 2) x -= r_text_w(s, sc);
    for (; *s; s++, x += 6 * sc) {
        const unsigned char *gl = F5[gidx(*s)];
        for (int c = 0; c < 5; c++) {
            int row = 0;
            while (row < 7) {                                   /* une pixels verticais em um unico sprite */
                if (!((gl[c] >> row) & 1)) { row++; continue; }
                int e = row; while (e + 1 < 7 && ((gl[c] >> (e + 1)) & 1)) e++;
                r_rect(x + c * sc, y + row * sc, x + (c + 1) * sc, y + (e + 1) * sc, r, g, b, a);
                row = e + 1;
            }
        }
    }
}

/* ---------- mundo ---------- */
static void hue(float h, int *r, int *g, int *b) {
    float x = fmodf(h, 360.0f) / 60.0f; int i = (int)x; float f = x - i;
    int q = (int)(255 * (1 - f)), t = (int)(255 * f);
    switch (i % 6) { case 0: *r = 255; *g = t; *b = 80; break; case 1: *r = q; *g = 255; *b = 80; break;
        case 2: *r = 80; *g = 255; *b = t; break; case 3: *r = 80; *g = q; *b = 255; break;
        case 4: *r = t; *g = 80; *b = 255; break; default: *r = 255; *g = 80; *b = q; }
}

void r_world(void) {
    float W = (float)gs->Width, H = (float)gs->Height, sc = 0.9f * G.cam.z, cx = G.cam.x, cy = G.cam.y;
    float vw = W * 0.5f / sc + 60.0f, vh = H * 0.5f / sc + 60.0f;
    #define SX(x) (((x) - cx) * sc + W * 0.5f)
    #define SY(y) (((y) - cy) * sc + H * 0.5f)
    /* grade hexagonal */
    const float hs = 90.0f, w = hs * 1.5f, hh = hs * 1.732f;
    for (int c = (int)floorf((cx - vw) / w) - 1; c <= (int)((cx + vw) / w) + 1; c++) {
        float oy = (c & 1) ? hh * 0.5f : 0.0f;
        for (int r = (int)floorf((cy - vh - oy) / hh) - 1; r <= (int)((cy + vh - oy) / hh) + 1; r++) {
            float x = c * w, y = r * hh + oy;
            for (int k = 0; k < 6; k++) {
                float a0 = k * 1.0472f, a1 = (k + 1) * 1.0472f;
                r_line(SX(x + cosf(a0) * hs), SY(y + sinf(a0) * hs), SX(x + cosf(a1) * hs), SY(y + sinf(a1) * hs), 8, 10, 16, 0x80);
            }
        }
    }
    /* limite da arena: faixa escura externa + anel vermelho */
    for (int i = 0; i < 120; i++) {
        float a0 = i * TAU_F / 120.0f, a1 = (i + 1) * TAU_F / 120.0f;
        float px = cosf(a0) * ARENA_R, py = sinf(a0) * ARENA_R;
        if (fabsf(px - cx) > vw + 400.0f || fabsf(py - cy) > vh + 400.0f) continue;
        float c0 = cosf(a0), s0 = sinf(a0), c1 = cosf(a1), s1 = sinf(a1);
        r_quad(SX(c0 * ARENA_R), SY(s0 * ARENA_R), SX(c0 * (ARENA_R + 900.0f)), SY(s0 * (ARENA_R + 900.0f)),
               SX(c1 * ARENA_R), SY(s1 * ARENA_R), SX(c1 * (ARENA_R + 900.0f)), SY(s1 * (ARENA_R + 900.0f)), 5, 6, 10, 0x80);
        r_quad(SX(c0 * (ARENA_R - 8)), SY(s0 * (ARENA_R - 8)), SX(c0 * (ARENA_R + 8)), SY(s0 * (ARENA_R + 8)),
               SX(c1 * (ARENA_R - 8)), SY(s1 * (ARENA_R - 8)), SX(c1 * (ARENA_R + 8)), SY(s1 * (ARENA_R + 8)), 255, 59, 59, 0x80);
    }
    /* particulas */
    for (int i = 0; i < G.nf; i++) {
        Food *f = &G.f[i];
        if (fabsf(f->x - cx) > vw || fabsf(f->y - cy) > vh) continue;
        float r = food_radius(f) * (1.0f + 0.15f * sinf(g_frame * 0.07f + f->ph)) * sc;
        int cr = f->r, cg = f->g, cb = f->b;
        if (f->kind == 3) hue(g_frame * 2.0f + f->ph * 60.0f, &cr, &cg, &cb);
        r_circle(SX(f->x), SY(f->y), r * 2.3f, cr, cg, cb, 0x26);
        r_circle(SX(f->x), SY(f->y), r, cr, cg, cb, 0x80);
    }
    /* cobras */
    for (int i = 0; i < G.ns; i++) {
        Snake *s = &G.s[i]; if (s->dead) continue;
        float r = snake_radius(s->mass) * sc; int nc = SKIN_N[s->skin];
        if (s->on) for (int k = 0; k < s->n; k += 3) {
            const uint8_t *c = SKIN_RGB[s->skin][0];
            r_circle(SX(s->seg[k].x), SY(s->seg[k].y), r * 2.0f, c[0], c[1], c[2], 0x18);
        }
        for (int k = s->n - 1; k >= 0; k--) {
            float x = SX(s->seg[k].x), y = SY(s->seg[k].y);
            if (x < -r - 4 || x > W + r + 4 || y < -r - 4 || y > H + r + 4) continue;
            const uint8_t *c = SKIN_RGB[s->skin][nc > 1 ? (k / 3) % nc : 0];
            r_circle(x, y, k == 0 ? r * 1.08f : r, c[0], c[1], c[2], 0x80);
            r_circle(x - r * 0.2f, y - r * 0.25f, r * 0.58f, 255, 255, 255, 0x1C);
        }
        float hx = SX(s->seg[0].x), hy = SY(s->seg[0].y), ca = cosf(s->a), sa = sinf(s->a);
        for (int e = -1; e <= 1; e += 2) {
            float ex = hx + ca * r * 0.45f - sa * r * 0.5f * e, ey = hy + sa * r * 0.45f + ca * r * 0.5f * e;
            r_circle(ex, ey, r * 0.42f, 255, 255, 255, 0x80);
            r_circle(ex + ca * r * 0.15f, ey + sa * r * 0.15f, r * 0.22f, 0, 0, 0, 0x80);
        }
        if (s->ai) r_text(s->name, hx, hy + r + 8.0f, 1.0f, 255, 255, 255, 0x50, 1);
    }
}

/* ---------- HUD (estilo do jogo de referencia) ---------- */
void r_hud(int show_info) {
    float W = (float)gs->Width, H = (float)gs->Height; Snake *p = &G.s[0]; char b[48];
    float mx = 56, my = 70, mr = 42;
    r_text("ARENA", 14, 8, 1.2f, 255, 255, 255, 0x40, 0);
    r_circle(mx, my, mr, 255, 255, 255, 0x20);
    for (int i = 0; i < G.ns; i++) {
        if (G.s[i].dead) continue;
        float x = mx + G.s[i].seg[0].x / ARENA_R * mr, y = my + G.s[i].seg[0].y / ARENA_R * mr;
        if (i == 0) r_circle(x, y, 3.0f, 255, 255, 255, 0x80); else r_circle(x, y, 2.0f, SKIN_RGB[G.s[i].skin][0][0], SKIN_RGB[G.s[i].skin][0][1], SKIN_RGB[G.s[i].skin][0][2], 0x80);
    }
    int top = G.nrank < 10 ? G.nrank : 10;
    for (int i = 0; i < top; i++) {
        Snake *s = &G.s[G.rank[i]]; const uint8_t *c = SKIN_RGB[s->skin][0]; float y = 12.0f + i * 14.0f;
        int me = (G.rank[i] == 0);
        snprintf(b, sizeof b, "#%d %s", i + 1, s->name);
        r_text(b, W - 190, y, 1.3f, me ? 255 : c[0], me ? 255 : c[1], me ? 255 : c[2], 0x80, 0);
        snprintf(b, sizeof b, "%d", (int)s->mass);
        r_text(b, W - 12, y, 1.3f, 255, 255, 255, 0x70, 2);
    }
    int rk = 0; for (int i = 0; i < G.nrank; i++) if (G.rank[i] == 0) rk = i + 1;
    r_text("POSICAO:", 14, H - 24, 1.3f, 125, 255, 82, 0x80, 0);
    snprintf(b, sizeof b, "%d DE %d", rk, G.nrank); r_text(b, 14 + r_text_w("POSICAO: ", 1.3f), H - 24, 1.3f, 255, 255, 255, 0x80, 0);
    snprintf(b, sizeof b, "%d", (int)p->mass); r_text(b, W * 0.5f - 40, H - 52, 4.0f, 255, 255, 255, 0x80, 1);
    r_text("TAMANHO", W * 0.5f - 40, H - 20, 1.2f, 106, 168, 255, 0x80, 1);
    snprintf(b, sizeof b, "%d", p->kills); r_text(b, W * 0.5f + 50, H - 52, 4.0f, 255, 255, 255, 0x80, 1);
    r_text("ABATES", W * 0.5f + 50, H - 20, 1.2f, 106, 168, 255, 0x80, 1);
    r_rect(W * 0.5f - 60, H - 8, W * 0.5f + 60, H - 5, 255, 255, 255, 0x30);           /* barra de aceleracao */
    float f = p->mass / 120.0f; if (f > 1) f = 1;
    r_rect(W * 0.5f - 60, H - 8, W * 0.5f - 60 + 120 * f, H - 5, p->mass > 12 ? 125 : 255, p->mass > 12 ? 255 : 82, 82, 0x80);
    if (!p->dead) {                                       /* seta para a particula especial mais proxima */
        float bd = 1e9f; int bf = -1; Vec2 h = p->seg[0];
        for (int i = 0; i < G.nf; i++) if (G.f[i].kind == 3) { float dx = G.f[i].x - h.x, dy = G.f[i].y - h.y, d = dx * dx + dy * dy; if (d < bd) { bd = d; bf = i; } }
        if (bf >= 0 && bd > 40000.0f) {
            float a = atan2f(G.f[bf].y - h.y, G.f[bf].x - h.x), x = W * 0.5f + cosf(a) * 90.0f, y = H * 0.5f + sinf(a) * 90.0f;
            r_tri(x + cosf(a) * 12, y + sinf(a) * 12, x + cosf(a + 2.5f) * 10, y + sinf(a + 2.5f) * 10, x + cosf(a - 2.5f) * 10, y + sinf(a - 2.5f) * 10, 255, 158, 192, 0x80);
        }
    }
    if (show_info) { snprintf(b, sizeof b, "COBRAS %d PARTICULAS %d ZOOM %d", G.nrank, G.nf, (int)(G.cam.z * 100)); r_text(b, 14, 96, 1.2f, 255, 213, 79, 0x80, 0); }
}
