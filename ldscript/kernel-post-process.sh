#!/bin/bash
TOOLSPATH=$1
HOST=$2
PROJ=$3
NAME=$4
ARCH=$5
FLOAT=$6
TYPE=$7

OUT=$PROJ

$TOOLSPATH/bin/$HOST-objcopy -j .text -j .priv_code -j .data -j .vectors -j .startup -O binary ./$OUT.elf ./$OUT.bin
$TOOLSPATH/bin/$HOST-objdump -S -j .text -j .priv_code -j .data -j .bss -j .sysmem -d $OUT.elf > $OUT.s
$TOOLSPATH/bin/$HOST-size ./$OUT.elf 
