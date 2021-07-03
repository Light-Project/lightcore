#!/bin/bash
qemu-system-riscv64 \
        -name "Lightcore for riscv64-virt"          \
        -cpu rv64                                   \
        -smp 0                                      \
        -m 32M                                      \
        -bios none                                  \
        -kernel ./bootloader/qemu-riscv64/boot.img  \
        -serial mon:stdio                           \
        -nographic                                  \
