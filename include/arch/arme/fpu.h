/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  include/arch/arme/fpu.h
 *  arme fpu define
 *  (C) 2020 arm
 * 
 *  Change Logs:
 *  Date            Notes
 *  2021-02-26      first version 
 * 
 */

#ifndef _ARCH_ARME_FPU_H_
#define _ARCH_ARME_FPU_H_

#include <types.h>
#include <arch/arme/map.h>

struct fpu_type{
    uint32_t RESERVED0[1];
    uint32_t FPCCR;                   /*!< Offset: 0x004 (R/W)  Floating-Point Context Control Register               */
    uint32_t FPCAR;                   /*!< Offset: 0x008 (R/W)  Floating-Point Context Address Register               */
    uint32_t FPDSCR;                  /*!< Offset: 0x00C (R/W)  Floating-Point Default Status Control Register        */
    uint32_t MVFR0;                   /*!< Offset: 0x010 (R/ )  Media and FP Feature Register 0                       */
    uint32_t MVFR1;                   /*!< Offset: 0x014 (R/ )  Media and FP Feature Register 1                       */
    uint32_t MVFR2;                   /*!< Offset: 0x018 (R/ )  Media and FP Feature Register 2                       */
};

#define FPU     ((FPU_Type *)FPU_BASE)

/* Floating-Point Context Control Register */
#define FPU_FPCCR_ASPEN_Pos                31                                             /*!< FPCCR: ASPEN bit Position */
#define FPU_FPCCR_ASPEN_Msk                (1UL << FPU_FPCCR_ASPEN_Pos)                   /*!< FPCCR: ASPEN bit Mask */

#define FPU_FPCCR_LSPEN_Pos                30                                             /*!< FPCCR: LSPEN Position */
#define FPU_FPCCR_LSPEN_Msk                (1UL << FPU_FPCCR_LSPEN_Pos)                   /*!< FPCCR: LSPEN bit Mask */

#define FPU_FPCCR_MONRDY_Pos                8                                             /*!< FPCCR: MONRDY Position */
#define FPU_FPCCR_MONRDY_Msk               (1UL << FPU_FPCCR_MONRDY_Pos)                  /*!< FPCCR: MONRDY bit Mask */

#define FPU_FPCCR_BFRDY_Pos                 6                                             /*!< FPCCR: BFRDY Position */
#define FPU_FPCCR_BFRDY_Msk                (1UL << FPU_FPCCR_BFRDY_Pos)                   /*!< FPCCR: BFRDY bit Mask */

#define FPU_FPCCR_MMRDY_Pos                 5                                             /*!< FPCCR: MMRDY Position */
#define FPU_FPCCR_MMRDY_Msk                (1UL << FPU_FPCCR_MMRDY_Pos)                   /*!< FPCCR: MMRDY bit Mask */

#define FPU_FPCCR_HFRDY_Pos                 4                                             /*!< FPCCR: HFRDY Position */
#define FPU_FPCCR_HFRDY_Msk                (1UL << FPU_FPCCR_HFRDY_Pos)                   /*!< FPCCR: HFRDY bit Mask */

#define FPU_FPCCR_THREAD_Pos                3                                             /*!< FPCCR: processor mode bit Position */
#define FPU_FPCCR_THREAD_Msk               (1UL << FPU_FPCCR_THREAD_Pos)                  /*!< FPCCR: processor mode active bit Mask */

#define FPU_FPCCR_USER_Pos                  1                                             /*!< FPCCR: privilege level bit Position */
#define FPU_FPCCR_USER_Msk                 (1UL << FPU_FPCCR_USER_Pos)                    /*!< FPCCR: privilege level bit Mask */

#define FPU_FPCCR_LSPACT_Pos                0                                             /*!< FPCCR: Lazy state preservation active bit Position */
#define FPU_FPCCR_LSPACT_Msk               (1UL /*<< FPU_FPCCR_LSPACT_Pos*/)              /*!< FPCCR: Lazy state preservation active bit Mask */

/* Floating-Point Context Address Register */
#define FPU_FPCAR_ADDRESS_Pos               3                                             /*!< FPCAR: ADDRESS bit Position */
#define FPU_FPCAR_ADDRESS_Msk              (0x1FFFFFFFUL << FPU_FPCAR_ADDRESS_Pos)        /*!< FPCAR: ADDRESS bit Mask */

/* Floating-Point Default Status Control Register */
#define FPU_FPDSCR_AHP_Pos                 26                                             /*!< FPDSCR: AHP bit Position */
#define FPU_FPDSCR_AHP_Msk                 (1UL << FPU_FPDSCR_AHP_Pos)                    /*!< FPDSCR: AHP bit Mask */

