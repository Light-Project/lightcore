#!/bin/bash

if [ ! $path ];then
    echo "Debugging is not supported, please input serial port number"
    
else
    echo -e "\e[36mFlash disk...\e[0m" 
    tools/esp/esptool.py --port $path -b 921600 write_flash 0 boot/disk.img
    echo -e "\e[36mDone...\e[0m" 
    picocom -b 115200 $path
fi
