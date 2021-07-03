#include <types.h>


typedef enum fatfs_media{
    
} fatfs_media_t;

typedef 

typedef enum fatfs_type{
    fatfs_type_12   = 12,
    fatfs_type_16   = 16,
    fatfs_type_32   = 32,
} fatfs_version_t;

typedef struct fatfs_bootsec_ext16{
    uint8_t 
    uint8_t 
    uint8_t 
    uint32_t 
} __packed fatfs_bootsec_ext16_t;

typedef struct fatfs_bootsec_ext32{
    /* 0x24: Number of sectors occupied by fat over 32m */
    uint32_t fat_sectors_32;
    /* 0x28: Extension flag */
    uint16_t ext_flag;
    /* 0x2A: Fat32 version */
    uint16_t version;
    /* 0x2C: The first cluster number of the root directory */
    uint32_t root_
    /* 0x43: Volume serial number */
    uint8_t volume_serial[11];
    /* 0x47: Volume label */
    uint8_t volume_label[11];
    /* 0x52: File system type */
    uint32_t fatfs_type;
    
} __packed fatfs_bootsec_ext32_t;

typedef struct fatfs_bootsec{
    /* 0X00: X86 Jump command */
    uint8_t jmp[3];
    /* 0X03: OEM information */
    uint8_t oem[8];
    /* 0X0B: Bytes per sector */
    uint16_t sectors_byte;
    /* 0X0D: Sectors Per Cluster */
    uint8_t cluster_sectors_num;
    /* 0X0E: Number of sectors reserved */
    uint16_t reserved_sector;
    /* 0X10: Number of fat */
    uint8_t fat_num;
    /* 
    * 0X11: Do not use (the number of root directories, 
    * FAT32 has broken this limit and is invalid, 
    * generally 0) 
    */
    uint16_t root_num;
    /* 0X13: The total number of sectors is less than 32m */
    uint16_t sectors_num_16;
    /* 0X15: Storage media */
    uint8_t media;
    /* 0X16: Number of sectors occupied by fat less than 32m */
    uint16_t fat_sectors_16;
    /* 0X18: Number of sectors per track */
    uint16_t track_sectors;
    /* 0x1A: Number of Head */
    uint16_t head_num;
    /* 0x1C: Hidden sector */
    uint32_t hide_sectors;
    /* 0x20: Total number of sectors over 32m */
    uint32_t hide_sectors_32;
    union{
        fatfs_bootsec_ext16_t ext16;
        fatfs_bootsec_ext32_t ext32;
    } 
} __packed fatfs_bootsec_t;


typedef enum fatfs_entry_attributes{
    file_readwrite  = 0x00,
    file_readonly   = 0x01,
    file_hidden     = 0x02,
    file_system     = 0x04,
    file_vollable   = 0x08,
    file_subdir     = 0x10,
    file_archive    = 0x20,
} fatfs_entry_attributes_t;

typedef struct fatfs_entry{
    /* 0x00: File name */
    uint8_t file_name[8];
    /* 0x08: File extension */
    uint8_t file_extname[3];
    /* 0x0B: file attributes */
    uint8_t attributes;
    /* 0x0C: Reserved */
    uint8_t reserved;
    /* 0x0D: 10 ms bit of creation time */
    uint8_t create_millisecs;
    /* 0x0E: Creation seconds */
    uint32_t create_seconds:5;
    /* 0x0E: Creation minutes */
    uint32_t create_minutes:6;
    /* 0x0E: Creation hours */
    uint32_t create_hours:5;
    /* 0x0E: Creation day */
    uint32_t create_day:5;
    /* 0x0E: Creation month */
    uint32_t create_month:4;
    /* 0x0E: Creation year */
    uint32_t create_year:7;
    /* 0x0E: Access day */
    uint32_t access_day:5;
    /* 0x0E: Access month */
    uint32_t access_month:4;
    /* 0x0E: Access year */
    uint32_t access_year:7;
    /* 0x14: Starting cluster high number */
    uint16_t cluster_high;
    /* 0x16: modify seconds */
    uint32_t modify_seconds:5;
    /* 0x16: modify minutes */
    uint32_t modify_minutes:6;
    /* 0x16: modify hours */
    uint32_t modify_hours:5;
    /* 0x16: modify day */
    uint32_t modify_day:5;
    /* 0x16: modify month */
    uint32_t modify_month:4;
    /* 0x16: modify year */
    uint32_t modify_year:7;
    /* 0x1A: Starting cluster low number */
    uint16_t cluster_low;
    /* 0x1C: File size */
    uint32_t file_size;
} __packed fatfs_entry_t;






