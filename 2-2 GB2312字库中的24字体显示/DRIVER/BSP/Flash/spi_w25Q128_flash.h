/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 立创论坛：https://oshwhub.com/forum
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 * Change Logs:
 * Date           Author       Notes
 * 2024-08-02     LCKFB-LP    first version
 */
#ifndef __SPI__W25Q128_FLASH_H__
#define __SPI__W25Q128_FLASH_H__

#include "stm32f4xx.h"

// W25Q128关键参数（4KB扇区+256字节页，根据你的芯片固定）
#define W25Q128_SECTOR_SIZE  4096
#define W25Q128_PAGE_SIZE    256
#define W25Q128_PAGE_MASK    (W25Q128_PAGE_SIZE - 1)  // 0xFF，用于计算页剩余字节



#define W25Q128_GBK_ADDR  		0x000000          	// 字库在W25Q128的起始地址（扇区对齐）

#define GBK_CHAR_NUM      8836      // 汉字总个数
#define GBK_CHAR_BYTES    32        // 单个汉字字节数（16×16点阵）
#define GBK_FONT_TOTAL    (GBK_CHAR_NUM * GBK_CHAR_BYTES)  // 总长度：8836×32=282752字节


#define GBK_FONT_PART1_LEN  (GBK_FONT_TOTAL / 2)  // 第一次写入长度：141376字节
#define GBK_FONT_PART2_LEN  (GBK_FONT_TOTAL - GBK_FONT_PART1_LEN)  // 第二次写入长度：141376字节
#define GBK_PART1_CHAR_NUM  (GBK_FONT_PART1_LEN / GBK_CHAR_BYTES)  // 第一次写入汉字个数：4418个



#define BSP_GPIO_RCU                  RCC_AHB1Periph_GPIOA // GPIO时钟
#define BSP_SPI_RCU                   RCC_APB2Periph_SPI1  // SPI时钟
#define BSP_SPI_NSS_RCU               RCC_AHB1Periph_GPIOA // CS引脚时钟

#define BSP_GPIO_PORT                 GPIOA
#define BSP_GPIO_AF                   GPIO_AF_SPI1

#define BSP_SPI                       SPI1

#define BSP_SPI_NSS                   GPIO_Pin_4        // 软件CS
        
#define BSP_SPI_SCK                   GPIO_Pin_5
#define BSP_SPI_SCK_PINSOURCE         GPIO_PinSource5

#define BSP_SPI_MISO                  GPIO_Pin_6
#define BSP_SPI_MISO_PINSOURCE        GPIO_PinSource6

#define BSP_SPI_MOSI                  GPIO_Pin_7
#define BSP_SPI_MOSI_PINSOURCE        GPIO_PinSource7


#define W25QXX_CS_ON(x)               GPIO_WriteBit(BSP_GPIO_PORT, BSP_SPI_NSS, x ? Bit_SET : Bit_RESET)

// DMA相关宏定义
#define SPI_DMA_TX_STREAM    DMA2_Stream3
#define SPI_DMA_TX_CHANNEL   DMA_Channel_3
#define SPI_DMA_TX_IRQn      DMA2_Stream3_IRQn
#define SPI_DMA_TX_IRQHandler DMA2_Stream3_IRQHandler
#define DMA_TX_BUFFER_SIZE   64  // 单次最大DMA传输长度（实验的64更稳定）
#define W25Q128_SECTOR_SIZE  4096
#define W25Q128_PAGE_SIZE    256



void bsp_spi_flash_init(void);
uint8_t spi_read_write_byte(uint8_t dat);
uint16_t W25Q128_readID(void);
void W25Q128_write_enable(void);
void W25Q128_wait_busy(void);
void W25Q128_erase_sector(uint32_t addr);
void W25Q128_write(uint8_t* buffer, uint32_t addr, uint16_t numbyte);
void W25Q128_read(uint8_t* buffer,uint32_t read_addr,uint16_t read_length);
void W25Q128_ContinuousWrite(uint8_t* buffer, uint32_t addr, uint32_t numbyte);
void bsp_spi_dma_tx_init(void);



#endif
