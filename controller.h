#ifndef CONTROLLER_H
#define CONTROLLER_H
#define B_UP 1
#define B_DOWN 2
#define B_LEFT 4
#define B_RIGHT 8
#define B_X 16
#define B_O 32
#define B_START 64
#define B_SELECT 128
#define B_R1 256
typedef struct { int btn, pressed; float ax, ay; } Pad;   /* ax/ay: analogico esquerdo -1..1 */
void pad_init(void);
void pad_poll(Pad *p);
#endif
