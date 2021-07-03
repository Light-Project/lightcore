/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  bootloader/x86/driver/floppy.c
 *  x86 pre floppy driver
 *  (C) 2020 wzl
 * 
 *  Change Logs:
 *  Date            Notes
 *  2021-02-26      Only 3.5 supported
 * 
 */

#include <init/initcall.h>
#include <system/irq.h>
#include <driver/platform.h>
#include <driver/block.h>
#include <driver/floppy/i82077.h>
#include <printk.h>
#include <mm.h>

#include <asm/io.h>
#include <asm/proc.h>

struct fdc_device {


};

static uint8_t fdc_inb(int fdc, int reg)
{


}

static void fdc_outb(int fdc, int reg, uint8_t val)
{


}

bool floppy_irq = false;

static irq_return_t floppy_handle(int nr, void *data)
{
    floppy_irq = true;
    return IRQ_RET_HANDLED;
}

#define irq_clean() floppy_irq = false

static state wait_irq()
{
    uint8_t timeout = 50;
    while((floppy_irq != true) && --timeout)
        // timer_msleep(1);
    if(timeout == 0)
        return -EIO;
    floppy_irq = false;
    return -ENOERR;   
}

/* Start the driver motor */
static inline void motor_on(uint8_t dev)
{
    uint8_t ready = dor_read() & FLOPPY_DOR_MOTOR_A << dev;
    if(ready)
        return;
    dor_mask(0, FLOPPY_DOR_MOTOR_A << dev);
    // msleep(300);
}

/* Stop the driver motor */
static inline void motor_off(uint8_t dev)
{
    dor_mask(FLOPPY_DOR_MOTOR_A << dev, 0);
}

static inline void floppy_select(uint8_t dev)
{
    dor_mask(FLOPPY_DOR_DSEL_MASK, dev & FLOPPY_DOR_DSEL_MASK);
}

/* Disable the floppy controller */
static void floppy_power_off()
{
    // Clear the reset bit (enter reset state) and clear 'enable IRQ and DMA'
    dor_mask(FLOPPY_DOR_IRQ | FLOPPY_DOR_RESET, 0);
}

/* Enable the floppy controller */
static void power_on()
{
    /* Clear the reset bit (enter reset state), but set 'enable IRQ and DMA' */
    dor_mask(FLOPPY_DOR_RESET, FLOPPY_DOR_IRQ);
    /* Real hardware needs a 4 microsecond delay */
    // usleep(4);
    /* Set the reset bit (normal operation) and keep 'enable IRQ and DMA' on */
    dor_mask(0, FLOPPY_DOR_RESET | FLOPPY_DOR_IRQ);
}


#define floppy_transmit_dma_read(dev, para) \
        floppy_transmit(FLOPPY_COMMAND_MF_BIT | FLOPPY_COMMAND_READ_DATA, dev, 8, 7, para, true, false, NULL, 0)

#define floppy_transmit_seek(dev, para)     \
        floppy_transmit(FLOPPY_COMMAND_SEEK, dev, 1, 7, para, false, false, NULL, 0)

#define floppy_transmit_id(para)            \
        floppy_transmit(FLOPPY_COMMAND_READ_ID, -1, 1, 7, para, false, false, NULL, 0)

#define floppy_transmit_version(para)       \
        floppy_transmit(FLOPPY_COMMAND_VERSION, -1, 0, 1, para, false, false, NULL, 0)

/**
 * Floppy disk operation integration function 
 * 
 * @param cmd: FloppyCommands
 * @param dev: floppy driver number
 * @param txsize: write parameter number
 * @param rxsize: read parameter number
 * @param para: read and write buffer
 * @param irqdma: wait for irq when set
 * @param piowrite: pio write mode when set
 * @param piobuffer: pio buffer
 * @param buffersize: pio buffer size
 * 
 * @return: MAIN_STATUS_REGISTER
 */
static uint8_t floppy_transmit(uint8_t cmd, int8_t dev, uint8_t txsize, uint8_t rxsize, uint8_t *para, bool irqdma, bool piowrite, uint8_t *piobuffer, uint32_t buffersize)
{
    uint8_t count, timeout, residue = 3;
    uint8_t *apt = NULL;
    state status = ERROR;
retry:
    timeout = 50;
    count = txsize;
    apt = para;


    if(dev != -1)
    {
        motor_on(dev);
        floppy_select(dev);
    }

    irq_clean();

    do{
        while((FLOPPY_MSR_RQM != (inb(FLOPPY_MAIN_STATUS_REGISTER) & (FLOPPY_MSR_RQM | FLOPPY_MSR_DIO))) && --timeout)
            // msleep(1);
        if(timeout == 0)
        {
            power_on();
            goto retry;                          // Error exit
        }
        if(txsize == count)
            outb(FLOPPY_DATA_FIFO, cmd);     // Send command byte to the FIFO port
        else
            outb(FLOPPY_DATA_FIFO, *apt++);  // then send the next parameter byte for the command to the FIFO port. 
    }while(count--);

    /* If using DMA on a read/write command, wait for a terminal IRQ6. */
    if(irqdma == true)
    {
        status = wait_irq();
        if(status == ERROR)
        {
            power_on();                         // Reset controller
            goto retry;                         // Error exit
        }
        goto out;
    }

    /* if NDMA not set, the command has no Execution phase, so skip to Result phase */
    if(!(inb(FLOPPY_MAIN_STATUS_REGISTER) & FLOPPY_MSR_NDMA))
        goto out;
    
    /* check execution phase para */
    if((piobuffer == NULL) | (buffersize == 0))
        goto exit;              // Error exit

    /* Execution phase */
    do{
        if((FLOPPY_MSR_RQM | FLOPPY_MSR_NDMA) != (inb(FLOPPY_MAIN_STATUS_REGISTER) & (FLOPPY_MSR_RQM | FLOPPY_MSR_NDMA)))
            break;
        /* Exchange information through fifo */
        if(piowrite == false)   // 
            *piobuffer++ = inb(FLOPPY_DATA_FIFO);
        else
            outb(FLOPPY_DATA_FIFO, *piobuffer++);
    }while(buffersize--);

    /* End phase */
out:
    timeout = 50;
    count = rxsize;
    apt = para;

    while(count--){
        while((FLOPPY_MSR_RQM | FLOPPY_MSR_DIO) != (inb(FLOPPY_MAIN_STATUS_REGISTER) & (FLOPPY_MSR_RQM | FLOPPY_MSR_DIO)) && --timeout)
            cpu_relax();
        if(timeout == 0)
        {
            power_on();
            goto retry;                          // Error exit
        }
        *apt++ = inb(FLOPPY_DATA_FIFO);
    }

    if(FLOPPY_MSR_RQM != (inb(FLOPPY_MAIN_STATUS_REGISTER) & 
    (FLOPPY_MSR_RQM | FLOPPY_MSR_DIO | FLOPPY_MSR_CB)) && residue--)
    {
        debug("    residue: %d\n\r", residue);
        goto retry;     // If an error occurs, try again three times
    }

exit:
    return inb(FLOPPY_MAIN_STATUS_REGISTER);
}

