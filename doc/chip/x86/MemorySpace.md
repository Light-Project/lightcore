# X86 memory space 



| name     | va     | size    | Description                |
| -------- | ------ | ------- | -------------------------- |
| INT      | 0x000  | 0x400   | 16bits interrupt vector    |
| BIOSDATA | 0x400  | 0x100   |                            |
| MBRSTACK | 0x500  | 0x2c0   |                            |
| MBR      | 0x7c00 | 0x200   | MBR load address           |
| MODE     | 0x7e00 | 0x200   | Mode load address          |
| RAM      | 0x8000 | 0x10000 | Bootloader loading address |



