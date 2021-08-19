#!bin/bash

while true; do
    sunxi-fel spl boot/preload.bin
    [[ $? == 0 ]] && break
    sleep 1
done

while true; do
    sunxi-fel -p write 0x81000000 boot/boot.bin
    [[ $? == 0 ]] && break
    sleep 1
done

arm-none-eabi-gdb -x boot/run/suniv
