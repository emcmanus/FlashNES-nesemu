#!/bin/sh

./wla-6502.exe -o nsf_bios.s nsf_bios.o
./wlalink.exe -b linkfile nsf_bios.bin
cp nsf_bios.bin ../codeblocks/
