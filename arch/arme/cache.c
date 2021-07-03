



/** \brief Enable D-Cache

    The function turns on D-Cache
  */
__STATIC_INLINE void SCB_EnableDCache (void)
{
  #if (__DCACHE_PRESENT == 1)
    uint32_t ccsidr, sshift, wshift, sw;
    uint32_t sets, ways;

    SCB->CSSELR = (0UL << 1) | 0UL;         // Level 1 data cache
    ccsidr  = SCB->CCSIDR;
    sets    = (uint32_t)(CCSIDR_SETS(ccsidr));
    sshift  = (uint32_t)(CCSIDR_LSSHIFT(ccsidr) + 4UL);
    ways    = (uint32_t)(CCSIDR_WAYS(ccsidr));
    wshift  = (uint32_t)((uint32_t)__CLZ(ways) & 0x1FUL);

    __DSB();

    do {                                   // invalidate D-Cache
         uint32_t tmpways = ways;
         do {
              sw = ((tmpways << wshift) | (sets << sshift));
              SCB->DCISW = sw;
            } while(tmpways--);
        } while(sets--);
    __DSB();

    SCB->CCR |=  (uint32_t)SCB_CCR_DC_Msk;   // enable D-Cache

    __DSB();
    __ISB();
  #endif
}


/** \brief Disable D-Cache

    The function turns off D-Cache
  */
__STATIC_INLINE void SCB_DisableDCache (void)
{
  #if (__DCACHE_PRESENT == 1)
    uint32_t ccsidr, sshift, wshift, sw;
    uint32_t sets, ways;

    SCB->CSSELR = (0UL << 1) | 0UL;         // Level 1 data cache
    ccsidr  = SCB->CCSIDR;
    sets    = (uint32_t)(CCSIDR_SETS(ccsidr));
    sshift  = (uint32_t)(CCSIDR_LSSHIFT(ccsidr) + 4UL);
    ways    = (uint32_t)(CCSIDR_WAYS(ccsidr));
    wshift  = (uint32_t)((uint32_t)__CLZ(ways) & 0x1FUL);

    __DSB();

    SCB->CCR &= ~(uint32_t)SCB_CCR_DC_Msk;  // disable D-Cache

    do {                                    // clean & invalidate D-Cache
         uint32_t tmpways = ways;
         do {
              sw = ((tmpways << wshift) | (sets << sshift));
              SCB->DCCISW = sw;
            } while(tmpways--);
        } while(sets--);


    __DSB();
    __ISB();
  #endif
}


/** \brief Invalidate D-Cache

    The function invalidates D-Cache
  */
__STATIC_INLINE void SCB_InvalidateDCache (void)
{
  #if (__DCACHE_PRESENT == 1)
    uint32_t ccsidr, sshift, wshift, sw;
    uint32_t sets, ways;

    SCB->CSSELR = (0UL << 1) | 0UL;         // Level 1 data cache
    ccsidr  = SCB->CCSIDR;
    sets    = (uint32_t)(CCSIDR_SETS(ccsidr));
    sshift  = (uint32_t)(CCSIDR_LSSHIFT(ccsidr) + 4UL);
    ways    = (uint32_t)(CCSIDR_WAYS(ccsidr));
    wshift  = (uint32_t)((uint32_t)__CLZ(ways) & 0x1FUL);

    __DSB();

    do {                                    // invalidate D-Cache
         uint32_t tmpways = ways;
         do {
              sw = ((tmpways << wshift) | (sets << sshift));
              SCB->DCISW = sw;
            } while(tmpways--);
        } while(sets--);

    __DSB();
    __ISB();
  #endif
}


/** \brief Clean D-Cache

    The function cleans D-Cache
  */
__STATIC_INLINE void SCB_CleanDCache (void)
{
  #if (__DCACHE_PRESENT == 1)
    uint32_t ccsidr, sshift, wshift, sw;
    uint32_t sets, ways;

    SCB->CSSELR = (0UL << 1) | 0UL;         // Level 1 data cache
    ccsidr  = SCB->CCSIDR;
    sets    = (uint32_t)(CCSIDR_SETS(ccsidr));
    sshift  = (uint32_t)(CCSIDR_LSSHIFT(ccsidr) + 4UL);
    ways    = (uint32_t)(CCSIDR_WAYS(ccsidr));
    wshift  = (uint32_t)((uint32_t)__CLZ(ways) & 0x1FUL);

    __DSB();

    do {                                    // clean D-Cache
         uint32_t tmpways = ways;
         do {
              sw = ((tmpways << wshift) | (sets << sshift));
              SCB->DCCSW = sw;
            } while(tmpways--);
        } while(sets--);

    __DSB();
    __ISB();
  #endif
}


/** \brief Clean & Invalidate D-Cache

    The function cleans and Invalidates D-Cache
  */
__STATIC_INLINE void SCB_CleanInvalidateDCache (void)
{
  #if (__DCACHE_PRESENT == 1)
    uint32_t ccsidr, sshift, wshift, sw;
    uint32_t sets, ways;

    SCB->CSSELR = (0UL << 1) | 0UL;         // Level 1 data cache
    ccsidr  = SCB->CCSIDR;
    sets    = (uint32_t)(CCSIDR_SETS(ccsidr));
    sshift  = (uint32_t)(CCSIDR_LSSHIFT(ccsidr) + 4UL);
    ways    = (uint32_t)(CCSIDR_WAYS(ccsidr));
    wshift  = (uint32_t)((uint32_t)__CLZ(ways) & 0x1FUL);

    __DSB();

    do {                                    // clean & invalidate D-Cache
         uint32_t tmpways = ways;
         do {
              sw = ((tmpways << wshift) | (sets << sshift));
              SCB->DCCISW = sw;
            } while(tmpways--);
        } while(sets--);

    __DSB();
    __ISB();
  #endif
} 
