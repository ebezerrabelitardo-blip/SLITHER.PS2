/* DualShock 2 via libpad (modulos SIO2MAN/PADMAN da BIOS) */
#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <libpad.h>
#include <string.h>
#include "controller.h"

static char padBuf[256] __attribute__((aligned(64)));
static int ok = 0;

void pad_init(void) {
    SifInitRpc(0);
    SifLoadModule("rom0:SIO2MAN", 0, NULL);
    SifLoadModule("rom0:PADMAN", 0, NULL);
    padInit(0);
    if (!padPortOpen(0, 0, padBuf)) return;
    int s = 0;
    for (int i = 0; i < 300000; i++) {
        s = padGetState(0, 0);
        if (s == PAD_STATE_STABLE || s == PAD_STATE_FINDCTP1) break;
    }
    if (s == PAD_STATE_STABLE || s == PAD_STATE_FINDCTP1) {
        padSetMainMode(0, 0, PAD_MMODE_DUALSHOCK, PAD_MMODE_LOCK);   /* ativa o analogico */
        ok = 1;
    }
}

void pad_poll(Pad *p) {
    static int prev = 0;
    struct padButtonStatus b;
    p->btn = 0; p->ax = p->ay = 0.0f;
    int st = padGetState(0, 0);
    if (st == PAD_STATE_STABLE || st == PAD_STATE_FINDCTP1) {
        if (!ok) { padSetMainMode(0, 0, PAD_MMODE_DUALSHOCK, PAD_MMODE_LOCK); ok = 1; }
        if (padRead(0, 0, &b)) {
            unsigned raw = 0xffff ^ b.btns;
            if (raw & PAD_UP) p->btn |= B_UP;
            if (raw & PAD_DOWN) p->btn |= B_DOWN;
            if (raw & PAD_LEFT) p->btn |= B_LEFT;
            if (raw & PAD_RIGHT) p->btn |= B_RIGHT;
            if (raw & PAD_CROSS) p->btn |= B_X;
            if (raw & PAD_CIRCLE) p->btn |= B_O;
            if (raw & PAD_START) p->btn |= B_START;
            if (raw & PAD_SELECT) p->btn |= B_SELECT;
            if (raw & PAD_R1) p->btn |= B_R1;
            p->ax = ((int)b.ljoy_h - 128) / 128.0f;
            p->ay = ((int)b.ljoy_v - 128) / 128.0f;
        }
    }
    p->pressed = p->btn & ~prev; prev = p->btn;
}
