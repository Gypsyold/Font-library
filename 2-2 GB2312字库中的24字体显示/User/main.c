/*
 * 开源！水墨屏GBK和GB2312字库写入和读出
 *
 * 使用到的电子器件：立创开发板STMF407VGT6、中景园黑白双色水墨屏
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
#include "GB2312_kaiti_24.h"





// Flash写入字库
void Write_GBK_Font_To_W25Q128(void)
{


   printf("W25Q128芯片ID是%x\r\n",W25Q128_readID());
	
	
#if 1  // ========== 写入GB2312字库24X24字体（8836个汉字，636192字节） ==========
    printf("开始写入GB2312字库24X24字体...\r\n");
    W25Q128_ContinuousWrite
	(
        (uint8_t *)m_char_bits,     // 数据指针
        W25Q128_GBK_ADDR,           // 写入起始地址
        sizeof(m_char_bits)         // 写入长度
    );
    printf("GB2312字库24X24字体写入完成！\r\n");
#endif


}


//对比验证写入是否正确
void Verify_GBK_Font(void)
{
    uint8_t read_buff[72];  // 读取缓冲区（24X24字是72字节=1个汉字）


    // 校验字库地址对于的字模
    W25Q128_read(read_buff, W25Q128_GBK_ADDR +  (1000)*72 , 72);
    for(uint8_t i=0; i<72; i++)
    {  
		printf("%d 实际0x%02X\r\n",i,read_buff[i]);
    }

}


// 水墨屏背景数组
u8 ImageBW[5624];
int main(void)
{
	board_init();
	uart1_init(115200U);	// 串口初始化，在写入的时候可以查看写入信息
	bsp_spi_flash_init();	// 初始化SPI
	bsp_spi_dma_tx_init(); 	// 再初始化DMA
	printf("设备ID是%x\r\n",W25Q128_readID());
	
	
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
	//从Flash中读取的
	EPD_ShowChineseString_flash(0,60,"阳仰耀椰耶腋彝衢徼",FONT_24X24,BLACK);
	EPD_Display(ImageBW);
	EPD_Update();
	EPD_DeepSleep();
	

}

