# ESP32 Interface Description



| Gpio | Name/Fun0   | Fun1      | Fun2     | Fun3   | Fun4       | Fun5     | Fun6         | Reset | Note |
| ---- | ----------- | --------- | -------- | ------ | ---------- | -------- | ------------ | ----- | ---- |
| 0    | GPIO0       | GPIO0     | CLK_OUT1 | GPIO0  | -          | -        | EMAC_TX_CLK  | 3     | r    |
| 1    | U0TXD       | U0TXD     | CLK_OUT3 | GPIO1  | -          | -        | EMAC_RXD2    | 3     | -    |
| 2    | GPIO2       | GPIO2     | HSPIWP   | GPIO2  | HS2_DATA0  | SD_DATA0 | -            | 2     | r    |
| 3    | U0RXD       | U0RXD     | CLK_OUT2 | GPIO3  | -          | -        | -            | 3     | -    |
| 4    | GPIO4       | GPIO4     | HSPIHD   | GPIO4  | HS2_DATA1  | SD_DATA1 | EMAC_TX_ER   | 2     | r    |
| 5    | GPIO5       | GPIO5     | VSPICS0  | GPIO5  | HS1_DATA6  | -        | EMAC_RX_CLK  | 3     | -    |
| 6    | SD_CLK      | SD_CLK    | SPICLK   | GPIO6  | HS1_CLK    | U1CTS    | -            | 3     | -    |
| 7    | SD_DATA_0   | SD_DATA_0 | SPIQ     | GPIO7  | HS1_DATA0  | U2RTS    | -            | 3     | -    |
| 8    | SD_DATA_1   | SD_DATA_1 | SPID     | GPIO8  | HS1_DATA1  | U2CTS    | -            | 3     | -    |
| 9    | SD_DATA_2   | SD_DATA_2 | SPIHD    | GPIO9  | HS1_DATA2  | U1RXD    | -            | 3     | -    |
| 10   | SD_DATA_3   | SD_DATA_3 | SPIWP    | GPIO10 | HS1_DATA3  | U1TXD    | -            | 3     | -    |
| 11   | SD_CMD      | SD_CMD    | SPICS0   | GPIO11 | HS1_CMD    | U1RTS    | -            | 3     | -    |
| 12   | MTDI        | MTDI      | HSPIQ    | GPIO12 | HS2_DATA2  | SD_DATA2 | EMAC_TXD3    | 2     | r    |
| 13   | MTCK        | MTCK      | HSPID    | GPIO13 | HS2_DATA3  | SD_DATA3 | EMAC_RX_ER   | 2     | r    |
| 14   | MTMS        | MTMS      | HSPICLK  | GPIO14 | HS2_CLK    | SD_CLK   | EMAC_TXD2    | 3     | r    |
| 15   | MTDO        | MTDO      | HSPICS0  | GPIO15 | HS2_CMD    | SD_CMD   | EMAC_RXD3    | 3     | r    |
| 16   | GPIO16      | GPIO16    | -        | GPIO16 | HS1_DATA4  | U2RXD    | EMAC_CLK_OUT | 1     | -    |
| 17   | GPIO17      | GPIO17    | -        | GPIO17 | HS1_DATA5  | U2TXD    | EMAC_CLK_180 | 1     | -    |
| 18   | GPIO18      | GPIO18    | VSPICLK  | GPIO18 | HS1_DATA7  | -        | -            | 1     | -    |
| 19   | GPIO19      | GPIO19    | VSPIQ    | GPIO19 | U0CTS      | -        | EMAC_TXD0    | 1     | -    |
| 21   | GPIO21      | GPIO21    | VSPIHD   | GPIO21 | -          | -        | EMAC_TX_EN   | 1     | -    |
| 22   | GPIO22      | GPIO22    | VSPIWP   | GPIO22 | U0RTS      | -        | EMAC_TXD1    | 1     | -    |
| 23   | GPIO23      | GPIO23    | VSPID    | GPIO23 | HS1_STROBE | -        | -            | 1     | -    |
| 25   | GPIO25      | GPIO25    | -        | GPIO25 | -          | -        | EMAC_RXD0    | 0     | r    |
| 26   | GPIO26      | GPIO26    | -        | GPIO26 | -          | -        | EMAC_RXD1    | 0     | r    |
| 27   | GPIO27      | GPIO27    | -        | GPIO27 | -          | -        | EMAC_RX_DV   | 0     | r    |
| 32   | 32K_XP      | GPIO32    | -        | GPIO32 | -          | -        | -            | 0     | r    |
| 33   | 32K_XN      | GPIO33    | -        | GPIO33 | -          | -        | -            | 0     | r    |
| 34   | VDET_1      | GPIO34    | -        | GPIO34 | -          | -        | -            | 0     | r,i  |
| 35   | VDET_2      | GPIO35    | -        | GPIO35 | -          | -        | -            | 0     | r,i  |
| 36   | SENSOR_VP   | GPIO36    | -        | GPIO36 | -          | -        | -            | 0     | r,i  |
| 37   | SENSOR_CAPP | GPIO37    | -        | GPIO37 | -          | -        | -            | 0     | r,i  |
| 38   | SENSOR_CAPN | GPIO38    | -        | GPIO38 | -          | -        | -            | 0     | r,i  |
| 39   | SENSOR_VN   | GPIO39    | -        | GPIO39 | -          | -        | -            | 0     | r,i  |

