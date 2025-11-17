/*
 * 开源！水墨屏GBK和GB2312字库写入和读出
 *
 * 使用到的电子器件：立创开发板STMF407VGT6、中景园黑白双色水墨屏模块
 * 声明：在嘉立创提供的库函数和中景园提供的示例显示代码上创写
 * 功能：可以显示GBK所有字，从外部Flash中读取，减少占用CPU资源
 * 地址：https://github.com/Gypsyold/Font-library
 * Date           Author      
 * 2025-11-17     雪碧的情人
 *  
 */

#include "stm32f4xx.h"                  // Device header
#include "board.h"
#include "bsp_uart.h"
#include <stdio.h>
#include "EPD_GUI.h"
#include "spi_w25Q128_flash.h"
#include "GBK_kaiti_24.h"





// Flash写入字库
void Write_GBK_Font_To_W25Q128(void)
{
   

   printf("W25Q128芯片ID是%x\r\n",W25Q128_readID());


	
#if 0  // ========== 第一次：写入GBK字库上半部分(12032个字 ，866304字节) ==========
    printf("开始写入GBK字库24X24字体上半部分...\r\n");
//	static uint8_t sram_buf[FIRST_PART_NUM * FONT_SIZE];  // 全局SRAM缓冲区（7980×72=574560字节）
//    memcpy(sram_buf, m_char_bits1, sizeof(m_char_bits1));  // 从Flash复制到SRAM
	
    W25Q128_ContinuousWrite
	(
        (uint8_t *)m_char_bits1,    	// 写入起始地址：字库开头（0x000000）
        W25Q128_GBK_ADDR,         	    // 数据指针：数组第0个汉字（上半段起始）
        sizeof(m_char_bits1)      		// 写入长度
    );
    printf("GBK字库24X24字体上半写入完成！\r\n");
#endif	
	
	

#if 1 // ========== 第二次：写入GBK字库下半部分(11908个字 ，857376字节) ==========
    printf("开始写入GBK字库24X24字体下半部分...\r\n");
    W25Q128_ContinuousWrite(
			(uint8_t *)m_char_bits2,
			W25Q128_GBK_ADDR + FIRST_PART_NUM * FONT_SIZE,
			sizeof(m_char_bits2)


    );
    printf("GBK字库24X24字体下半部分写入完成！\r\n");
 
#endif





}



//对比验证写入是否正确
void Verify_GBK_Font(void)
{
    uint8_t read_buff[72];  


    // 校验字库地址对于的字模
    W25Q128_read(read_buff, W25Q128_GBK_ADDR +  (1111)*72 , 72);
    for(uint8_t i=0; i<72; i++)
    {  
		printf("%d 实际0x%02X\r\n",i,read_buff[i]);
    }

}


//水墨屏背景数组
u8 ImageBW[5624];
int main(void)
{

	board_init();
	uart1_init(115200U);		// 串口初始化，在写入的时候可以查看写入信息
	bsp_spi_flash_init();		// 初始化flash的SPI
	bsp_spi_dma_tx_init();		// 初始化SPI传输的DMA
	printf("设备ID是%x\r\n",W25Q128_readID());
	

	
	/*擦除扇区,只需要调用一次*/
//	for(uint16_t i = 0;i<500;i++)
//	{
//	
//	W25Q128_erase_sector(i);
//	printf("擦除%d扇区\r\n",i);	
//	W25Q128_wait_busy(); 
//	delay_ms(10);
//	}
	
	delay_ms(500);

	/*写入flash*/
//	Write_GBK_Font_To_W25Q128();
	
	/*对比验证写入*/
//	Verify_GBK_Font();


	printf("完成\r\n");
	
	
	/*水墨屏显示*/
	EPD_GPIOInit();	
	EPD_Init();
	Paint_NewImage(ImageBW,EPD_W,EPD_H,0,WHITE);		//创建画布 
	Paint_Clear(WHITE);	
	EPD_Display_Clear();		
	EPD_ShowString(36,130,"windows",16,BLACK);
	EPD_ShowChinese(0,0,"中景园电子",16,BLACK);
	EPD_ShowChinese(0,16,"电子",24,BLACK);
	
	
	
	EPD_ShowChineseString_flash(0,60,(uint8_t *)"狤狣狧獲獳螺络麻蚟",FONT_24X24,BLACK);
	EPD_ShowChineseString_flash(0,84,(uint8_t *)"丂丠乆乊乑乕乗",FONT_24X24,BLACK);
	EPD_ShowChineseString_flash(0,108,(uint8_t *)"垚淼頔",FONT_24X24,BLACK);
	
	EPD_Display(ImageBW);
	EPD_Update();
	EPD_DeepSleep();


	while(1)
	{}

}

