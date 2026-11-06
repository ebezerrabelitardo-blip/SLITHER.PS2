/* SLITHER.PS2 - ponto de entrada. Ciclo: ler controle -> atualizar -> desenhar -> vsync. */
#include <math.h>
#include "game.h"
#include "render.h"
#include "controller.h"
#include "menu.h"

int main(void) {
    pad_init();
    r_init();
    rng_seed(0xC0FFEE);
    const float dt = r_pal() ? 0.02f : (1.0f / 60.0f);
    State st = ST_MENU; Pad pad = {0, 0, 0, 0}; int info = 0, deadT = 0;
    for (;;) {
        pad_poll(&pad);
        if (pad.pressed & B_SELECT) info ^= 1;
        r_begin();
        if (st == ST_PLAY) {
            float ta = 0.0f; int has = 0;
            if (sqrtf(pad.ax * pad.ax + pad.ay * pad.ay) > 0.3f) { ta = atan2f(pad.ay, pad.ax); has = 1; }   /* analogico esquerdo */
            else if (pad.btn & (B_LEFT | B_RIGHT)) { ta = G.s[0].ta + ((pad.btn & B_RIGHT) ? 0.08f : 0.0f) - ((pad.btn & B_LEFT) ? 0.08f : 0.0f); has = 1; }
            game_update(dt, ta, has, (pad.btn & B_X) != 0);                                                 /* X = acelerar */
            r_world(); r_hud(info);
            if (G.s[0].dead) { if (++deadT > 60) { records_add((int)G.s[0].mass); st = ST_DEAD; } } else deadT = 0;
            if (pad.pressed & B_START) st = ST_PAUSE;
        } else if (st == ST_PAUSE || st == ST_DEAD) {
            r_world(); r_hud(info);
            st = menu_run(st, &pad);
            if (st == ST_PLAY) deadT = 0;
        } else {
            st = menu_run(st, &pad);
        }
        r_end();
    }
    return 0;
}
