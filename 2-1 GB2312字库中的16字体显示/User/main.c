#include "stm32f4xx.h"                  // Device header
#include "board.h"
#include "bsp_uart.h"
#include <stdio.h>
#include "EPD_GUI.h"
#include "spi_w25Q128_flash.h"
#include "GB2312_kaiti_16.h"






void Write_GBK_Font_To_W25Q128(void)
{


   printf("W25Q128芯片ID是%x\r\n",W25Q128_readID());

#if 0
	//测试
	printf("开始测试写入...\r\n");
	W25Q128_ContinuousWrite((uint8_t *)test,W25Q128_GBK_ADDR,sizeof(test));
	printf("字库测试写入完成 ！！\r\n");
	
#endif	
	
	
#if 1  // ========== 写入字库上半部分（4418个汉字，141376字节） ==========
    printf("开始写入GB2312字库16*16字体...\r\n");
    W25Q128_ContinuousWrite
	(
        (uint8_t *)m_char_bits,     // 写入起始地址：字库开头（0x000000）
        W25Q128_GBK_ADDR,           // 数据指针：数组第0个汉字（上半段起始）
        sizeof(m_char_bits)         // 写入长度
    );
    printf("字库写入完成！\r\n");
#endif


}


//校验显示
void Verify_GBK_Font(void)
{
    uint8_t read_buff[32];  // 读取缓冲区（32字节=1个汉字）


    //  校验字库
    W25Q128_read(read_buff, W25Q128_GBK_ADDR + 32 * 128, 32);
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
	bsp_spi_dma_tx_init(); 	//DMA初始化（用于传输大量字库）
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
	
	EPD_GPIOInit();	
	EPD_Init();
	Paint_NewImage(ImageBW,EPD_W,EPD_H,0,WHITE);		//创建画布
	Paint_Clear(WHITE);	
	EPD_Display_Clear();		
	EPD_ShowString(36,130,"windows",16,BLACK);
	EPD_ShowChinese(0,0,"中景园电子",16,BLACK);
	EPD_ShowChinese(0,16,"电子",24,BLACK);
	//从Flash中读取的
	EPD_ShowChineseString_flash(0,60,"雪碧的情人",FONT_16X16,BLACK);
	EPD_Display(ImageBW);
	EPD_Update();
	EPD_DeepSleep();
	

}

