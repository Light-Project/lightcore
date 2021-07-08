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
        -hda $image                         \
        -device piix4-usb-uhci,id=usb1      \
        -device ich9-usb-ehci1,id=usb2      \
        -device i2c-ddc                     \
        -device i8042                       \
        -device ES1370                      \
        -soundhw pcspk                      \
        -serial stdio
fi
