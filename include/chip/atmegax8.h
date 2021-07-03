#ifndef _ATMEGAX8_H_
#define _ATMEGAX8_H_

#include <chip/atmegax8/interrupt.h>

/*
 * PINB – The Port B Input Pins Address
 */
#define PINB  (*(volatile unsigned char *)0x23)
#define PINB0 0 // Input Pins Address
#define PINB1 1 // Input Pins Address
#define PINB2 2 // Input Pins Address
#define PINB3 3 // Input Pins Address
#define PINB4 4 // Input Pins Address
#define PINB5 5 // Input Pins Address
#define PINB6 6 // Input Pins Address
#define PINB7 7 // Input Pins Address

/*
 * DDRB – The Port B Data Direction Register
 */
#define DDRB  (*(volatile unsigned char *)0x24)
#define DDB0 0 // Data Direction Register
#define DDB1 1 // Data Direction Register
#define DDB2 2 // Data Direction Register
#define DDB3 3 // Data Direction Register
#define DDB4 4 // Data Direction Register
#define DDB5 5 // Data Direction Register
#define DDB6 6 // Data Direction Register
#define DDB7 7 // Data Direction Register

/*
 * PORTB – The Port B Data Register
 */
#define PORTB  (*(volatile unsigned char *)0x25)
#define PORTB0 0 // Data Register
#define PORTB1 1 // Data Register
#define PORTB2 2 // Data Register
#define PORTB3 3 // Data Register
#define PORTB4 4 // Data Register
#define PORTB5 5 // Data Register
#define PORTB6 6 // Data Register
#define PORTB7 7 // Data Register

/*
 * PINC – The Port C Input Pins Address
 */
#define PINC   (*(volatile unsigned char *)0x26)
#define PINC0 0 // Input Pins Address
#define PINC1 1 // Input Pins Address
#define PINC2 2 // Input Pins Address
#define PINC3 3 // Input Pins Address
#define PINC4 4 // Input Pins Address
#define PINC5 5 // Input Pins Address
#define PINC6 6 // Input Pins Address

/*
 * DDRC – The Port C Data Direction Register
 */
#define DDRC   (*(volatile unsigned char *)0x27)
#define DDC0 0 // Data Direction Register
#define DDC1 1 // Data Direction Register
#define DDC2 2 // Data Direction Register
#define DDC3 3 // Data Direction Register
#define DDC4 4 // Data Direction Register
#define DDC5 5 // Data Direction Register
#define DDC6 6 // Data Direction Register

/*
 * PORTC – The Port C Data Register 
 */
#define PORTC  (*(volatile unsigned char *)0x28)
#define PORTC0 0 // Data Register
#define PORTC1 1 // Data Register
#define PORTC2 2 // Data Register
#define PORTC3 3 // Data Register
#define PORTC4 4 // Data Register
#define PORTC5 5 // Data Register
#define PORTC6 6 // Data Register

/*
 * PIND – The Port D Input Pins Address
 */
#define PIND   (*(volatile unsigned char *)0x29)
#define PIND0 0 // Input Pins Address
#define PIND1 1 // Input Pins Address
#define PIND2 2 // Input Pins Address
#define PIND3 3 // Input Pins Address
#define PIND4 4 // Input Pins Address
#define PIND5 5 // Input Pins Address
#define PIND6 6 // Input Pins Address
#define PIND7 7 // Input Pins Address

/*
 * DDRD – The Port D Data Direction Register
 */
#define DDRD  (*(volatile unsigned char *)0x2A)
#define DDD0 0 // Data Direction Register
#define DDD1 1 // Data Direction Register
#define DDD2 2 // Data Direction Register
#define DDD3 3 // Data Direction Register
#define DDD4 4 // Data Direction Register
#define DDD5 5 // Data Direction Register
#define DDD6 6 // Data Direction Register
#define DDD7 7 // Data Direction Register

/*
 * PORTD – The Port D Data Register
 */
#define PORTD (*(volatile unsigned char *)0x2B)
#define PORTD0 0 // Data Register
#define PORTD1 1 // Data Register
#define PORTD2 2 // Data Register
#define PORTD3 3 // Data Register
#define PORTD4 4 // Data Register
#define PORTD5 5 // Data Register
#define PORTD6 6 // Data Register
#define PORTD7 7 // Data Register


/*
 * TIFR0 – Timer/Counter 0 Interrupt Flag Register
 */
#define TIFR0 (*(volatile unsigned char *)0x35)
#define TOV0    0 // Timer/Counter 0 Overflow Flag
#define OCF0A   1 // Timer/Counter 0 Output Compare A Match Flag
#define OCF0B   2 // Timer/Counter 0 Output Compare B Match Flag

/*
 * TIFR1 – Timer/Counter 1 Interrupt Flag Register
 */
#define TIFR1 (*(volatile unsigned char *)0x36)
#define TOV1  0 // Timer/Counter1, Overflow Flag
#define OCF1A 1 // Timer/Counter1, Output Compare A Match Flag
#define OCF1B 2 // Timer/Counter1, Output Compare B Match Flag
#define ICF1  5 // Timer/Counter1, Input Capture Flag

/*
 * TIFR2 – Timer/Counter 2 Interrupt Flag Register
 */
