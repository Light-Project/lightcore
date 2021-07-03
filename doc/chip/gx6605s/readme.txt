C-SKY Development Kit

Intro
=====

C-SKY is a CPU Architecture from www.c-sky.com and has it own instruction set.
Just like arm and mips in linux/arch, it named as 'csky'.

gx6605s develop board is made by Hangzhou Nationalchip and C-SKY.

Hardware Spec:
  * CPU: ck610 up to 594Mhz
  * Integrate with 64MB ddr2 in SOC.
  * Integrate with hardware Jtag.
  * Integrate with usb-to-serial chip.
  * USB ehci controller in SOC.
  * Power Supply: DC 5V from two micro-usb.

How to build it
===============

The gx6605s_defconfig configuration is a sample configuration with
all that is required to bring the gx6605s Development Board:

  $ make gx6605s_defconfig

Build everything
----------------

Note: you will need to have access to the network, since Buildroot will
download the packages' sources.

  $ make

Result of the build
-------------------

After building, you should obtain this tree:

    +-- kernel
    +-- boot/
        +-- boot.bin
        +-- gx6605s.dtb

How to run it
=============

Prepare Jtag-Server
-------------------

  Download the Jtag-Server here:

  https://github.com/c-sky/tools/raw/master/DebugServerConsole-linux-x86_64-V4.2.00-20161213.tar.gz

  Go to the unpacked directory:

  $./DebugServerConsole -ddc -rstwait 1000 -prereset -port 1025

  Perhaps you need to use "sudo", which need libusb to detect c510:b210

  $ sudo ./DebugServerConsole -ddc -rstwait 1000 -prereset -port 1025

Run
---

  Setup the Console with the rate 115200/8-N-1.

  $ make run

