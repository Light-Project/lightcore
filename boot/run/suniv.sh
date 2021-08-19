#!bin/bash
sunxi-fel spl boot/preload.bin
sunxi-fel -p write 0x81000000 boot/boot.bin
arm-none-eabi-gdb -x boot/run/suniv
