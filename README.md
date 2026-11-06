# SLITHER.PS2

Recriacao independente, para PlayStation 2, da experiencia de jogo de cobras multijogador
(crescer comendo particulas, acelerar gastando massa, eliminar adversarios). Sem codigo,
nome, imagens ou recursos do jogo original. C99 + PS2SDK + gsKit.

## Estrutura
```
include/      game.h (logica), render.h, controller.h, menu.h
src/          game, snake, ai, collision, particles, camera  -> logica pura (roda no PC)
              render, menu, controller, main                 -> PS2 (gsKit / libpad)
iso/SYSTEM.CNF   BOOT2 = cdrom0:\SLUS_111.99;1
tools/make_iso.sh  gera slither_ps2.iso com o ELF renomeado para SLUS_111.99
tests/sim_test.c   teste headless (colisao, crescimento, aceleracao, IA, memoria)
.github/workflows/build.yml   compila ELF + ISO e publica como artefato
```

## Compilar (GitHub Actions)
Envie a pasta para um repositorio. O workflow roda `make test`, `make` e `make iso` na imagem
`ps2dev/ps2dev:latest`. Baixe `slither_ps2.elf` e `slither_ps2.iso` em *Actions > Artifacts*.

## Compilar localmente
```
docker run --rm -v "$PWD":/src -w /src ps2dev/ps2dev:latest sh -c "apk add --no-cache xorriso && make && make iso"
```
Ou instale o PS2DEV (https://github.com/ps2dev/ps2dev) e rode `make` com `PS2DEV`, `PS2SDK` e `GSKIT` no ambiente.

## Rodar
- **PCSX2:** *System > Run ELF* com `slither_ps2.elf`, ou carregue `slither_ps2.iso` (precisa de BIOS PS2 propria).
- **PS2 real:** ELF via uLaunchELF/OPL/FreeMcBoot (USB, SMB ou HDD). A ISO precisa de OPL; discos gravados
  exigem modchip/softmod. O ID `SLUS_111.99` e usado so como nome do arquivo de boot; confira se nao colide
  com um jogo comercial (o PCSX2 aplica correcoes por serial).

## Controles (DualShock 2)
Analogico esquerdo: direcao | X: acelerar | START: pausar | SELECT: informacoes | D-pad: menus (e curva sem analogico) | O: voltar.

## Estado dos testes
| Item | Situacao |
|---|---|
| Logica (colisao, crescimento, aceleracao, IA, limites, memoria) | Testada no PC: `make test` |
| Sintaxe dos arquivos PS2 | Checada no PC com headers de teste (stubs) |
| Compilacao real com PS2SDK/gsKit | **Pendente**: rodar o GitHub Actions |
| PCSX2 (video, controle, desempenho) | **Pendente** |
| PS2 real | **Pendente** |

## Ainda nao implementado
Audio (AudSrv), memory card (recordes so valem na sessao), multiplayer local e online, nickname editavel,
construtor de cobras. Pontos para validar no emulador: ordem `gsKit_queue_exec`/`gsKit_sync_flip`,
upload da textura do circulo, desempenho com 16 cobras (reduza `TARGET_FOOD`/bots em `game.h` se cair de 60 FPS).
