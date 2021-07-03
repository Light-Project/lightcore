#!/bin/bash
echo -e "\e[36mERASE...\e[0m" 
st-flash erase
echo -e "\e[36mFLASH...\e[0m" 
st-flash write ./kernel.bin 0x08000000
echo -e "\e[36mRESET...\e[0m" 
st-flash reset
echo -e "\e[36mDONE...\e[0m" 
