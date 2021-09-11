# ESP8266 memory space 

| bus  | name       | address    | size       | Description              |
| ---- | ---------- | ---------- | ---------- | ------------------------ |
| Data | Reserved   | 0x00000000 | 0x20000000 | Operation forbidden      |
| Data | Reserved   | 0x20000000 | 0x1FF00000 | Unmapped                 |
|      |            |            |            |                          |
| Data | Dport      | 0x3FF00000 | 0x10       |                          |
| Data | Fuse       | 0x3FF00050 | 0x10       | Mac,  Chipid             |
| Data | PHY        | 0x3FF00058 |            |                          |
| Data | WDEV       | 0x3FF20000 | 0x20000    |                          |
| Data | WDEV_CYPTO | 0x3FF20600 |            |                          |
| Data | DROM0      | 0x3FF40000 |            |                          |
| Data | DRAM0      | 0x3FFE8000 | 0x14000    | User data RAM            |
| Data | DRAM1      | 0x3FFFC000 | 0x4000     | ETS system data RAM      |
| Code | BROM       | 0x40000000 | 0x10000    | Internal ROM             |
| Code | Reserved   | 0x40010000 | -          | Unmapped                 |
| Code | IRAM       | 0x40100000 | 0x8000     | Bootloader Ram           |
| Code | ICACHE     | 0x40108000 | 0x8000     | Instruction CACHE.       |
| Code | FLASH      | 0x40200000 | 0x200000   | Flash is mapped here.    |
| Code | Reserved   | 0x40600000 | -          | Unmapped                 |
| AHB  | UART0      | 0x60000000 | 0x100      | Uart port 0              |
| AHB  | SPI1       | 0x60000100 | 0x100      | Spi port 1               |
| AHB  | SPI0       | 0x60000200 | 0x100      | Spi port 0               |
| AHB  | GPIO       | 0x60000300 | 0x100      | Pio                      |
| AHB  | WIFI?      | 0x60000500 | 0x100      | wifi baseband            |
| AHB  | FRC        | 0x60000600 | 0x100      | timer                    |
| AHB  | RTC        | 0x60000700 | 0x100      |                          |
| AHB  | IOMUX      | 0x60000800 | 0x100      |                          |
| AHB  | WDT        | 0x60000900 | 0x100      |                          |
| AHB  | TOUT       | 0x60000A00 | 0x100      | ADC                      |
| AHB  | SLC0       | 0x60000B00 | 0x100      | SDIO Slave               |
| AHB  | ?          | 0x60000D00 | 0x100      |                          |
| AHB  | I2S0       | 0x60000E00 | 0x100      | I2S                      |
| AHB  | UART1      | 0x60000F00 | 0x100      | Uart port 1              |
| AHB  | RTC_SYS    | 0x60001000 | 0x200      |                          |
| AHB  | RTC_USER   | 0x60001200 | 0x200      | Persists over deep sleep |
| AHB  | Reserved   | 0x60002000 | 0x70000000 | Operation forbidden      |
| AHB  |            | 0x60009600 |            |                          |
| AHB  | RF         | 0x60009A00 |            |                          |
| Data | Reserved   | 0x70000000 | 0x90000000 | Unmapped                 |

* Code Bus:  (rwx) only be accessed in 32-bit alignment, otherwise cause command exception
* Data Bus:  (rw) No execution
* AHB  Bus:  (rw) Peripheral bus
