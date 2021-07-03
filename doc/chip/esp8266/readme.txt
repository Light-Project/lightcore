C-SKY Development Kit

Intro
=====

C-SKY is a CPU Architecture from www.c-sky.com and has it own instruction set.
Just like arm and mips in linux/arch, it named as 'csky'.

gx6605s develop board is made by Hangzhou Nationalchip and C-SKY.

Hardware Spec:
  * CPU: ck610 up to 594Mhz
  * Integrate with 96KiB Sram in SOC.
  * Power Supply: DC 5V from two micro-usb.

How to build it
===============

The gx6605s_defconfig configuration is a sample configuration with
all that is required to bring the gx6605s Development Board:

  $ make esp8266_defconfig
  $ make

Result of the build
-------------------

After building, you should obtain this tree:

    +-- kernel
    +-- boot/
        +-- esp8266.dtb
        +-- kernel.bin
        +-- preload-0x00000.bin

How to run it
=============

  Setup the Console with the rate 115200/8-N-1.
  $ make run path=/dev/xxx
