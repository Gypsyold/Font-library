#include "EPD.h"
#include "board.h"
#include "EPD_DMA.h"

/*******************************************************************
		函数说明:判忙函数
		入口参数:无
		说明:忙状态为1		
*******************************************************************/
void EPD_READBUSY(void)
{
	while(1)
	{
		if(EPD_ReadBusy==0)
		{
			break;
		}
	}
}

/*******************************************************************
		函数说明:硬件复位函数
		入口参数:无
		说明:在E-Paper进入Deepsleep状态后需要硬件复位	
*******************************************************************/
void EPD_HW_RESET(void)
{
	delay_ms(100);
	EPD_RES_Clr();
	delay_ms(10);
	EPD_RES_Set();
	delay_ms(10);
	EPD_READBUSY();
}

/*******************************************************************
		函数说明:更新函数
		入口参数:无	
		说明:更新显示内容到E-Paper		
*******************************************************************/
void EPD_Update(void)
{
	EPD_WR_REG(0x22);
	EPD_WR_DATA8(0xF4);
	EPD_WR_REG(0x20);
	EPD_READBUSY();
}
/*******************************************************************
		函数说明:局刷更新函数
		入口参数:无
		说明:E-Paper工作在局刷模式
*******************************************************************/
void EPD_PartUpdate(void)
{
	
	EPD_WR_REG(0x22);
	EPD_WR_DATA8(0x1C);
	EPD_WR_REG(0x20);
	EPD_READBUSY();
}
/*******************************************************************
		函数说明:快刷更新函数
		入口参数:无
		说明:E-Paper工作在快刷模式
*******************************************************************/
void EPD_FastUpdate(void)
{
	EPD_WR_REG(0x22);
	EPD_WR_DATA8(0xC7);
	EPD_WR_REG(0x20);
	EPD_READBUSY();
}

/*******************************************************************
		函数说明:休眠函数
		入口参数:无
		说明:屏幕进入低功耗模式		
*******************************************************************/
void EPD_DeepSleep(void)
{
	EPD_WR_REG(0x10);
	EPD_WR_DATA8(0x01);
	delay_ms(200);
}

/*******************************************************************
		函数说明:初始化函数
		入口参数:无
		说明:调整E-Paper默认显示方向
*******************************************************************/
void EPD_Init(void)
{
	EPD_HW_RESET();
	EPD_READBUSY();   
	EPD_WR_REG(0x12);  //SWRESET
	EPD_READBUSY();   
	// 初始化用于 SPI 发送的大块数据 DMA
	EPD_DMA_Init();
	
	EPD_WR_REG(0x3C); //BorderWavefrom
	EPD_WR_DATA8(0x05);
	
	EPD_WR_REG(0x01); //Driver output control      
	EPD_WR_DATA8((EPD_H-1)%256);    	//低8位
	EPD_WR_DATA8((EPD_H-1)/256);		//高8位
	EPD_WR_DATA8(0x00); 				//顶部→底部，左侧→右侧

	EPD_WR_REG(0x11); //data entry mode       //规定图像数据写入 RAM 时的地址计数器更新方向
	EPD_WR_DATA8(0x02);// 010	X 递减，Y 递增
	EPD_WR_REG(0x44); //set Ram-X address start/end position   
	EPD_WR_DATA8(EPD_W/8-1);    
	EPD_WR_DATA8(0x00);  
	EPD_WR_REG(0x45); //set Ram-Y address start/end position          
	EPD_WR_DATA8(0x00);
	EPD_WR_DATA8(0x00); 
	EPD_WR_DATA8((EPD_H-1)%256); 
	EPD_WR_DATA8((EPD_H-1)/256);
	EPD_WR_REG(0x21); //  Display update control
	EPD_WR_DATA8(0x00);		
	EPD_WR_DATA8(0x80);	
	EPD_WR_REG(0x18); //Read built-in temperature sensor
	EPD_WR_DATA8(0x80);	
	EPD_WR_REG(0x4E);   // set RAM x address count to 0;
	EPD_WR_DATA8(EPD_W/8-1);    
	EPD_WR_REG(0x4F);   // set RAM y address count to 0X199;    
	EPD_WR_DATA8(0x00);
	EPD_WR_DATA8(0x00);
  EPD_READBUSY();
}

/*******************************************************************
		函数说明:快刷初始化函数
		入口参数:无
		说明:E-Paper工作在快刷模式
*******************************************************************/
void EPD_FastInit(void)
{
	EPD_HW_RESET();
	EPD_READBUSY();
//  EPD_WR_REG(0x12);  //旋转180度显示 不能重新进行软复位IC
//	EPD_READBUSY();
	EPD_WR_REG(0x18);
	EPD_WR_DATA8(0x80);
	EPD_WR_REG(0x22);
	EPD_WR_DATA8(0xB1);
	EPD_WR_REG(0x20);
	EPD_READBUSY();
	EPD_WR_REG(0x1A);
	EPD_WR_DATA8(0x64);
	EPD_WR_DATA8(0x00);
	EPD_WR_REG(0x22);
	EPD_WR_DATA8(0x91);
	EPD_WR_REG(0x20);
	EPD_READBUSY();
}


/*******************************************************************
		函数说明:清屏函数
		入口参数:无
		说明:E-Paper刷白屏
*******************************************************************/
void EPD_Display_Clear(void)
{
	u16 i;
	EPD_WR_REG(0x3C);
	EPD_WR_DATA8(0x01);
	EPD_WR_REG(0x24);
	for(i=0;i<5624;i++)
	{
		EPD_WR_DATA8(0xFF);
	}	
	EPD_WR_REG(0x26);
	for(i=0;i<5624;i++)
	{
		EPD_WR_DATA8(0xFF);
	}	
}

/*******************************************************************
		函数说明:局刷擦除旧数据
		入口参数:无
		说明:E-Paper工作在局刷模式下调用
*******************************************************************/
void EPD_Clear_R26H(void)
{
	u16 i;
	EPD_READBUSY();
	EPD_WR_REG(0x26);
	for(i=0;i<5624;i++)
	{
		EPD_WR_DATA8(0xFF);
	}
}

/*******************************************************************
		函数说明:数组数据更新到E-Paper
		入口参数:无
		说明:
*******************************************************************/
//void EPD_Display(const u8 *image)
//{
//	u16 i,j,Width,Height;
//	Width=(EPD_W%8==0)?(EPD_W/8):(EPD_W/8+1);
//	Height=EPD_H;
//	EPD_WR_REG(0x24);
//	for (j=0;j<Height;j++) 
//	{
//		for (i=0;i<Width;i++) 
//		{
//			EPD_WR_DATA8(image[i+j*Width]);
//		}
//	}
//}

void EPD_Display(const u8 *image)
{
	u16 Width,Height;
	Width=(EPD_W%8==0)?(EPD_W/8):(EPD_W/8+1);
	Height=EPD_H;
	EPD_WR_REG(0x24);
	// 使用 DMA 一次性发送整幅图像数据（Width*Height 字节）
	EPD_SendData_DMA((const uint8_t*)image, (uint16_t)(Width*Height));
	// 等待 DMA 发送完成
	while (!g_epd_dma_tx_done) {;}
}



