
/**
 * mtd read-only memory
 */
#include <init/initcall.h>
#include <driver/platform.h>
#include <driver/mtd.h>

static map_rom_read(struct mtd_device *mdev, loff_t src, 
                uchar *buff, size_t len, size_t *rlen)
{
    *rlen = min(mdev->size, src + len);
    while(rlen -= 4) {
        *buff = readl();
        src += 4;
        buff += 4;
    }
}

static state mtd_ops map_rom_ops = {
    .read = map_rom_read,
    .read = map_rom_read,

};

static state map_rom_probe()
{


}

static struct map_rom_driver = {
    .driver = {
        .name = "mtd-rom",
    },
    .probe = ,

};

static state map_rom_init(void)
{

}

driver_initcall(map_rom_init);