#define TIFR2 (*(volatile unsigned char *)0x37)
#define TOV2    0 // Timer/Counter 2 Overflow Flag
#define OCF2A   1 // Timer/Counter 2 Output Compare A Match Flag
#define OCF2B   2 // Timer/Counter 2 Output Compare B Match Flag

/*
 * PCIFR – Pin Change Interrupt Flag Register
 */
#define PCIFR (*(volatile unsigned char *)0x3B)
#define PCIF0 0 // Pin Change Interrupt Flag 0
#define PCIF1 1 // Pin Change Interrupt Flag 1
#define PCIF2 2 // Pin Change Interrupt Flag 2

/*
 * EIFR – External Interrupt Flag Register
 */
#define EIFR (*(volatile unsigned char *)0x3C)
#define INTF0 0 // External Interrupt Flag 0
#define INTF1 1 // External Interrupt Flag 1

/*
 * EIMSK – External Interrupt Mask Register
 */
#define EIMSK (*(volatile unsigned char *)0x3D)
#define INT0 0 //External Interrupt Request 0 Enable
#define INT1 1 //External Interrupt Request 1 Enable

/*
 * GPIOR0 – General Purpose I/O Register 0
 */
#define GPIOR0 (*(volatile unsigned char *)0x3E)
#define GPIOR00 0 // I/O Register
#define GPIOR01 1 // I/O Register
#define GPIOR02 2 // I/O Register
#define GPIOR03 3 // I/O Register
#define GPIOR04 4 // I/O Register
#define GPIOR05 5 // I/O Register
#define GPIOR06 6 // I/O Register
#define GPIOR07 7 // I/O Register

/*
 * EECR – The EEPROM Control Register
 */
#define EECR (*(volatile unsigned char *)0x3F)
#define EERE  0 // EEPROM Read Enable
#define EEPE  1 // EEPROM Write Enable
#define EEMPE 2 // EEPROM Master Write Enable
#define EERIE 3 // EEPROM Ready Interrupt Enable
#define EEPM0 4 // EEPROM Programming Mode Bits
#define EEPM1 5 // EEPROM Programming Mode Bits

/*
 * EEDR – The EEPROM Data Register
 */
#define EEDR (*(volatile unsigned char *)0x40)
#define EEDR0 0 // Data Register
#define EEDR1 1 // Data Register
#define EEDR2 2 // Data Register
#define EEDR3 3 // Data Register
#define EEDR4 4 // Data Register
#define EEDR5 5 // Data Register
#define EEDR6 6 // Data Register
#define EEDR7 7 // Data Register

/*
 * EEAR – The EEPROM Address Register
 */
#define EEAR (*(volatile unsigned char *)0x41)

/*
 * EEARH and EEARL – The EEPROM Address Register
 */
#define EEARL (*(volatile unsigned char *)0x41)
#define EEAR0 0 // Address Register
#define EEAR1 1 // Address Register
#define EEAR2 2 // Address Register
#define EEAR3 3 // Address Register
#define EEAR4 4 // Address Register
#define EEAR5 5 // Address Register
#define EEAR6 6 // Address Register
#define EEAR7 7 // Address Register

#define EEARH (*(volatile unsigned char *)0x42)
#define EEAR8 0 // Address Register
#define EEAR9 1 // Address Register

/*
 * GTCCR – General Timer/Counter Control Register
 */
#define GTCCR (*(volatile unsigned char *)0x43)
#define PSRSYNC 0 // Prescaler Reset
#define PSRASY  1 // Reset Timer/Counter2
#define TSM     7 // Timer/Counter Synchronization Mode

/*
 * TCCR0A – Timer/Counter Control Register A
 */
#define TCCR0A (*(volatile unsigned char *)0x44)
#define WGM00   0 // Waveform Generation Mode
#define WGM01   1 // Waveform Generation Mode
#define COM0B0  4 // Compare Match Output B Mode
#define COM0B1  5 // Compare Match Output B Mode
#define COM0A0  6 // Compare Match Output A Mode
#define COM0A1  7 // Compare Match Output A Mode

/*
 * TCCR0B – Timer/Counter Control Register B
 */
#define TCCR0B (*(volatile unsigned char *)0x45)
#define CS00    0 // Clock Select
#define CS01    1 // Clock Select
#define CS02    2 // Clock Select
#define WGM02   3 // Waveform Generation Mode
#define FOC0B   6 // Force Output Compare B
#define FOC0A   7 // Force Output Compare A

/*
 * TCNT0 – Timer/Counter Register
 */
#define TCNT0 (*(volatile unsigned char *)0x46)
#define TCNT0_0 0 // Timer/Counter Register
#define TCNT0_1 1 // Timer/Counter Register
#define TCNT0_2 2 // Timer/Counter Register
#define TCNT0_3 3 // Timer/Counter Register
#define TCNT0_4 4 // Timer/Counter Register
#define TCNT0_5 5 // Timer/Counter Register
#define TCNT0_6 6 // Timer/Counter Register
#define TCNT0_7 7 // Timer/Counter Register

/*
 * OCR0A – Output Compare Register A
 */
