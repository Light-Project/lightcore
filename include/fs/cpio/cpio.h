
struct cpio_header {
    /* 0x00: cpio magic */
    uint8_t magic[6];
    /* 0x06: cpio magic */
    uint8_t ino[8];
    /* 0x0e: cpio magic */
    uint8_t mode[8];
    /* 0x16: cpio magic */
    uint8_t uid[8];
    /* 0x1e: cpio magic */
    uint8_t gid[8];
    /* 0x26: cpio magic */
    uint8_t nlink[8];
    /* 0x2e: cpio magic */
    uint8_t mtime[8];
    /* 0x36: cpio magic */
    uint8_t filesize[8];      /* must be 0 for FIFOs and directories */
    /* 0x3e: cpio magic */
    uint8_t devmajor[8];
    /* 0x46: cpio magic */
    uint8_t devminor[8];    
    /* 0x4e: cpio magic */
    uint8_t rdevmajor[8];   /* only valid for chr and blk special files */
    /* 0x56: cpio magic */
    uint8_t rdevminor[8];   /* only valid for chr and blk special files */
    /* 0x5e: cpio magic */
    uint8_t namesize[8];    /* count includes terminating NUL in pathname */
    /* 0x66: cpio magic */
    uint8_t check[8];       /* 0 for "new" portable format; for CRC format the sum of all the bytes in the file  */
} __packed cpio_header_t; 
