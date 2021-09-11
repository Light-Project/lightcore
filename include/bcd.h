#ifndef _BCD_H_
#define _BCD_H_

#define bcd2bin(x) (((x) & 0x0f) + ((x) >> 4) * 10)
#define bin2bcd(x) ((((x) / 10) << 4) + (x) % 10)

#endif /* _BCD_H_ */
        