#define OCR0A (*(volatile unsigned char *)0x47)
#define OCR0A_0 0 // Output Compare Register
#define OCR0A_1 1 // Output Compare Register
#define OCR0A_2 2 // Output Compare Register
#define OCR0A_3 3 // Output Compare Register
#define OCR0A_4 4 // Output Compare Register
#define OCR0A_5 5 // Output Compare Register
#define OCR0A_6 6 // Output Compare Register
#define OCR0A_7 7 // Output Compare Register

/*
 * OCR0B – Output Compare Register B
 */
#define OCR0B (*(volatile unsigned char *)0x48)
#define OCR0B_0 0 // Output Compare Register
#define OCR0B_1 1 // Output Compare Register
#define OCR0B_2 2 // Output Compare Register
#define OCR0B_3 3 // Output Compare Register
#define OCR0B_4 4 // Output Compare Register
#define OCR0B_5 5 // Output Compare Register
#define OCR0B_6 6 // Output Compare Register
#define OCR0B_7 7 // Output Compare Register

/*
 * GPIOR1 – General Purpose I/O Register 1
 */
#define GPIOR1 (*(volatile unsigned char *)0x4A)
#define GPIOR10 0 // I/O Register
#define GPIOR11 1 // I/O Register
#define GPIOR12 2 // I/O Register
#define GPIOR13 3 // I/O Register
#define GPIOR14 4 // I/O Register
#define GPIOR15 5 // I/O Register
#define GPIOR16 6 // I/O Register
#define GPIOR17 7 // I/O Register

/*
 * GPIOR2 – General Purpose I/O Register 2
 */
#define GPIOR2 (*(volatile unsigned char *)0x4B)
#define GPIOR20 0 // I/O Register
#define GPIOR21 1 // I/O Register
#define GPIOR22 2 // I/O Register
#define GPIOR23 3 // I/O Register
#define GPIOR24 4 // I/O Register
#define GPIOR25 5 // I/O Register
#define GPIOR26 6 // I/O Register
#define GPIOR27 7 // I/O Register

/*
 * SPCR – SPI Control Register
 */
#define SPCR (*(volatile unsigned char *)0x4C)
#define SPR0 0 // SPI Clock Rate Select
#define SPR1 1 // SPI Clock Rate Select
#define CPHA 2 // Clock Phase
#define CPOL 3 // Clock Polarity
#define MSTR 4 // Master/Slave Select
#define DORD 5 // Data Order
#define SPE  6 // SPI Enable
#define SPIE 7 // SPI Interrupt Enable

/*
 * SPSR – SPI Status Register
 */
#define SPSR (*(volatile unsigned char *)0x4D)
#define SPI2X 0 // Double SPI Speed Bit
#define WCOL  6 // Write COLlision Flag
#define SPIF  7 // SPI Interrupt Flag

/*
 * SPDR – SPI Data Register
 */
#define SPDR (*(volatile unsigned char *)0x4E)
#define SPDR0 0 // Data Register
#define SPDR1 1 // Data Register
#define SPDR2 2 // Data Register
#define SPDR3 3 // Data Register
#define SPDR4 4 // Data Register
#define SPDR5 5 // Data Register
#define SPDR6 6 // Data Register
#define SPDR7 7 // Data Register

/*
 * ACSR – Analog Comparator Control and Status Register
 */
#define ACSR (*(volatile unsigned char *)0x50)
#define ACIS0 0 // Analog Comparator Interrupt Mode Select
#define ACIS1 1 // Analog Comparator Interrupt Mode Select
#define ACIC  2 // Analog Comparator Input Capture Enable
#define ACIE  3 // Analog Comparator Interrupt Enable
#define ACI   4 // Analog Comparator Interrupt Flag
#define ACO   5 // Analog Comparator Output
#define ACBG  6 // Analog Comparator Bandgap Select
#define ACD   7 // Analog Comparator Disable

/*
 * SMCR – Sleep Mode Control Register
 */
#define SMCR (*(volatile unsigned char *)0x53)
#define SE  0 // Sleep Enable
#define SM0 1 // Sleep Mode Select Bits 0
#define SM1 2 // Sleep Mode Select Bits 1
#define SM2 2 // Sleep Mode Select Bits 2

//Sleep Mode Define
#define idle                0x00
#define ADC_noise_reduction 0x01
#define Power_down          0x02
#define Standby             0x06
#define Power_save          0x07

/*
 * MCUSR – MCU Status Register
 */
#define MCUSR (*(volatile unsigned char *)0x54)
#define PORF  0 // Power-on Reset Flag
#define EXTRF 1 // External Reset Flag
#define BORF  2 // Brown-out Reset Flag
#define WDRF  3 // Watchdog System Reset Flag

/*
 * MCUCR – MCU Control Register
 */
#define MCUCR (*(volatile unsigned char *)0x55)
#define IVCE  0 // Interrupt Vector Change Enable
#define IVSEL 1 // Interrupt Vector Select
#define PUD   4 // Pull-up Disable
#define BODSE 5 // BOD Sleep Enable
#define BODS  6 // BOD Sleep

/*
 * SPMCSR – Store Program Memory Control and Status Register
 */
