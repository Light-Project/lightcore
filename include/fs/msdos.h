#ifndef _FS_MSDOS_H_
#define _FS_MSDOS_H_

#define MSDOS_MAGIC 0xaa55

struct msdos_entry {
    uint8_t flag;       /* 0x80 active partition */
    uint8_t head;       /* start head */
    uint8_t sec;        /* start sector */
    uint8_t cyl;        /* start cylinder */
    uint8_t type;       /* partition type */
    uint8_t end_head;   /* end head */
    uint8_t end_sec;    /* end sector */
    uint8_t end_cyl;    /* end cylinder */
    uint32_t lba;       /* start lba */
    uint32_t size;      /* partition size */
} __packed;

struct msdos_head {
    uint8_t mbr[446];
    struct msdos_entry dpt[4];
    uint16_t magic;
} __packed;

#endif /* _FS_MSDOS_H_ */
