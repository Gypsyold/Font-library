#include "stm32f4xx.h"                  // Device header
#include "board.h"
#include "bsp_uart.h"
#include <stdio.h>
#include "EPD_GUI.h"
#include "spi_w25Q128_flash.h"
#include "GBK_kaiti_16.h"





// 写字库函数
void Write_GBK_Font_To_W25Q128(void)
{
   

   printf("W25Q128芯片ID是%x\r\n",W25Q128_readID());

#if 0
	//测试
	printf("开始测试写入...\r\n");
	W25Q128_ContinuousWrite((uint8_t *)test,W25Q128_GBK_ADDR,sizeof(test));
	printf("字库测试写入完成 ！！\r\n");
	
#endif	
	
#if 1  // ========== 第一次：写入字库上半部分（4418个汉字，141376字节） ==========
    printf("开始写入GBK字库16X16字体...\r\n");
    W25Q128_ContinuousWrite
	(
        (uint8_t *)m_char_bits,    		// 写入起始地址：字库开头（0x000000）
        W25Q128_GBK_ADDR,         	    // 数据指针：数组第0个汉字（上半段起始）
        sizeof(m_char_bits)      		// 写入长度
    );
    printf("GBK字库16X16字体写入完成！\r\n");
#endif	
	
	
	
#if 0  // ========== 第一次：写入字库上半部分（4418个汉字，141376字节） ==========
    printf("开始GBK字库16X16字体第一部分...\r\n");
    W25Q128_ContinuousWrite
	(
        (uint8_t *)m_char_bits,    		// 写入起始地址：字库开头（0x000000）
        W25Q128_GBK_ADDR,         	    // 数据指针：数组第0个汉字（上半段起始）
        First_Part_Num * FONT_SIZE      // 写入长度
    );
    printf("GBK字库16X16字体第一部分写入完成！\r\n");
#endif

#if 0 // ========== 第二次：写入字库下半部分（4418个汉字，141376字节） ==========
    printf("开始写入字库第二部分...\r\n");
    W25Q128_ContinuousWrite(
			(uint8_t *)(m_char_bits + First_Part_Num),
			W25Q128_GBK_ADDR + First_Part_Num * FONT_SIZE,
			Next_Part_Num * FONT_SIZE


    );
    printf("字库第二写入完成！\r\n");
 
#endif

#if 0 // ========== 第三次：写入字库下半部分（4418个汉字，141376字节） ==========
    printf("开始写入字库第三部分...\r\n");
    W25Q128_ContinuousWrite
	(
		(uint8_t *)(m_char_bits + First_Part_Num + Next_Part_Num),
		W25Q128_GBK_ADDR + ((First_Part_Num + Next_Part_Num)* FONT_SIZE),
		(FONT_CHAR_COUNT - First_Part_Num - Next_Part_Num) * FONT_SIZE


    );
    printf("字库第三部分写入完成！\r\n");
 
#endif



}


void Verify_GBK_Font(void)
{
    uint8_t read_buff[32];  // 读取缓冲区（32字节=1个汉字）


    // 1. 校验字库开头（地址0x000000，对应m_char_bits[0]）
    W25Q128_read(read_buff, W25Q128_GBK_ADDR +  (15875 )*32 , 32);
    for(uint8_t i=0; i<32; i++)
    {  
		printf("实际0x%02X\r\n",read_buff[i]);
    }

}


u8 ImageBW[5624];
int main(void)
{

	board_init();
	uart1_init(115200U);
	bsp_spi_flash_init();
	bsp_spi_dma_tx_init(); // 再初始化DMA
	printf("设备ID是%x\r\n",W25Q128_readID());
	
	//Write_GBK_Font_To_W25Q128();
	//Verify_GBK_Font();
//	for(uint8_t i = 0;i<100;i++)
//	{
//	
//	W25Q128_erase_sector(i);
//	printf("擦除%d扇区\r\n",i);		
//	}
	


	printf("完成\r\n");
//	
	EPD_GPIOInit();	
	EPD_Init();
	Paint_NewImage(ImageBW,EPD_W,EPD_H,0,WHITE);		//创建画布
	Paint_Clear(WHITE);	
	EPD_Display_Clear();		
	EPD_ShowString(36,130,"windows",16,BLACK);
	EPD_ShowChinese(0,0,"中景园电子",16,BLACK);
	EPD_ShowChinese(0,16,"电子",24,BLACK);
	
	EPD_ShowChineseString_flash(0,60,"垚",FONT_16X16,BLACK);
	EPD_Display(ImageBW);
	EPD_Update();
	EPD_DeepSleep();
//	

}

