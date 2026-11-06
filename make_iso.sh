#!/bin/sh
# Gera slither_ps2.iso com SYSTEM.CNF + SLUS_111.99 (o ELF renomeado) na raiz.
set -e
rm -rf build_iso && mkdir -p build_iso
cp slither_ps2.elf build_iso/SLUS_111.99
cp iso/SYSTEM.CNF build_iso/SYSTEM.CNF
if command -v mkisofs >/dev/null 2>&1; then MK="mkisofs"; 
elif command -v genisoimage >/dev/null 2>&1; then MK="genisoimage";
else MK="xorriso -as mkisofs"; fi
$MK -o slither_ps2.iso -V SLITHER_PS2 -sysid PLAYSTATION -iso-level 1 -no-rr -no-pad build_iso
echo "ISO gerada: slither_ps2.iso"