#define SPMCSR (*(volatile unsigned char *)0x57)
#define SELFPRGEN 0 // Self Programming Enable
#define PGERS     1 // Page Erase
#define PGWRT     2 // Page Write
#define BLBSET    3 // Boot Lock Bit Set
#define RWWSRE    4 // Read-While-Write Section Read Enable
#define RWWSB     6 // Read-While-Write Section Busy
#define SPMIE     7 // SPM Interrupt Enable

/*
 * SP – Stack Pointer High and Stack Pointer Low Register
 */
#define SP  (*(volatile unsigned char *)0x5C)

/*
 * SPH and SPL – Stack Pointer High and Stack Pointer Low Register
 */
#define SPL  (*(volatile unsigned char *)0x5D)
#define SP0 0
#define SP1 1
#define SP2 2
#define SP3 3
#define SP4 4
#define SP5 5
#define SP6 6
#define SP7 7

#define SPH  (*(volatile unsigned char *)0x5E)
#define SP8  0
#define SP9  1
#define SP10 2
#define SP11 3
#define SP12 4
#define SP13 5
#define SP14 6
#define SP15 7

/*
 * SREG – AVR Status Register
 */
#define SREG   (*(volatile unsigned char *)0x5F)
#define C 0
#define Z 1
#define N 2
#define V 3
#define S 4
#define H 5
#define T 6
#define I 7

/*
 * WDTCSR – Watchdog Timer Control Register
 */
#define WDTCSR (*(volatile unsigned char *)0x60)
#define WDP0 0 // Watchdog Timer Prescaler
#define WDP1 1 // Watchdog Timer Prescaler
#define WDP2 2 // Watchdog Timer Prescaler
#define WDE  3 // Watchdog System Reset Enable
#define WDCE 4 // Watchdog Change Enable
#define WDP3 5 // Watchdog Timer Prescaler
#define WDIE 6 // Watchdog Interrupt Enable
#define WDIF 7 // Watchdog Interrupt Flag

/*
 * CLKPR – Clock Prescale Register
 */
#define CLKPR (*(volatile unsigned char *)0x61)
#define CLKPS0 0 // Clock Prescaler Select Bits
#define CLKPS1 1 // Clock Prescaler Select Bits
#define CLKPS2 2 // Clock Prescaler Select Bits
#define CLKPS3 3 // Clock Prescaler Select Bits
#define CLKPCE 7 // Clock Prescaler Change Enable

/*
 * PRR – Power Reduction Register
 */
#define PRR (*(volatile unsigned char *)0x64)
#define PRADC    0 // Power Reduction ADC
#define PRUSART0 1 // Power Reduction USART0
#define PRSPI    2 // Power Reduction Serial Peripheral Interface
#define PRTIM1   3 // Power Reduction Timer/Counter1
#define PRTIM0   5 // Power Reduction Timer/Counter0
#define PRTIM2   6 // Power Reduction Timer/Counter2
#define PRTWI    7 // Power Reduction TWI

/*
 * OSCCAL – Oscillator Calibration Register
 */
#define OSCCAL (*(volatile unsigned char *)0x66)
#define CAL0 0 // Oscillator Calibration Value
#define CAL1 1 // Oscillator Calibration Value
#define CAL2 2 // Oscillator Calibration Value
#define CAL3 3 // Oscillator Calibration Value
#define CAL4 4 // Oscillator Calibration Value
#define CAL5 5 // Oscillator Calibration Value
#define CAL6 6 // Oscillator Calibration Value
#define CAL7 7 // Oscillator Calibration Value

/*
 * PCICR – Pin Change Interrupt Control Register
 */
#define PCICR (*(volatile unsigned char *)0x68)
#define PCIE0 0 // Pin Change Interrupt Enable 0
#define PCIE1 1 // Pin Change Interrupt Enable 1
#define PCIE2 2 // Pin Change Interrupt Enable 2

/*
 * EICRA – External Interrupt Control Register A
 */
#define EICRA (*(volatile unsigned char *)0x69)
#define ISC00 0 // Interrupt Sense Control
#define ISC01 1 // Interrupt Sense Control
#define ISC10 2 // Interrupt Sense Control
#define ISC11 3 // Interrupt Sense Control

/*
 * PCMSK0 – Pin Change Mask Register 0
 */
#define PCMSK0 (*(volatile unsigned char *)0x6B)
#define PCINT0 0 // Pin Change Enable Mask
#define PCINT1 1 // Pin Change Enable Mask
#define PCINT2 2 // Pin Change Enable Mask
#define PCINT3 3 // Pin Change Enable Mask
#define PCINT4 4 // Pin Change Enable Mask
#define PCINT5 5 // Pin Change Enable Mask
#define PCINT6 6 // Pin Change Enable Mask
#define PCINT7 7 // Pin Change Enable Mask

/*
 * PCMSK1 – Pin Change Mask Register 1
 */
#define PCMSK1 (*(volatile unsigned char *)0x6C)
#define PCINT8  0 // Pin Change Enable Mask
#define PCINT9  1 // Pin Change Enable Mask
#define PCINT10 2 // Pin Change Enable Mask
#define PCINT11 3 // Pin Change Enable Mask
#define PCINT12 4 // Pin Change Enable Mask
#define PCINT13 5 // Pin Change Enable Mask
#define PCINT14 6 // Pin Change Enable Mask

/*
 * PCMSK2 – Pin Change Mask Register 2
 */
