#include "SPI_Init.h"
#include "board.h"

void EPD_SPI_Configuration(void) 
{
    SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    // 使能SPI2和GPIOB时钟（顺序无关，确保都开启）
    RCC_AHB1PeriphClockCmd(EPD_SCL_GPIO_CLK | EPD_SDA_GPIO_CLK, ENABLE);
    RCC_APB1PeriphClockCmd(EPD_SCL_SPI_CLK, ENABLE);

	
    // 配置SPI2引脚复用功能（PB13=SPI2_SCK，PB15=SPI2_MOSI）
    GPIO_PinAFConfig(EPD_SCL_GPIO_PORT, GPIO_PinSource13, GPIO_AF_SPI2);
    GPIO_PinAFConfig(EPD_SDA_GPIO_PORT, GPIO_PinSource15, GPIO_AF_SPI2);

    // 配置SCK和MOSI为“复用推挽输出”，速度100MHz（和软件SPI一致）
    GPIO_InitStructure.GPIO_Pin = EPD_SCL_GPIO_PIN | EPD_SDA_GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  							// 上拉
    GPIO_Init(EPD_SCL_GPIO_PORT, &GPIO_InitStructure);

    // SPI2核心配置（关键修正）
    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;  				// 仅发送（正确，水墨屏无需接收）
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;              				// 主机模式（正确）
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;          				// 8位数据（正确，4线SPI模式）
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                 				// 修正：CPOL=0（空闲时低电平，和软件一致）
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;               				// 正确：CPHA=0（上升沿采样，和软件一致）
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                  				// 软件控制CS（正确，复用原逻辑）
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;         				// 高位在前（正确，和软件一致）
    SPI_InitStructure.SPI_CRCPolynomial = 7;                   				// 不使用CRC，随意设置（正确）
    SPI_Init(SPI2, &SPI_InitStructure);

    // 使能SPI2
    SPI_Cmd(SPI2, ENABLE);
}

void EPD_GPIOInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(EPD_RES_GPIO_CLK|EPD_DC_GPIO_CLK|EPD_CS_GPIO_CLK|EPD_BUSY_GPIO_CLK, ENABLE);
    
    // 配置RES、DC、CS为推挽输出（和软件SPI一致）
    GPIO_InitStructure.GPIO_Pin = EPD_RES_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(EPD_RES_GPIO_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = EPD_DC_GPIO_PIN;
    GPIO_Init(EPD_DC_GPIO_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = EPD_CS_GPIO_PIN;
    GPIO_Init(EPD_CS_GPIO_PORT, &GPIO_InitStructure);
    
    // 配置BUSY为上拉输入（和软件SPI一致）
    GPIO_InitStructure.GPIO_Pin = EPD_BUSY_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(EPD_BUSY_GPIO_PORT, &GPIO_InitStructure);
    
    
    EPD_CS_Set();		// CS高电平（释放屏幕）
    EPD_RES_Set();		// RES高电平 （复位前准备）
	EPD_DC_Set();		// DC高电平	（默认数据模式）
    
    // 关键：必须调用SPI配置函数，初始化硬件SPI（软件SPI不需要这一步）
    EPD_SPI_Configuration();
}

// 总线发送函数（无需修改，逻辑和软件SPI一致）
void EPD_WR_Bus(u8 dat)
{
    EPD_CS_Clr(); // 选中屏幕
	delay_us(5);
	
    // 等待发送缓冲区为空
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI2, dat);

    // 等待总线空闲（确保数据发送完成）
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET);
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	
	delay_us(5);
    EPD_CS_Set(); // 释放屏幕
}

// 命令/数据发送函数（无需修改，完全复用软件SPI逻辑）
void EPD_WR_REG(u8 reg)
{
    EPD_DC_Clr();
    EPD_WR_Bus(reg);
    EPD_DC_Set();
}

void EPD_WR_DATA8(u8 dat)
{
    EPD_DC_Set();

    EPD_WR_Bus(dat);
    EPD_DC_Set();  // 和软件SPI保持一致（虽然第二次是多余的，但保持完全一致）
}

