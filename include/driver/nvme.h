/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_NVME_H_
#define _DRIVER_NVME_H_

enum nvme_registers {
    NVME_CAP        = 0x0000,   /* Controller Capabilities */
    NVME_VS         = 0x0008,   /* Version */
    NVME_INTMS      = 0x000c,   /* Interrupt Mask Set */
    NVME_INTMC      = 0x0010,   /* Interrupt Mask Clear */
    NVME_CC         = 0x0014,   /* Controller Configuration */
    NVME_CSTS       = 0x001c,   /* Controller Status */
    NVME_NSSR       = 0x0020,   /* NVM Subsystem Reset */
    NVME_AQA        = 0x0024,   /* Admin Queue Attributes */
    NVME_ASQ        = 0x0028,   /* Admin SQ Base Address */
    NVME_ACQ        = 0x0030,   /* Admin CQ Base Address */
    NVME_CMBLOC     = 0x0038,   /* Controller Memory Buffer Location */
    NVME_CMBSZ      = 0x003c,   /* Controller Memory Buffer Size */
    NVME_BPINFO     = 0x0040,   /* Boot Partition Information */
    NVME_BPRSEL     = 0x0044,   /* Boot Partition Read Select */
    NVME_BPMBL      = 0x0048,   /* Boot Partition Memory Buffer Location */
    NVME_CMBMSC     = 0x0050,   /* Controller Memory Buffer Memory Space Control */
    NVME_PMRCAP     = 0x0e00,   /* Persistent Memory Capabilities */
    NVME_PMRCTL     = 0x0e04,   /* Persistent Memory Region Control */
    NVME_PMRSTS     = 0x0e08,   /* Persistent Memory Region Status */
    NVME_PMREBS     = 0x0e0c,   /* Persistent Memory Region Elasticity Buffer Size */
    NVME_PMRSWTP    = 0x0e10,   /* Persistent Memory Region Sustained Write Throughput */
    NVME_DBS        = 0x1000,   /* SQ 0 Tail Doorbell */
};

struct nvme_cqe {
    uint32_t cdw0;
    uint32_t res;
    uint16_t sq_head;
    uint16_t sq_id;
    uint16_t cid;
    uint16_t status;
} __packed;

struct nvme_sqe {
    uint32_t cdw0;
    uint32_t nsid;
    uint64_t res;
    uint64_t mptr;
    uint64_t dptr_prp1;
    uint64_t dptr_prp2;
};

/****************************************************************************************/
/*      Mnemonic                value                 meaning/usage                     */

#define NVME_CAP_CSS_NVME       BIT_ULL(37)

#define NVME_CSTS_FATAL         BIT(1)
#define NVME_CSTS_RDY           BIT(0)

#define NVME_CC_ENABLE          BIT(0)

#endif /* _DRIVER_NVME_H_ */
