#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include "mkespboot.h"

int chipv = 0;

enum chip_version
{
    esp8266 = 0,
    esp32   = 1,
};

static void error(const char * str, ...)
{
    va_list args;
    va_start(args, str);
    vfprintf(stderr, str, args);
    va_end(args);
    fputc('\n', stderr);
    exit(0);
}

void help()
{
    printf("Usage: mkespimage [option] file\n");
}

static uint8_t check_sum(const uint8_t *s, int len, uint8_t crc)
{
    while (len--)
        crc ^= *s++;
    return crc;
}

int esp8266_fiximage(char *path)
{
    ESP_head_t *buffer = NULL;
    FILE *iamge = NULL;
    uint32_t image_size = 0;
    uint8_t check = 0;
    
    iamge = fopen(path, "rb");
    if(iamge == NULL)
        error("can not open file");
    
    fseek(iamge, SEEK_SET, SEEK_END);  
    image_size = ftell(iamge);
    
    buffer = malloc(image_size);
    if(buffer == NULL)
        error("insufficient memory");

    fseek(iamge, 0, SEEK_SET);
    fread(buffer, image_size, 1, iamge);
    fclose(iamge);

    ESP_check_t *ESP_check = &buffer->seg + buffer->size;
    
    if(buffer->segment_count == 0)
        error("No segments found");
    
    check = check_sum(&buffer->seg, buffer->size, 0xef);
    
    ESP_check->checksum = check;

    iamge = fopen(path, "wb+");    
    if(iamge == NULL)
        error("can not open file");
    fwrite(buffer, image_size, 1, iamge);
    fflush(iamge);
    fclose(iamge);
    
    free(buffer);
    
    return 0; 
}

void esp8266_info(char *path)
{
    ESP_head_t *buffer = NULL;
    FILE *iamge = NULL;
    uint32_t image_size = 0;
    
    iamge = fopen(path, "rb");
    if(iamge == NULL)
        error("can not open file");
    
    fseek(iamge, SEEK_SET, SEEK_END);  
    image_size = ftell(iamge);
    
    buffer = malloc(image_size);
    if(buffer == NULL)
        error("insufficient memory");

    fseek(iamge, 0, SEEK_SET);
    fread(buffer, image_size, 1, iamge);
    fclose(iamge);
    
    ESP_check_t *check = &buffer->seg + buffer->size;
    
    printf("--------------------------\n");
    printf("Magic: 0x%x\n", buffer->magic);
    printf("Segments Num: 0x%x\n", buffer->segment_count);
    printf("User flag0: 0x%x\n", buffer->flags0);
    printf("User flag1: 0x%x\n", buffer->flags1);
    printf("Entry point: 0x%x\n", buffer->entry);
    printf("Checksum: 0x%x\n", check->checksum); 
    printf("--------------------------\n");
    
    free(buffer);
}


int main(int argc, char ** argv)
{
    char *para = NULL;
   
    while(--argc)
    {
        para = *++argv;
        if(*para == '-' && para[1])
        switch (*++para)
        {
            case 'c':
                if(!strcmp("esp8266", argv[1]))
                    chipv = esp8266;
                else if(!strcmp("esp32", argv[1]))
                    chipv = esp32;
                else
                    error("Unknown chip");
                printf("Chip version: %s\n", argv[1]);
            break;
            
            case 'f':
                if()
                    esp8266_fiximage(argv[1]);
            break;
            
            case 'i':
                info(argv[1]);
            break;
            
            case 'h':
            default:
                help();
            break;
        }
        
    }
   
}