#define PCMSK2 (*(volatile unsigned char *)0x6D)
#define PCINT16 0 // Pin Change Enable Mask
#define PCINT17 1 // Pin Change Enable Mask
#define PCINT18 2 // Pin Change Enable Mask
#define PCINT19 3 // Pin Change Enable Mask
#define PCINT20 4 // Pin Change Enable Mask
#define PCINT21 5 // Pin Change Enable Mask
#define PCINT22 6 // Pin Change Enable Mask
#define PCINT23 7 // Pin Change Enable Mask

/*
 * TIMSK0 – Timer/Counter 0 Interrupt Mask Register
 */
#define TIMSK0 (*(volatile unsigned char *)0x6e)
#define TOIE0   0 // Timer/Counter0 Overflow Interrupt Enable
#define OCIE0A  1 // Timer/Counter0 Output Compare Match A Interrupt Enable
#define OCIE0B  2 //Timer/Counter Output Compare Match B Interrupt Enable

/*
 * TIMSK1 – Timer/Counter 1 Interrupt Mask Register
 */
#define TIMSK1 (*(volatile unsigned char *)0x6F)
#define TOIE1  0 // Timer/Counter1, Overflow Interrupt Enable
#define OCIE1A 1 // Timer/Counter1, Output Compare A Match Interrupt Enable
#define OCIE1B 2 // Timer/Counter1, Output Compare B Match Interrupt Enable
#define ICIE1  5 // Timer/Counter1, Input Capture Interrupt Enable


/*
 * TIMSK2 – Timer/Counter 2 Interrupt Mask Register
 */
#define TIMSK2 (*(volatile unsigned char *)0x70)
#define TOIE2   0 // Timer/Counter2 Overflow Interrupt Enable
#define OCIE2A  1 // Timer/Counter2 Output Compare Match A Interrupt Enable
#define OCIE2B  2 //Timer/Counter Output Compare Match B Interrupt Enable

/*
 * ADC – The ADC Data Register
 */
#define ADC (*(volatile unsigned char *)0x78)

/*
 * ADCL and ADCH – The ADC Data Register
 */
#define ADCL (*(volatile unsigned char *)0x78)
#define ADCL0 0 // Data Register
#define ADCL1 1 // Data Register
#define ADCL2 2 // Data Register
#define ADCL3 3 // Data Register
#define ADCL4 4 // Data Register
#define ADCL5 5 // Data Register
#define ADCL6 6 // Data Register
#define ADCL7 7 // Data Register

#define ADCH (*(volatile unsigned char *)0x79)
#define ADCH0 0 // Data Register
#define ADCH1 1 // Data Register
#define ADCH2 2 // Data Register
#define ADCH3 3 // Data Register
#define ADCH4 4 // Data Register
#define ADCH5 5 // Data Register
#define ADCH6 6 // Data Register
#define ADCH7 7 // Data Register

/*
 * ADCSRA – ADC Control and Status Register A
 */
#define ADCSRA (*(volatile unsigned char *)0x7A)
#define ADPS0 0 // ADC Prescaler Select Bits
#define ADPS1 1 // ADC Prescaler Select Bits
#define ADPS2 2 // ADC Prescaler Select Bits
#define ADIE  3 // ADC Interrupt Enable
#define ADIF  4 // ADC Interrupt Flag
#define ADATE 5 // ADC Auto Trigger Enable
#define ADSC  6 // ADC Start Conversion
#define ADEN  7 // ADC Enable

/*
 * ADCSRB – ADC Control and Status Register B
 */
#define ADCSRB (*(volatile unsigned char *)0x7B)
#define ADTS0 0 // ADC Auto Trigger Source
#define ADTS1 1 // ADC Auto Trigger Source
#define ADTS2 2 // ADC Auto Trigger Source
#define ACME  6 // Analog Comparator Multiplexer Enable

/*
 * ADMUX – ADC Multiplexer Selection Register
 */
#define ADMUX (*(volatile unsigned char *)0x7C)
#define MUX0  0 // Analog Channel Selection Bits
#define MUX1  1 // Analog Channel Selection Bits
#define MUX2  2 // Analog Channel Selection Bits
#define MUX3  3 // Analog Channel Selection Bits
#define ADLAR 5 // ADC Left Adjust Result
#define REFS0 6 // Reference Selection Bits
#define REFS1 7 // Reference Selection Bits

/*
 * DIDR0 – Digital Input Disable Register 0
 */
#define DIDR0 (*(volatile unsigned char *)0x7E)
#define ADC0D 0 // Digital Input Disable
#define ADC1D 1 // Digital Input Disable
#define ADC2D 2 // Digital Input Disable
#define ADC3D 3 // Digital Input Disable
#define ADC4D 4 // Digital Input Disable
#define ADC5D 5 // Digital Input Disable

/*
 * 
 */
#define DIDR1 (*(volatile unsigned char *)0x7F)
#define AIN0D 0
#define AIN1D 1

/*
 * TCCR1A – Timer/Counter Control Register A
 */
