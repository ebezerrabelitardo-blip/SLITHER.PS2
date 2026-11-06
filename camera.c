#include "game.h"
#include <math.h>
void camera_update(float dt) {
    Snake *p = &G.s[0]; if (p->dead) return;
    float k = dt * 10.0f; if (k > 1.0f) k = 1.0f;
    G.cam.x += (p->seg[0].x - G.cam.x) * k; G.cam.y += (p->seg[0].y - G.cam.y) * k;
    float zt = 1.1f - sqrtf(p->mass) * 0.012f; if (zt < 0.4f) zt = 0.4f;   /* zoom dinamico */
    float kz = dt * 1.5f; if (kz > 1.0f) kz = 1.0f;
    G.cam.z += (zt - G.cam.z) * kz;
}
