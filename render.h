#ifndef RENDER_H
#define RENDER_H
void  r_init(void);
void  r_begin(void);
void  r_end(void);
int   r_w(void);
int   r_h(void);
int   r_pal(void);
void  r_rect(float x1, float y1, float x2, float y2, int r, int g, int b, int a);
void  r_circle(float x, float y, float rad, int r, int g, int b, int a);
void  r_line(float x1, float y1, float x2, float y2, int r, int g, int b, int a);
void  r_tri(float x1, float y1, float x2, float y2, float x3, float y3, int r, int g, int b, int a);
void  r_quad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, int r, int g, int b, int a);
void  r_gradient(int r1, int g1, int b1, int r2, int g2, int b2);
void  r_text(const char *s, float x, float y, float scale, int r, int g, int b, int a, int align); /* 0=esq 1=centro 2=dir */
float r_text_w(const char *s, float scale);
void  r_world(void);
void  r_hud(int show_info);
extern int g_frame;
#endif
