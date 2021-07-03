 

static struct spi_ops esp8266_spi_ops = {
    
    
    
};
 
static struct dt_device_id espressif_uart_dt_table[] =
{
    {
        .compatible = "espressif,esp8266-spi"
    },
    {},
};

static state esp8266_spi_probe(struct platform_device *dev)
{
    
}

static state esp8266_spi_remove(struct platform_device *dev)
{
    
}

static struct platform_driver esp8266_spi_driver = {
    .driver = {        
        .name = "espressif,esp8266-spi",
        .dt_table = espressif_uart_dt_table,
    },
    .prebe = esp8266_spi_probe,
    .remove = esp8266_spi_remove,
};