#define FPU_FPDSCR_DN_Pos                  25                                             /*!< FPDSCR: DN bit Position */
#define FPU_FPDSCR_DN_Msk                  (1UL << FPU_FPDSCR_DN_Pos)                     /*!< FPDSCR: DN bit Mask */

#define FPU_FPDSCR_FZ_Pos                  24                                             /*!< FPDSCR: FZ bit Position */
#define FPU_FPDSCR_FZ_Msk                  (1UL << FPU_FPDSCR_FZ_Pos)                     /*!< FPDSCR: FZ bit Mask */

#define FPU_FPDSCR_RMode_Pos               22                                             /*!< FPDSCR: RMode bit Position */
#define FPU_FPDSCR_RMode_Msk               (3UL << FPU_FPDSCR_RMode_Pos)                  /*!< FPDSCR: RMode bit Mask */

/* Media and FP Feature Register 0 */
#define FPU_MVFR0_FP_rounding_modes_Pos    28                                             /*!< MVFR0: FP rounding modes bits Position */
#define FPU_MVFR0_FP_rounding_modes_Msk    (0xFUL << FPU_MVFR0_FP_rounding_modes_Pos)     /*!< MVFR0: FP rounding modes bits Mask */

#define FPU_MVFR0_Short_vectors_Pos        24                                             /*!< MVFR0: Short vectors bits Position */
#define FPU_MVFR0_Short_vectors_Msk        (0xFUL << FPU_MVFR0_Short_vectors_Pos)         /*!< MVFR0: Short vectors bits Mask */

#define FPU_MVFR0_Square_root_Pos          20                                             /*!< MVFR0: Square root bits Position */
#define FPU_MVFR0_Square_root_Msk          (0xFUL << FPU_MVFR0_Square_root_Pos)           /*!< MVFR0: Square root bits Mask */

#define FPU_MVFR0_Divide_Pos               16                                             /*!< MVFR0: Divide bits Position */
#define FPU_MVFR0_Divide_Msk               (0xFUL << FPU_MVFR0_Divide_Pos)                /*!< MVFR0: Divide bits Mask */

#define FPU_MVFR0_FP_excep_trapping_Pos    12                                             /*!< MVFR0: FP exception trapping bits Position */
#define FPU_MVFR0_FP_excep_trapping_Msk    (0xFUL << FPU_MVFR0_FP_excep_trapping_Pos)     /*!< MVFR0: FP exception trapping bits Mask */

#define FPU_MVFR0_Double_precision_Pos      8                                             /*!< MVFR0: Double-precision bits Position */
#define FPU_MVFR0_Double_precision_Msk     (0xFUL << FPU_MVFR0_Double_precision_Pos)      /*!< MVFR0: Double-precision bits Mask */

#define FPU_MVFR0_Single_precision_Pos      4                                             /*!< MVFR0: Single-precision bits Position */
#define FPU_MVFR0_Single_precision_Msk     (0xFUL << FPU_MVFR0_Single_precision_Pos)      /*!< MVFR0: Single-precision bits Mask */

#define FPU_MVFR0_A_SIMD_registers_Pos      0                                             /*!< MVFR0: A_SIMD registers bits Position */
#define FPU_MVFR0_A_SIMD_registers_Msk     (0xFUL /*<< FPU_MVFR0_A_SIMD_registers_Pos*/)  /*!< MVFR0: A_SIMD registers bits Mask */

/* Media and FP Feature Register 1 */
#define FPU_MVFR1_FP_fused_MAC_Pos         28                                             /*!< MVFR1: FP fused MAC bits Position */
#define FPU_MVFR1_FP_fused_MAC_Msk         (0xFUL << FPU_MVFR1_FP_fused_MAC_Pos)          /*!< MVFR1: FP fused MAC bits Mask */

#define FPU_MVFR1_FP_HPFP_Pos              24                                             /*!< MVFR1: FP HPFP bits Position */
#define FPU_MVFR1_FP_HPFP_Msk              (0xFUL << FPU_MVFR1_FP_HPFP_Pos)               /*!< MVFR1: FP HPFP bits Mask */

#define FPU_MVFR1_D_NaN_mode_Pos            4                                             /*!< MVFR1: D_NaN mode bits Position */
#define FPU_MVFR1_D_NaN_mode_Msk           (0xFUL << FPU_MVFR1_D_NaN_mode_Pos)            /*!< MVFR1: D_NaN mode bits Mask */

#define FPU_MVFR1_FtZ_mode_Pos              0                                             /*!< MVFR1: FtZ mode bits Position */
#define FPU_MVFR1_FtZ_mode_Msk             (0xFUL /*<< FPU_MVFR1_FtZ_mode_Pos*/)          /*!< MVFR1: FtZ mode bits Mask */
 
#endif /* _ARCH_ARME_FPU_H_ */
