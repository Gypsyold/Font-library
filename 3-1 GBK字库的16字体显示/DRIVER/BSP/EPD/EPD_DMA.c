#include "stm32f4xx.h"                  // Device header
#include "EPD_DMA.h"
#include "SPI_Init.h"

// DMA句柄（全局，方便中断和传输函数访问）
DMA_InitTypeDef DMA_InitStructure;

// 传输完成标志：0=进行中，1=完成
volatile uint8_t g_epd_dma_tx_done = 1;

void EPD_DMA_Init(void)
{
	//使能DMA1时钟 
	RCC_AHB1PeriphClockCmd(EPD_DMA_CLK, ENABLE);

	//复位DMA数据流（避免残留配置）
	DMA_DeInit(EPD_DMA_STREAM);
	while (DMA_GetCmdStatus(EPD_DMA_STREAM) != DISABLE); // 等待数据流复位完成


	DMA_InitStructure.DMA_Channel = EPD_DMA_CHANNEL;							// 通道0（SPI2_TX专属）
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI2->DR; 			// 外设地址：SPI2数据寄存器
	DMA_InitStructure.DMA_Memory0BaseAddr = 0;									// 内存地址：发送时动态指定
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral; 					// 方向：内存→外设（MCU→水墨屏）
	DMA_InitStructure.DMA_BufferSize = 0; 										// 传输长度：发送时动态指定
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 			// 外设地址不递增（固定SPI_DR）
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 					// 内存地址递增（连续发多字节）
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; 	// 外设8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 			// 内存8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; 								// 普通模式（传输完停止）
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; 						// 中等优先级
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable; 						// 禁用FIFO（直接传输，高速稳定）
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single; 				// 内存单次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; 		// 外设单次传输

	// 初始化DMA数据流
	DMA_Init(EPD_DMA_STREAM, &DMA_InitStructure);


	DMA_ITConfig(EPD_DMA_STREAM, DMA_IT_TC, ENABLE); // 关键！TC=Transfer Complete（传输完成）
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);


	//开启SPI2的DMA发送使能（关键：关联SPI和DMA）
	// 这是唯一需要操作SPI的地方，目的是让SPI触发DMA传输
	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);

	//初始禁用DMA（避免误触发
	DMA_Cmd(EPD_DMA_STREAM, DISABLE);
}



// 用DMA发送数组（适用于大块显存数据）
void EPD_SendData_DMA(const uint8_t *data, uint16_t len) 
{
  // 1. 确保DMA通道处于关闭状态
  DMA_Cmd(EPD_DMA_STREAM, DISABLE);

  // 2. 设置传输参数
  EPD_DMA_STREAM->M0AR = (uint32_t)data; // 内存地址：数组首地址
  EPD_DMA_STREAM->NDTR = len; // 传输长度：数组字节数

  // 3. 切换到数据模式（DC拉高），选中屏幕（CS拉低）
  EPD_DC_Set(); // 数据模式
  EPD_CS_Clr(); // 选中屏幕


  // 4. 清除之前的中断标志，启动DMA传输
  DMA_ClearFlag(EPD_DMA_STREAM,EPD_SPI_TX_DMA_FLAG); // 清除传输完成标志
  g_epd_dma_tx_done = 0;
  DMA_Cmd(EPD_DMA_STREAM, ENABLE); // 开始传输
}




void DMA1_Stream4_IRQHandler()
{
	 // 检查是否是传输完成中断
  if (DMA_GetITStatus(EPD_DMA_STREAM,EPD_SPI_TX_DMA_IT) != RESET) 
	{
		// 1. 清除中断标志
		DMA_ClearITPendingBit(EPD_DMA_STREAM,EPD_SPI_TX_DMA_IT);

		// 2. 关闭DMA通道（防止重复传输）
		DMA_Cmd(EPD_DMA_STREAM, DISABLE);

		// 3. 等待SPI硬件完成最后一字节发送（确保总线空闲）
		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET);

		// 4. 传输完成，拉高CS释放屏幕
		EPD_CS_Set();
		g_epd_dma_tx_done = 1;
	  }
}



