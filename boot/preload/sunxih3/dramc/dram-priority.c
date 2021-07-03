#include <boot.h>
#include "dramc.h"
#include <chip/sunxih3.h>

enum {
    MBUS_PORT_CPU 		= 0,
    MBUS_PORT_GPU 		= 1,
    MBUS_PORT_UNUSED	= 2,
    MBUS_PORT_DMA 		= 3,
    MBUS_PORT_VE 		= 4,
    MBUS_PORT_CSI 		= 5,
    MBUS_PORT_NAND 		= 6,
    MBUS_PORT_SS 		= 7,
    MBUS_PORT_TS 		= 8,
    MBUS_PORT_DI 		= 9,
    MBUS_PORT_DE 		= 10,
    MBUS_PORT_DE_CFD 	= 11,
    MBUS_PORT_UNKNOWN1	= 12,
    MBUS_PORT_UNKNOWN2	= 13,
    MBUS_PORT_UNKNOWN3	= 14,
};

enum {
    MBUS_QOS_LOWEST 	= 0,
    MBUS_QOS_LOW 		= 1,
    MBUS_QOS_HIGH 		= 2,
    MBUS_QOS_HIGHEST 	= 3,
};

static inline void mbus_configure_port(u8_t port,
                int bwlimit,
                int priority,
                u8_t qos,
                u8_t waittime,
                u8_t acs,
                u16_t bwl0,
                u16_t bwl1,
                u16_t bwl2)
{
    struct h3_dram_com_reg_t * com = (struct h3_dram_com_reg_t *)H3_DRAM_COM_BASE;

    u32_t cfg0 = ( (bwlimit ? (1 << 0) : 0)
            | (priority ? (1 << 1) : 0)
            | ((qos & 0x3) << 2)
            | ((waittime & 0xf) << 4)
            | ((acs & 0xff) << 8)
            | (bwl0 << 16) );
    u32_t cfg1 = ((u32_t)bwl2 << 16) | (bwl1 & 0xffff);

    write32((virtual_addr_t)&com->mcr[port][0], cfg0);
    write32((virtual_addr_t)&com->mcr[port][1], cfg1);
}

#define MBUS_CONF(port, bwlimit, qos, acs, bwl0, bwl1, bwl2)	\
    mbus_configure_port(MBUS_PORT_ ## port, bwlimit, 0, 		\
                MBUS_QOS_ ## qos, 0, acs, bwl0, bwl1, bwl2)

static void mctl_set_master_priority(void)
{
    struct h3_dram_com_reg_t * com = (struct h3_dram_com_reg_t *)H3_DRAM_COM_BASE;

    write32((virtual_addr_t)&com->bwcr, (1 << 16) | (400 << 0));
    write32((virtual_addr_t)&com->mapr, 0x00000001);

    MBUS_CONF(   CPU,  1, HIGHEST, 0,  512,  256,  128);
    MBUS_CONF(   GPU,  1,    HIGH, 0, 1536, 1024,  256);
    MBUS_CONF(UNUSED,  1, HIGHEST, 0,  512,  256,   96);
    MBUS_CONF(   DMA,  1, HIGHEST, 0,  256,  128,   32);
    MBUS_CONF(    VE,  1,    HIGH, 0, 1792, 1600,  256);
    MBUS_CONF(   CSI,  1, HIGHEST, 0,  256,  128,   32);
    MBUS_CONF(  NAND,  1,    HIGH, 0,  256,  128,   64);
    MBUS_CONF(    SS,  1, HIGHEST, 0,  256,  128,   64);
    MBUS_CONF(    TS,  1, HIGHEST, 0,  256,  128,   64);
    MBUS_CONF(    DI,  1,    HIGH, 0, 1024,  256,   64);
    MBUS_CONF(    DE,  1, HIGHEST, 3, 8192, 6120, 1024);
    MBUS_CONF(DE_CFD,  1,    HIGH, 0, 1024,  288,   64);
} 
