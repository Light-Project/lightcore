#include <bits.h>
 
enum Watchdog_ESP8266_Registers
{
    WDT_ESP8266_OPT0     = 0x00, // Control register
    WDT_ESP8266_TIM0     = 0x04, // Reset time
    WDT_ESP8266_TIM1    = 0x08, // Interruption time
    WDT_ESP8266_TIMUP     = 0x14, // Feed register
};

/********************************************************************************************/
/*      Mnemonic                        value           meaning/usage                       */

/* CTL bitflag definitions */
#define WDT_ESP8266_CTL_RSTLEN_MASK     0x38
#define WDT_ESP8266_CTL_RSPMOD_MASK     0x6
#define WDT_ESP8266_CTL_EN              BIT(0)

/* OP & OPND bitflag definitions */
/* wdt freq = osc / 32000, osc = 40m -> (1 / 40m) = 0.8ms */
/* 2^n * 0.8ms, mask 0xf, n = 13 -> (2^13 = 8192) * 0.8 * 0.001 = 6.5536 */

/* RST bitflag definitions */
#define WDT_ESP8266_RST_FEED_MASK       0x73

