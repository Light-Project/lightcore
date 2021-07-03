# Sunxih3 memory space 



| name                | bus   | start addr  | size | Description                 | open |
| ------------------- | ----- | ----------- | ---- | --------------------------- | ---- |
| SRAM A1             | AHB1  | 0x00000000  | 64K  | Boot ram                    | yes  |
| SRAM A2             | AHB0  | 0x0004 4000 |      | Cpus ram                    | yes  |
| SRAM C              | AHB1  | 0x0001 0000 |      |                             |      |
| DE                  | AHB1  | 0x0100 0000 |      |                             | no   |
| De-interlaced       | AHB1  | 0x0140 0000 |      |                             | no   |
| System Control      | AHB1  | 0x01C0 0000 |      | System Control              | yes  |
| DMA                 | AHB1  | 0x01C0 2000 |      | DMA                         | yes  |
| NFDC                | AHB1  | 0x01C0 3000 |      | NAND Flash Controller       | yes  |
| TS                  | AHB1  | 0x01C0 6000 |      | Transport Stream Controller | yes  |
| Key Memory Space    | AHB1  | 0x01C0 B000 |      |                             | no   |
| LCD 0               | AHB1  | 0x01C0 C000 |      |                             | yes  |
| LCD 1               | AHB1  | 0x01C0 D000 |      |                             | yes  |
| VE                  | AHB1  | 0x01C0 E000 |      |                             | no   |
| SD/MMC 0            | AHB1  | 0x01C0 F000 |      | SD/MMC controller           | yes  |
| SD/MMC 1            | AHB1  | 0x01C1 0000 |      | SD/MMC controller           | yes  |
| SD/MMC 2            | AHB1  | 0x01C1 1000 |      | SD/MMC controller           | yes  |
| SID                 | APB1  | 0x01C14000  |      | Security ID                 | no   |
| Crypto Engine       | AHB1  | 0x01C15000  |      |                             |      |
| MSG_BOX             | AHB1  | 0x01C17000  |      | Message Box                 | yes  |
| SPINLOCK            | AHB1  | 0x01C18000  |      | Spinlock                    | yes  |
| USB-OTG_Device      | AHB1  | 0x01C19000  |      | MSUSB                       |      |
| USB-OTG_EHCI0/OHCI0 | AHB1  | 0x01C1A000  |      | ECHI OCHI                   |      |
| USB-HCI1            | AHB2  | 0x01C1B000  |      | ECHI OCHI                   | yes  |
| USB-HCI2            | AHB2  | 0x01C1C000  |      | ECHI OCHI                   | yes  |
| USB-HCI3            | AHB2  | 0x01C1D000  |      | ECHI OCHI                   | yes  |
| SMC                 | AHB1  |             |      | Secure Memory Controller    | yes  |
| CCU                 | APB1  |             |      | Clock Configuration Unit    | yes  |
| PIO                 | APB1  |             |      | Port Controller             | yes  |
| TIMER               | APB1  |             |      |                             | yes  |
| OWA                 |       |             |      | One Wire Audio              | yes  |
| PWM                 | APB1  |             |      |                             | yes  |
| KEYADC              | APB1  |             |      |                             | yes  |
| I2S/PCM 0           | APB1  |             |      |                             | yes  |
| I2S/PCM 1           | APB1  |             |      |                             | yes  |
| I2S/PCM 2           | APB1  |             |      |                             | yes  |
| AC                  | APB1  |             |      | Audio Codec                 | yes  |
| SMTA                | APB1  |             |      | Secure Memory Touch Arbiter | yes  |
| THS                 | APB1  |             |      | Thermal Sensor              | yes  |
| UART 0              | APB2  |             |      | UART 16550                  | yes  |
| UART 1              | APB2  |             |      | UART 16550                  | yes  |
| UART 2              | APB2  |             |      | UART 16550                  | yes  |
| UART 3              | APB2  |             |      | UART 16550                  | yes  |
| TWI 0               | APB2  |             |      | I2C MV64xxx                 | yes  |
| TWI 1               | APB2  |             |      | I2C MV64xxx                 | yes  |
| TWI 2               | APB2  |             |      | I2C MV64xxx                 | yes  |
| SCR                 |       |             |      | Smart Card Reader           | yes  |
| EMAC                | AHB2  |             |      | Ethernet MAC                | yes  |
| GPU                 | AHB1  |             |      | Mali-400 MP2                | no   |
| HSTMR               |       |             |      | High-speed Timer            | yes  |
| DRAMCOM             | AHB1  |             |      |                             |      |
| DRAMCTL0            | AHB1  |             |      |                             |      |
| DRAMPHY0            | AHB1  |             |      |                             |      |
| SPI0                | AHB1  |             |      | Serial Peripheral Interface | yes  |
| SPI1                | AHB1  |             |      | Serial Peripheral Interface | yes  |
| SCU                 |       |             |      |                             |      |
| CSI                 | AHB1  |             |      |                             | yes  |
| TVE                 |       |             |      |                             |      |
| HDMI                |       |             |      |                             |      |
| RTC                 | APB0  |             |      |                             | yes  |
| R_TIMER             | APB0  |             |      |                             |      |
| R_INTC              | APB0  |             |      |                             |      |
| R_WDOG              | APB0  |             |      |                             |      |
| R_PRCM              | APB0  |             |      |                             |      |
| R_TWD               | APB0  |             |      |                             |      |
| R_CPUCFG            | APB0  |             |      |                             |      |
| R_CIR-RX            | APB0  |             |      |                             |      |
| R_TWI               | APB0  |             |      |                             | yes  |
| R_UART              | APB0  |             |      |                             | yes  |
| R_PIO               | APB0  |             |      | CPUs Port Controller        | yes  |
| R_PWM               | APB0  |             |      |                             |      |
| CoreSight Debug     | APB0  |             |      |                             |      |
| TSGEN RO            | APB0  |             |      |                             |      |
| TSGEN CTRL          | APB0  |             |      |                             |      |
| DDR-III/LPDDR-II    | AXIS2 |             |      |                             | yes  |
| N-BROM              | APB1  |             |      |                             | no   |
| S-BROM              | APB1  |             |      |                             | no   |

