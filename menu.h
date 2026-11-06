#ifndef MENU_H
#define MENU_H
#include "controller.h"
typedef enum { ST_MENU, ST_SKINS, ST_SETTINGS, ST_ONLINE, ST_PLAY, ST_PAUSE, ST_DEAD } State;
extern int g_mode, g_skin, g_recs[5];
void  records_add(int score);
State menu_run(State st, const Pad *p);   /* desenha a tela atual e trata a entrada; devolve o proximo estado */
#endif
