#!/bin/bash

image='boot/disk.img'
bochsconf='boot/run/bochs-x86'

if [ $path ]; then
    while true; do
        [[ -a $path ]] && break
        echo "wait for device insert"
        sleep 1
    done

    dd if=$image of=$path bs=444 count=1 conv=notrunc
    dd if=$image of=$path bs=512 skip=1 seek=1 conv=notrunc

elif [ $bochs ];then
    bochs -q -f $bochsconf

else
    qemu-system-x86_64                      \
        -name "Lightcore for x86"           \
        -cpu max -M q35                     \
        -smp 2,sockets=1,cores=2,threads=1  \
        -m 2G -fda $image                   \
        -device rtl8139                     \
        -device ES1370                      \
        -soundhw pcspk                      \
        -serial stdio
fi
