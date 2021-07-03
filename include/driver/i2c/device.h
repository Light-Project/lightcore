#ifndef _I2CDEF_H_
#define _I2CDEF_H_

typedef enum{
    i2c_7adds   = (uint8_t) 0x00,
    i2c_10adds  = (uint8_t) 0x01
    
}lc_i2cdef_addmode

typedef enum{
    i2c_noneack = (uint8_t) 0x00,
    i2c_currack = (uint8_t) 0x01
    
}lc_i2cdef_ackmode

typedef enum{
    i2c_noneack = (uint8_t) 0x00,
    i2c_currack = (uint8_t) 0x01
    
}lc_i2cdef_ackmode

typedef struct{
    uint32_t            clock,
    uint16_t            addr,
    lc_i2cdef_addmode   addmode,
    lc_i2cdef_ackmode   ackmode
    
}lc_i2cdef;


#endif
