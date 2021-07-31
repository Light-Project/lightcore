#!/bin/bash

image='boot/disk.img'
bochsconf='boot/run/bochs-x86'

if [ $path ];then
    dd if=$image of=$path bs=444 count=1 conv=notrunc
    dd if=$image of=$path bs=512 skip=1 seek=1 conv=notrunc 
    
elif [ $bochs ];then
    bochs -q -f $bochsconf
    
else 
    qemu-system-x86_64                      \
        -name "Lightcore for x86"           \
        -cpu core2duo-v1                    \
        -smp 2,sockets=1,cores=2,threads=1  \
        -m 64M                              \
        -fda $image                         \
        -device qemu-xhci                   \
        -device usb-ehci                    \
        -device piix3-usb-uhci              \
        -device usb-kbd                     \
        -device rtl8139                     \
        -device e1000                       \
        -device ES1370                      \
        -soundhw pcspk                      \
        -serial stdio
fi
