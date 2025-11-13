#ifndef _EPD_DMA_H_
#define _EPD_DMA_H_
#include "stdint.h"


// SPI2_TX DMA 配置宏定义（STM32F407固定对应关系）
#define EPD_DMA_CHANNEL       DMA_Channel_0       // SPI2_TX对应DMA通道0
#define EPD_DMA_STREAM        DMA1_Stream4        // SPI2_TX对应DMA1数据流4
#define EPD_DMA_CLK           RCC_AHB1Periph_DMA1 // DMA1时钟（AHB1总线）
#define EPD_SPI_TX_DMA_FLAG   DMA_FLAG_TCIF4      // DMA1_Stream4传输完成标志
#define EPD_SPI_TX_DMA_IT     DMA_IT_TCIF4        // 中断类型：传输完成


void EPD_DMA_Init(void);
void EPD_SendData_DMA(const uint8_t *data, uint16_t len);


// 传输完成标志（由中断置位），0=进行中，1=完成
extern volatile uint8_t g_epd_dma_tx_done;

#endif

