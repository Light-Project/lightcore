# ESP32 memory space 



| Bus  | name             | acc  | start addr | size     | Description              |
| ---- | ---------------- | ---- | ---------- | -------- | ------------------------ |
| Data | Reserved         | All  | 0x00000000 | -        | Unmapped                 |
| Data | External Flash   | All  | 0x3F400000 | 0x400000 |                          |
| Data | External SRAM    | All  | 0x3F800000 | 0x400000 |                          |
| Data | DPort            | All  | 0x3FF00000 | 0x1000   |                          |
| Data | AES Accelerator  | All  | 0x3FF01000 | 0x1000   |                          |
| Data | RSA Accelerator  | All  | 0x3FF02000 | 0x1000   |                          |
| Data | SHA Accelerator  | All  | 0x3FF03000 | 0x1000   |                          |
| Data | Secure Boot      | All  | 0x3FF04000 | 0x1000   |                          |
| Data | MMU              | All  | 0x3FF10000 |          |                          |
| Data | PID              | All  | 0x3FF1F000 | 0x1000   |                          |
| Data | Peripheral       | All  | 0x3FF40000 | 0x40000  | Remap 0x60000000 (FIFO)  |
| Data | RTC_FAST         | Pro  | 0x3FF80000 | 0x2000   |                          |
| Data | Reserved         | All  | 0x3FF82000 | -        |                          |
| Data | BROM1            | All  | 0x3FF90000 | 0x10000  |                          |
| Data | Reserved         | All  | 0x3FFA0000 | -        |                          |
| Data | SRAM2            | All  | 0x3FFA0000 | 0x32000  | Data Ram2 (200KiB)       |
| Data | SRAM1            | All  | 0x3FFE0000 | 0x20000  | Data Ram1 (128KiB)       |
| Code | BROM0            | All  | 0x40000000 | 0x8000   | Remap 0x40008000         |
| Code | BROM0            | All  | 0x40008000 | 0x20000  |                          |
| Code | Reserved         | All  | 0x40060000 | 0x10000  |                          |
| Code | SRAM0            | All  | 0x40070000 | 0x10000  | ICACHE (64KiB)           |
| Code | SRAM0            | All  | 0x40080000 | 0x20000  | Code Ram0 (128KiB)       |
| Code | SRAM1            | All  | 0x400A0000 | 0x10000  | Code Ram1 (64KiB)        |
| Code | SRAM1            | All  | 0x400B0000 | 0x8000   | Remap                    |
| Code | SRAM1            | All  | 0x400B8000 | 0x8000   | Code Ram1 (32KiB)        |
| Code | RTC_FAST         | Pro  | 0x400C0000 | 0x2000   | Coprocessor memory       |
| Code | External Flash   | All  | 0x400C2000 | 0xB3E000 | Flash is mapped here.    |
| D/C  | RTC_SLOW         | All  | 0x50000000 | 0x2000   | Persists over deep sleep |
| D/C  | Reserved         | All  | 0x50002000 | -        | Unmapped                 |
| AHB  | UART0            | All  | 0x60000000 | 0x1000   |                          |
| AHB  | SPI1             | All  | 0x60002000 | 0x1000   |                          |
| AHB  | SPI0             | All  | 0x60003000 | 0x1000   |                          |
| AHB  | GPIO             | All  | 0x60004000 | 0x1000   |                          |
| AHB  | SIGMADELTA       | All  | 0x60004F00 |          |                          |
| AHB  | RTCCNTL          | All  | 0x60008000 | 0x400    |                          |
| AHB  | RTCIO            | All  | 0x60008400 | 0x400    |                          |
| AHB  | SENS             | All  | 0x60008800 | 0x400    |                          |
| AHB  | HINF             | All  | 0x6000B000 | 0x400    |                          |
| AHB  | UHCI1            | All  | 0x6000C000 | 0x100    |                          |
| AHB  | IO MUX           | All  | 0x60009000 | 0x1000   |                          |
| AHB  | SDIO Slave0      | All  | 0x6000B000 | 0x1000   |                          |
| AHB  | UDMA1            | All  | 0x6000C000 | 0x1000   |                          |
| AHB  | I2S0             | All  | 0x6000F000 | 0x1000   |                          |
| AHB  | UART1            | All  | 0x60010000 | 0x1000   |                          |
| AHB  | I2C0             | All  | 0x60013000 | 0x1000   |                          |
| AHB  | UDMA0            | All  | 0x60014000 | 0x1000   |                          |
| AHB  | SDIO Slave1      | All  | 0x60015000 | 0x1000   |                          |
| AHB  | RMT              | All  | 0x60016000 | 0x1000   |                          |
| AHB  | PCNT             | All  | 0x60017000 | 0x1000   |                          |
| AHB  | SDIO Slave3      | All  | 0x60018000 | 0x1000   |                          |
| AHB  | LED PWM          | All  | 0x60019000 | 0x1000   |                          |
| AHB  | Efuse Controller | All  | 0x6001A000 | 0x1000   |                          |
| AHB  | Flash Encryption | All  | 0x6001B000 | 0x1000   |                          |
| AHB  | PWM0             | All  | 0x6001E000 | 0x1000   |                          |
| AHB  | TIMG0            | All  | 0x6001F000 | 0x1000   |                          |
| AHB  | TIMG1            | All  | 0x60020000 | 0x1000   |                          |
| AHB  | SPI2             | All  | 0x60024000 | 0x1000   |                          |
| AHB  | SPI3             | All  | 0x60025000 | 0x1000   |                          |
| AHB  | SYSCON           | All  | 0x60026000 | 0x1000   |                          |
| AHB  | I2C1             | All  | 0x60027000 | 0x1000   |                          |
| AHB  | SDMMC            | All  | 0x60028000 | 0x1000   |                          |
| AHB  | EMAC             | All  | 0x60029000 | 0x1000   |                          |
| AHB  | PWM1             | All  | 0x6002C000 | 0x1000   |                          |
| AHB  | I2S1             | All  | 0x6002D000 | 0x1000   |                          |
| AHB  | UART2            | All  | 0x6002E000 | 0x1000   |                          |
| AHB  | PWM2             | All  | 0x6002F000 | 0x1000   |                          |
| AHB  | PWM3             | All  | 0x60030000 | 0x1000   |                          |
| AHB  | RNG              | All  | 0x60035000 | 0x1000   |                          |

* Code Bus:  (rwx) only be accessed in 32-bit alignment, otherwise cause command exception
* Data Bus:  (rw) No execution
* AHB  Bus:  (rw) Peripheral bus, no cache

