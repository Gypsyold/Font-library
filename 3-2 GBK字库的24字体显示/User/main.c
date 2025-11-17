#include "stm32f4xx.h"                  // Device header
#include "board.h"
#include "bsp_uart.h"
#include <stdio.h>
#include "EPD_GUI.h"
#include "spi_w25Q128_flash.h"
#include "GBK_songti_24.h"

#include <string.h>




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
	
#if 0  // ========== 第一次：写入字库第一部分 ==========
    printf("开始写入GBK字库24X24字体第一部分...\r\n");
//	static uint8_t sram_buf[FIRST_PART_NUM * FONT_SIZE];  // 全局SRAM缓冲区（7980×72=574560字节）
//    memcpy(sram_buf, m_char_bits1, sizeof(m_char_bits1));  // 从Flash复制到SRAM
	
    W25Q128_ContinuousWrite
	(
        (uint8_t *)m_char_bits1,    	// 写入起始地址：字库开头（0x000000）
        W25Q128_GBK_ADDR,         	    // 数据指针：数组第0个汉字（上半段起始）
        sizeof(m_char_bits1)      		// 写入长度
    );
    printf("GBK字库24X24字体第一部分写入完成！\r\n");
#endif	
	
	

#if 0 // ========== 第二次：写入字库第二部分 ==========
    printf("开始写入GBK字库24X24字体第二部分...\r\n");
    W25Q128_ContinuousWrite(
			(uint8_t *)m_char_bits2,
			W25Q128_GBK_ADDR + FIRST_PART_NUM * FONT_SIZE,
			sizeof(m_char_bits2)


    );
    printf("GBK字库24X24字体第二部分写入完成！\r\n");
 
#endif

#if 1 // ========== 第三次：写入字库第二部分 ==========
    printf("开始写入GBK字库24X24字体第三部分...\r\n");
    W25Q128_ContinuousWrite(
			(uint8_t *)m_char_bits3,
			W25Q128_GBK_ADDR + (FIRST_PART_NUM * FONT_SIZE) + (NEXT_PART_NUM * FONT_SIZE),
			(FONT_CHAR_COUNT - FIRST_PART_NUM - NEXT_PART_NUM)* FONT_SIZE


    );
    printf("GBK字库24X24字体第三部分写入完成！\r\n");
 
#endif
 



}


void Verify_GBK_Font(void)
{
    uint8_t read_buff[32];  // 读取缓冲区（32字节=1个汉字）


    // 1. 校验字库开头（地址0x000000，对应m_char_bits[0]）
    W25Q128_read(read_buff, W25Q128_GBK_ADDR +  (1111)*72 , 72);
    for(uint8_t i=0; i<72; i++)
    {  
		printf("%d 实际0x%02X\r\n",i,read_buff[i]);
    }

}


u8 ImageBW[5624];
int main(void)
{

	board_init();
	uart1_init(115200U);
	bsp_spi_flash_init();
	bsp_spi_dma_tx_init();
	printf("设备ID是%x\r\n",W25Q128_readID());
	

	
	//擦除扇区
//	for(uint16_t i = 0;i<500;i++)
//	{
//	
//	W25Q128_erase_sector(i);
//	printf("擦除%d扇区\r\n",i);	
//	W25Q128_wait_busy(); 
//	delay_ms(10);
//	}
//	
//	delay_ms(500);

//	//写入flash
//	Write_GBK_Font_To_W25Q128();
	
	//验证
//	Verify_GBK_Font();


	printf("完成\r\n");
	
	EPD_GPIOInit();	
	EPD_Init();
	Paint_NewImage(ImageBW,EPD_W,EPD_H,0,WHITE);		//创建画布 丂丠乆乊乑乕乗
	Paint_Clear(WHITE);	
	EPD_Display_Clear();		
	EPD_ShowString(36,130,"windows",16,BLACK);
	EPD_ShowChinese(0,0,"中景园电子",16,BLACK);
	EPD_ShowChinese(0,16,"电子",24,BLACK);
	
	EPD_ShowChineseString_flash(0,60,"狤狣狧獲獳螺络妈麻蚟",FONT_24X24,BLACK);//狤狣狧獲獳螺络妈麻蚟
	EPD_ShowChineseString_flash(0,84,"丂丠乆乊乑乕乗",FONT_24X24,BLACK);
	EPD_ShowChineseString_flash(0,108,"韩淼頔燚靳梦垚",FONT_24X24,BLACK);
	EPD_Display(ImageBW);
	EPD_Update();
	EPD_DeepSleep();


}