#define TCCR1A (*(volatile unsigned char *)0x80)
#define WGM10   0 // Waveform Generation Mode
#define WGM11   1 // Waveform Generation Mode
#define COM1B0  4 // Compare Match Output B Mode
#define COM1B1  5 // Compare Match Output B Mode
#define COM1A0  6 // Compare Match Output A Mode
#define COM1A1  7 // Compare Match Output A Mode

/*
 * TCCR1B – Timer/Counter Control Register B
 */
#define TCCR1B (*(volatile unsigned char *)0x81)
#define CS10    0 // Clock Select
#define CS11    1 // Clock Select
#define CS12    2 // Clock Select
#define WGM12   3 // Waveform Generation Mode
#define WGM13   4 // Waveform Generation Mode
#define ICES1   6 // Force Output Compare B
#define ICNC1   7 // Force Output Compare A


/*
 * TCCR1C – Timer/Counter Control Register C
 */
#define TCCR1C (*(volatile unsigned char *)0x82)
#define FOC1B   6 // Force Output Compare for Channel B
#define FOC1A   7 // Force Output Compare for Channel A

/*
 * TCNT1 – Timer/Counter1
 */
#define TCNT1 _SFR_MEM16(0x84)

/*
 * TCNT1H and TCNT1L – Timer/Counter1
 */
#define TCNT1L (*(volatile unsigned char *)0x84)
#define TCNT1L_0 0 // Timer/Counter Register
#define TCNT1L_1 1 // Timer/Counter Register
#define TCNT1L_2 2 // Timer/Counter Register
#define TCNT1L_3 3 // Timer/Counter Register
#define TCNT1L_4 4 // Timer/Counter Register
#define TCNT1L_5 5 // Timer/Counter Register
#define TCNT1L_6 6 // Timer/Counter Register
#define TCNT1L_7 7 // Timer/Counter Register

#define TCNT1H (*(volatile unsigned char *)0x85)
#define TCNT1H_0 0 // Timer/Counter Register
#define TCNT1H_1 1 // Timer/Counter Register
#define TCNT1H_2 2 // Timer/Counter Register
#define TCNT1H_3 3 // Timer/Counter Register
#define TCNT1H_4 4 // Timer/Counter Register
#define TCNT1H_5 5 // Timer/Counter Register
#define TCNT1H_6 6 // Timer/Counter Register
#define TCNT1H_7 7 // Timer/Counter Register

/*
 *  ICR1 – Input Capture Register 1
 */
#define ICR1 _SFR_MEM16(0x86)

/*
 *  ICR1H and ICR1L – Input Capture Register 1
 */
#define ICR1L (*(volatile unsigned char *)0x86)
#define ICR1L0 0 // Input Capture Register 1
#define ICR1L1 1 // Input Capture Register 1
#define ICR1L2 2 // Input Capture Register 1
#define ICR1L3 3 // Input Capture Register 1
#define ICR1L4 4 // Input Capture Register 1
#define ICR1L5 5 // Input Capture Register 1
#define ICR1L6 6 // Input Capture Register 1
#define ICR1L7 7 // Input Capture Register 1

#define ICR1H (*(volatile unsigned char *)0x87)
#define ICR1H0 0 // Input Capture Register 1
#define ICR1H1 1 // Input Capture Register 1
#define ICR1H2 2 // Input Capture Register 1
#define ICR1H3 3 // Input Capture Register 1
#define ICR1H4 4 // Input Capture Register 1
#define ICR1H5 5 // Input Capture Register 1
#define ICR1H6 6 // Input Capture Register 1
#define ICR1H7 7 // Input Capture Register 1

/*
 * OCR1A – Output Compare Register 1 A
 */
#define OCR1A _SFR_MEM16(0x88)

/*
 * OCR1AH and OCR1AL – Output Compare Register 1 A
 */
#define OCR1AL (*(volatile unsigned char *)0x88)
#define OCR1AL_0 0 // Output Compare Register 1 A
#define OCR1AL_1 1 // Output Compare Register 1 A
#define OCR1AL_2 2 // Output Compare Register 1 A
#define OCR1AL_3 3 // Output Compare Register 1 A
#define OCR1AL_4 4 // Output Compare Register 1 A
#define OCR1AL_5 5 // Output Compare Register 1 A
#define OCR1AL_6 6 // Output Compare Register 1 A
#define OCR1AL_7 7 // Output Compare Register 1 A

#define OCR1AH (*(volatile unsigned char *)0x89)
#define OCR1AH_0 0 // Output Compare Register 1 A
#define OCR1AH_1 1 // Output Compare Register 1 A
#define OCR1AH_2 2 // Output Compare Register 1 A
#define OCR1AH_3 3 // Output Compare Register 1 A
#define OCR1AH_4 4 // Output Compare Register 1 A
#define OCR1AH_5 5 // Output Compare Register 1 A
#define OCR1AH_6 6 // Output Compare Register 1 A
#define OCR1AH_7 7 // Output Compare Register 1 A

/*
 * OCR1B – Output Compare Register 1 B
 */
#define OCR1B _SFR_MEM16(0x8A)

/*
 * OCR1BH and OCR1BL – Output Compare Register 1 B
 */
