 

/*
 *  AK4531 codec registers
 */

#define AK4531_LMASTER  0x00	/* master volume left */
#define AK4531_RMASTER  0x01	/* master volume right */
#define AK4531_LVOICE   0x02	/* channel volume left */
#define AK4531_RVOICE   0x03	/* channel volume right */
#define AK4531_LFM      0x04	/* FM volume left */
#define AK4531_RFM      0x05	/* FM volume right */
#define AK4531_LCD      0x06	/* CD volume left */
#define AK4531_RCD      0x07	/* CD volume right */
#define AK4531_LLINE    0x08	/* LINE volume left */
#define AK4531_RLINE    0x09	/* LINE volume right */
#define AK4531_LAUXA    0x0a	/* AUXA volume left */
#define AK4531_RAUXA    0x0b	/* AUXA volume right */
#define AK4531_MONO1    0x0c	/* MONO1 volume left */
#define AK4531_MONO2    0x0d	/* MONO1 volume right */
#define AK4531_MIC      0x0e	/* MIC volume */
#define AK4531_MONO_OUT 0x0f	/* Mono-out volume */
#define AK4531_OUT_SW1  0x10	/* Output mixer switch 1 */
#define AK4531_OUT_SW2  0x11	/* Output mixer switch 2 */
#define AK4531_LIN_SW1  0x12	/* Input left mixer switch 1 */
#define AK4531_RIN_SW1  0x13	/* Input right mixer switch 1 */
#define AK4531_LIN_SW2  0x14	/* Input left mixer switch 2 */
#define AK4531_RIN_SW2  0x15	/* Input right mixer switch 2 */
#define AK4531_RESET    0x16	/* Reset & power down */
#define AK4531_CLOCK    0x17	/* Clock select */
#define AK4531_AD_IN    0x18	/* AD input select */
#define AK4531_MIC_GAIN 0x19	/* MIC amplified gain */

