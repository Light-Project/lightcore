# GX6605s memory space 



| name          | address    | size       | interrupt |
| ------------- | ---------- | ---------- | --------- |
| brom          | 0x00000000 | 0x100000   | -         |
| sram          | 0x00100000 | 0x80000    | -         |
| gx6605s-timer | 0x0020a000 | 0x400      | 10        |
| gx6605s-intc  | 0x00500000 | 0x400      | none      |
| mbl-gpio      | 0x00305000 | 0x10       | none      |
| ns16550a      | 0x00403000 | 0x400      | 15        |
| generic-ehci  | 0x00900000 | 0x400      | 59        |
| generic-ohci0 | 0x00a00000 | 0x400      | 58        |
| generic-ohci1 | 0x00b00000 | 0x400      | 57        |
| Internal DDR2 | 0x10000000 | 0x04000000 | -         |