#define OCR1BL (*(volatile unsigned char *)0x8A)
#define OCR1BL_0 0 // Output Compare Register 1 B
#define OCR1BL_1 1 // Output Compare Register 1 B
#define OCR1BL_2 2 // Output Compare Register 1 B
#define OCR1BL_3 3 // Output Compare Register 1 B
#define OCR1BL_4 4 // Output Compare Register 1 B
#define OCR1BL_5 5 // Output Compare Register 1 B
#define OCR1BL_6 6 // Output Compare Register 1 B
#define OCR1BL_7 7 // Output Compare Register 1 B

#define OCR1BH (*(volatile unsigned char *)0x8B)
#define OCR1BH_0 0 // Output Compare Register 1 B
#define OCR1BH_1 1 // Output Compare Register 1 B
#define OCR1BH_2 2 // Output Compare Register 1 B
#define OCR1BH_3 3 // Output Compare Register 1 B
#define OCR1BH_4 4 // Output Compare Register 1 B
#define OCR1BH_5 5 // Output Compare Register 1 B
#define OCR1BH_6 6 // Output Compare Register 1 B
#define OCR1BH_7 7 // Output Compare Register 1 B

/*
 * TCCR2A – Timer/Counter Control Register A
 */
#define TCCR2A (*(volatile unsigned char *)0xB0)
#define WGM20   0 // Waveform Generation Mode
#define WGM21   1 // Waveform Generation Mode
#define COM2B0  4 // Compare Match Output B Mode
#define COM2B1  5 // Compare Match Output B Mode
#define COM2A0  6 // Compare Match Output A Mode
#define COM2A1  7 // Compare Match Output A Mode

/*
 * TCCR2B – Timer/Counter Control Register B
 */
#define TCCR2B (*(volatile unsigned char *)0xB1)
#define CS20    0 // Clock Select
#define CS21    1 // Clock Select
#define CS22    2 // Clock Select
#define WGM22   3 // Waveform Generation Mode
#define FOC2B   6 // Force Output Compare B
#define FOC2A   7 // Force Output Compare A

/*
 * TCNT2 – Timer/Counter Register
 */
#define TCNT2 (*(volatile unsigned char *)0xB2)
#define TCNT2_0 0 // Timer/Counter Register
#define TCNT2_1 1 // Timer/Counter Register
#define TCNT2_2 2 // Timer/Counter Register
#define TCNT2_3 3 // Timer/Counter Register
#define TCNT2_4 4 // Timer/Counter Register
#define TCNT2_5 5 // Timer/Counter Register
#define TCNT2_6 6 // Timer/Counter Register
#define TCNT2_7 7 // Timer/Counter Register

/*
 * OCR2A – Output Compare Register A
 */
#define OCR2A (*(volatile unsigned char *)0xB3)
#define OCR2A_0 0 // Output Compare Register
#define OCR2A_1 1 // Output Compare Register
#define OCR2A_2 2 // Output Compare Register
#define OCR2A_3 3 // Output Compare Register
#define OCR2A_4 4 // Output Compare Register
#define OCR2A_5 5 // Output Compare Register
#define OCR2A_6 6 // Output Compare Register
#define OCR2A_7 7 // Output Compare Register

/*
 * OCR2B – Output Compare Register B
 */
#define OCR2B (*(volatile unsigned char *)0xB4)
#define OCR2B_0 0 // Output Compare Register
#define OCR2B_1 1 // Output Compare Register
#define OCR2B_2 2 // Output Compare Register
#define OCR2B_3 3 // Output Compare Register
#define OCR2B_4 4 // Output Compare Register
#define OCR2B_5 5 // Output Compare Register
#define OCR2B_6 6 // Output Compare Register
#define OCR2B_7 7 // Output Compare Register

/*
 * ASSR – Asynchronous Status Register
 */
#define ASSR (*(volatile unsigned char *)0xB6)
#define TCR2BUB 0 // Timer/Counter Control Register2 Update Busy
#define TCR2AUB 1 // Timer/Counter Control Register2 Update Busy
#define OCR2BUB 2 // Output Compare Register2 Update Busy
#define OCR2AUB 3 // Output Compare Register2 Update Busy
#define TCN2UB  4 // Timer/Counter2 Update Busy
#define AS2     5 // Asynchronous Timer/Counter2
#define EXCLK   6 // Enable External Clock Input

/*
 * TWBR – TWI Bit Rate Register
 */
#define TWBR (*(volatile unsigned char *)0xB8)
#define TWBR0 0 // TWI Bit Rate Register
#define TWBR1 1 // TWI Bit Rate Register
#define TWBR2 2 // TWI Bit Rate Register
#define TWBR3 3 // TWI Bit Rate Register
#define TWBR4 4 // TWI Bit Rate Register
#define TWBR5 5 // TWI Bit Rate Register
#define TWBR6 6 // TWI Bit Rate Register
#define TWBR7 7 // TWI Bit Rate Register

/*
 * TWSR – TWI Status Register
 */
#define TWSR (*(volatile unsigned char *)0xB9)
#define TWPS0 0 // TWI Prescaler Bits
#define TWPS1 1 // TWI Prescaler Bits
#define TWS3  3 // TWI Status
#define TWS4  4 // TWI Status
#define TWS5  5 // TWI Status
#define TWS6  6 // TWI Status
#define TWS7  7 // TWI Status

/*
 * TWAR – TWI (Slave) Address Register
 */