/**
 * Floppy disk DMA limit read
 * 
 * @param dev: floppy driver number
 * @param buffer: read buffer addr
 * @param lba: floppy lba addr
 * @param len: read length (It must be a multiple of 512)
 * 
 * @return: running state
 */
static state dma_read(uint8_t dev, uint32_t buffer, uint32_t lba , uint32_t len)
{
    uint8_t para[8];
    // state dma_ready;
    uint16_t cyl, head, sector;
    uint16_t limit, end;

    /* Make dma ready before transmission */
    // dma_ready = dma_floppy(buffer, len, 0);
    // if(dma_ready == ERROR)
    //     return ERROR;

    len /= 512;

    do{
        /* Converting LBA to CHS */
        lba2chs(lba, &cyl, &head, &sector);

        /* Calculate end of transfer sector */
        end = sector + len - 1;
        end = min((uint16_t)FLOPPY_144_SECTORS_PER_TRACK, end);
        limit = end - sector + 1;

        /* Write command starts transmission */
        para[0] = (head << 2) | dev;            // 1: head number << 2 | drive number
        para[1] = cyl;                          // 2: cylinder number 
        para[2] = head;                         // 3: head number
        para[3] = sector;                       // 4: starting sector number
        para[4] = FLOPPY_SIZE_CODE;             // 5: all floppy drives use 512bytes per sector
        para[5] = end;                          // 6: last sector number on the track
        para[6] = FLOPPY_GAPLEN;                // 7: GAP1 default size
        para[7] = FLOPPY_DATALEN;               // 8: all floppy drives use 512bytes per sector 
        floppy_transmit_dma_read(dev, para);

        // debug("0x%x\n\r", cyl);
        // debug("0x%x\n\r", head);
        // debug("0x%x\n\r", sector);
        // debug("0x%x\n\r", end);
        // debug("floppy dma return args1 0x%02x \n\r", para[0]);
        // debug("floppy dma return args2 0x%02x \n\r", para[1]);

        len -= limit;
        lba += limit;
    }while(len);

    return OK;
}

static enum Floppy_Version floppy_version(void)
{
    uint8_t para[1];
    floppy_transmit_version(para);
    switch(para[0])
    {
        case 0x90:
            return "82077AA";
        break;

        default:
            return "ERROR";
    }
}

static state floppy_read(uint8_t dev, uint32_t buffer, uint32_t lba , uint32_t len)
{
    uint32_t limit = 0;

    /* Check input para */
    if((buffer == 0)|(len == 0))
        return ERROR;

    /* Check the drive */

    /* 16 bit boundary detection, ready to read */
    do{
        limit = min(0x10000 - (buffer & 0xffff), len);
        dma_read(dev, buffer += limit, lba, limit);
        len -= limit;
    }while(len);
    
    return OK;
} 

static state i82077_open(struct block_device *bdev, fmode_t mode)
{
    return -ENOERR;   
}

static state i82077_release(struct block_device *bdev, fmode_t mode)
{
    
    return -ENOERR;   
}

static struct block_ops floppy_ops = {
    .open = i82077_open,
    .release = i82077_release,
};


/* Init one floppy controller */
static state fdc_init_one()
{
    struct block_device *bdev;

    bdev = kmalloc(sizeof(*bdev), GFP_KERNEL);
    bdev->ops = &floppy_ops;
    block_register(bdev);

    irq_alloc(0x26, 0, floppy_handle, NULL, "floppy");

    return -ENOERR;  
}

/* Init all floppy controller */
static state fdc_init()
{ 
    fdc_init_one();

}

static state floppy_probe(struct platform_device *pdev)
{
    
    return -ENOERR;
}

static state floppy_remove(struct platform_device *pdev)
{

    floppy_power_off();
    return -ENOERR;
}

static struct platform_driver floppy_driver = {
    .driver = {
        .name = "floppy-i82077",
    },
    .probe = floppy_probe,
    .remove = floppy_remove,
};

static state floppy_init(void)
{
    return platform_driver_register(&floppy_driver);
}

driver_initcall(floppy_init);
