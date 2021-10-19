#!bin/sh

while true; do
    sunxi-fel spl boot/preload.bin 2>/dev/null
    [[ $? == 0 ]] && break
    sleep 1
done

while true; do
    sunxi-fel -p write 0x81000000 boot/boot.bin 2>/dev/null
    [[ $? == 0 ]] && break
    sleep 1
done

read -p "Connect the debugger, press any key to continue"
arm-none-eabi-gdb -x boot/run/suniv
