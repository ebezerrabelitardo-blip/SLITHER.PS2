# SLITHER.PS2 - compilacao com PS2SDK + gsKit (ex.: imagem Docker ps2dev/ps2dev)
EE_BIN  = slither_ps2.elf
EE_OBJS = src/main.o src/game.o src/snake.o src/ai.o src/collision.o src/particles.o \
          src/camera.o src/render.o src/menu.o src/controller.o

GSKIT ?= $(PS2DEV)/gsKit
EE_INCS    += -Iinclude -I$(GSKIT)/include -I$(PS2SDK)/ports/include
EE_LDFLAGS += -L$(GSKIT)/lib -L$(PS2SDK)/ports/lib
EE_CFLAGS  += -O2 -G0 -Wall -std=gnu99 -fsingle-precision-constant
EE_LIBS    += -lgskit -ldmakit -lpad -lm -lc -lkernel

all: $(EE_BIN)

clean:
	rm -f $(EE_OBJS) $(EE_BIN) SLUS_111.99 *.iso

iso: $(EE_BIN)
	sh tools/make_iso.sh

test:
	gcc -O2 -Wall -Iinclude tests/sim_test.c src/game.c src/snake.c src/ai.c src/collision.c src/particles.c src/camera.c -lm -o sim_test
	./sim_test

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