#define TWAR (*(volatile unsigned char *)0xBA)
#define TWGCE 0 // TWI General Call Recognition Enable Bit
#define TWA0  1 // TWI (Slave) Address Register
#define TWA1  2 // TWI (Slave) Address Register
#define TWA2  3 // TWI (Slave) Address Register
#define TWA3  4 // TWI (Slave) Address Register
#define TWA4  5 // TWI (Slave) Address Register
#define TWA5  6 // TWI (Slave) Address Register
#define TWA6  7 // TWI (Slave) Address Register

/*
 * TWDR – TWI Data Register
 */
#define TWDR (*(volatile unsigned char *)0xBB)
#define TWD0 0 // TWI Data Register
#define TWD1 1 // TWI Data Register
#define TWD2 2 // TWI Data Register
#define TWD3 3 // TWI Data Register
#define TWD4 4 // TWI Data Register
#define TWD5 5 // TWI Data Register
#define TWD6 6 // TWI Data Register
#define TWD7 7 // TWI Data Register

/*
 * TWCR – TWI Control Register
 */
#define TWCR (*(volatile unsigned char *)0xBC)
#define TWIE  0 // TWI Interrupt Enable
#define TWEN  2 // TWI Enable Bit
#define TWWC  3 // TWI Write Collision Flag
#define TWSTO 4 // TWI STOP Condition Bit
#define TWSTA 5 // TWI START Condition Bit
#define TWEA  6 // TWI Enable Acknowledge Bit
#define TWINT 7 // TWI Interrupt Flag

/*
 * TWAMR – TWI (Slave) Address Mask Register
 */
#define TWAMR (*(volatile unsigned char *)0xBD)
#define TWAM0 0 // TWI Address Mask
#define TWAM1 1 // TWI Address Mask
#define TWAM2 2 // TWI Address Mask
#define TWAM3 3 // TWI Address Mask
#define TWAM4 4 // TWI Address Mask
#define TWAM5 5 // TWI Address Mask
#define TWAM6 6 // TWI Address Mask

/*
 *UCSRnA – USART Control and Status Register n A
 */
#define UCSR0A (*(volatile unsigned char *)0xC0)
#define MPCM0   0 //Multi-processor Communication Mode
#define U2X0    1 //Double the USART Transmission Speed
#define UPE0    2 //USART Parity Error
#define DOR0    3 //Data OverRun
#define FE0     4 //Frame Error
#define UDRE0   5 //USART Data Register Empty
#define TXC0    6 //USART Transmit Complete
#define RXC0    7 //USART Receive Complete

/*
 *UCSRnB – USART Control and Status Register n B
 */
#define UCSR0B (*(volatile unsigned char *)0xC1)
#define RXB80   0 //Transmit Data Bit 8 n
#define TXB80   1 //Receive Data Bit 8 n
#define UCSZ02  2 //Character Size n
#define TXEN0   3 //TXENn: Transmitter Enable n
#define RXEN0   4 //Receiver Enable n
#define UDRIE0  5 //USART Data Register Empty Interrupt Enable n
#define TXCIE0  6 //TX Complete Interrupt Enable n
#define RXCIE0  7 //RX Complete Interrupt Enable n

/*
 *UCSRnC – USART Control and Status Register n C
 */
#define UCSR0C (*(volatile unsigned char *)0xC2)
#define UCPOL0  0 //Clock Polarity
#define UCSZ00  1 //Character Size 0
#define UCSZ01  2 //Character Size 1
#define USBS0   3 //Stop Bit Select
#define UPM00   4 //Parity Mode 0
#define UPM01   5 //Parity Mode 1
#define UMSEL00 6 //USART Mode Select 0
#define UMSEL01 7 //USART Mode Select 1

/*
 * UBRR0 – USART Baud Rate Registers
 */
#define UBRR0 (*(volatile unsigned char *)0xC4)

/*
 * UBRR0L and UBRR0H – USART Baud Rate Registers
 */
#define UBRR0L (*(volatile unsigned char *)0xC4)
#define UBRR0_0 0 // Baud Rate Registers
#define UBRR0_1 1 // Baud Rate Registers
#define UBRR0_2 2 // Baud Rate Registers
#define UBRR0_3 3 // Baud Rate Registers
#define UBRR0_4 4 // Baud Rate Registers
#define UBRR0_5 5 // Baud Rate Registers
#define UBRR0_6 6 // Baud Rate Registers
#define UBRR0_7 7 // Baud Rate Registers

#define UBRR0H (*(volatile unsigned char *)0xC5)
#define UBRR0_8  0 // Baud Rate Registers
#define UBRR0_9  1 // Baud Rate Registers
#define UBRR0_10 2 // Baud Rate Registers
#define UBRR0_11 3 // Baud Rate Registers

/*
 * UDR0 – USART I/O Data Register 0
 */
#define UDR0 (*(volatile unsigned char *)0xC6)
#define UDR0_0 0 // Data Register
#define UDR0_1 1 // Data Register
#define UDR0_2 2 // Data Register
#define UDR0_3 3 // Data Register
#define UDR0_4 4 // Data Register
#define UDR0_5 5 // Data Register
#define UDR0_6 6 // Data Register
#define UDR0_7 7 // Data Register

#endif
