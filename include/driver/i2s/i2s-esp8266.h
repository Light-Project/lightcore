#include <bits.h>
 
enum I2S_ESP8266_Registers
{
    I2S_ESP8266_TXFIFO      = 0x00, // load value into the counter
    I2S_ESP8266_RXFIFO      = 0x04, // current value of the counter
    I2S_ESP8266_CONF        = 0x08,
    I2S_ESP8266_INT_RAW     = 0x0c,
    I2S_ESP8266_INT_ST      = 0x10,
    I2S_ESP8266_INT_ENA     = 0x14,
    I2S_ESP8266_INT_CLR     = 0x18,
    I2S_ESP8266_TIMING      = 0x1c,
    I2S_ESP8266_FIFO_CONF   = 0x20,
    I2S_ESP8266_RXEOF_NR    = 0x24,
    I2S_ESP8266_SIGLE_DATA  = 0x28,
    I2S_ESP8266_CHAN        = 0x2c,
};

/********************************************************************************************/
/*      Mnemonic                        value           meaning/usage                       */

/* CONF bitflag definitions */
#define I2S_ESP8266_CONF_BCK_DIV            0xfc00000   // 
#define I2S_ESP8266_CONF_CLKM_DIV           0x3f0000    // 
#define I2S_ESP8266_CONF_BITS_MOD           0xf000      // 
#define I2S_ESP8266_CONF_RECE_MSB_SHIFT     BIT(11)     // 
#define I2S_ESP8266_CONF_TRANS_MSB_SHIFT    BIT(10)     // 
#define I2S_ESP8266_CONF_RX_START           BIT(9)      // 
#define I2S_ESP8266_CONF_TX_START           BIT(8)      // 
#define I2S_ESP8266_CONF_MSB_RIGHT          BIT(7)      // 
#define I2S_ESP8266_CONF_RIGHT_FIRST        BIT(6)      // 
#define I2S_ESP8266_CONF_RECE_SLAVE_MOD     BIT(5)      // 
#define I2S_ESP8266_CONF_TRANS_SLAVE_MOD    BIT(4)      // 
#define I2S_ESP8266_CONF_RESET_RXFIFO       BIT(3)      // 
#define I2S_ESP8266_CONF_RESET_TXFIFO       BIT(2)      // 
#define I2S_ESP8266_CONF_RESET_RX           BIT(1)      // 
#define I2S_ESP8266_CONF_RESET_TX           BIT(0)      // 
#define I2S_ESP8266_CONF_RESET_MASK         0x0f        // 

/* INT* bitflag definitions */
#define I2S_ESP8266_INT_TX_REMPTY           BIT(5)      // 
#define I2S_ESP8266_INT_TX_WFULL            BIT(4)      // 
#define I2S_ESP8266_INT_RX_REMPTY           BIT(3)      // 
#define I2S_ESP8266_INT_RX_WFULL            BIT(2)      // 
#define I2S_ESP8266_INT_PUT_DATA            BIT(1)      // 
#define I2S_ESP8266_INT_TAKE_DATA           BIT(0)      // 

/* FIFO_CONF bitflag definitions */
#define I2S_ESP8266_
