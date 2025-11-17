/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 立创论坛：https://oshwhub.com/forum
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 * Change Logs:
 * Date           Author       	Notes
 * 2024-08-02     LCKFB-LP    	first version
 * 2025-11-17     雪碧的情人		next version
 */
#include "spi_w25Q128_flash.h"
#include "board.h"
#include <stddef.h>
#include <stdio.h>
/**********************************************************
 * 函 数 名 称：bsp_spi_init
 * 函 数 功 能：初始化SPI
 * 传 入 参 数：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
**********************************************************/
void bsp_spi_flash_init(void)
{
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 使能 GPIO 时钟 */
    RCC_AHB1PeriphClockCmd (BSP_GPIO_RCU, ENABLE);

    /* SPI时钟使能 */
    RCC_APB2PeriphClockCmd(BSP_SPI_RCU, ENABLE);

    /* 设置引脚复用 */
    GPIO_PinAFConfig(BSP_GPIO_PORT, BSP_SPI_SCK_PINSOURCE, BSP_GPIO_AF); 
    GPIO_PinAFConfig(BSP_GPIO_PORT, BSP_SPI_MISO_PINSOURCE, BSP_GPIO_AF); 
    GPIO_PinAFConfig(BSP_GPIO_PORT, BSP_SPI_MOSI_PINSOURCE, BSP_GPIO_AF); 

    /* 配置SPI引脚引脚SCK */
    GPIO_InitStructure.GPIO_Pin = BSP_SPI_SCK;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  

    GPIO_Init(BSP_GPIO_PORT, &GPIO_InitStructure);

    /* 配置SPI引脚MISO */
    GPIO_InitStructure.GPIO_Pin = BSP_SPI_MISO;
    GPIO_Init(BSP_GPIO_PORT, &GPIO_InitStructure);

    /* 配置SPI引脚MOSI */
    GPIO_InitStructure.GPIO_Pin = BSP_SPI_MOSI;
    GPIO_Init(BSP_GPIO_PORT, &GPIO_InitStructure);  

    /* 配置SPI引脚CS */
    GPIO_InitStructure.GPIO_Pin = BSP_SPI_NSS;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(BSP_GPIO_PORT, &GPIO_InitStructure);

    /* CS引脚高电平 */
    W25QXX_CS_ON(1);

    /* FLASH_SPI 模式配置 */
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; 		// 传输模式全双工
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                			// 配置为主机
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;            			// 8位数据
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;                			// 极性相位
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                   			// 软件cs
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; 		// SPI时钟预调因数为2
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;        				//高位在前
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(BSP_SPI, &SPI_InitStructure);

    /* 使能 FLASH_SPI  */
    SPI_Cmd(BSP_SPI, ENABLE);
	
	W25QXX_CS_ON(1); 														// 片选拉高
}


/*全局变量声明*/
static DMA_InitTypeDef DMA_InitStructure;  									// 全局DMA配置结构体
static uint8_t dma_tx_complete_flag = 0;   									// DMA传输完成标志（中断置1，主函数清0）
/**********************************************************
 * 函 数 名 称：bsp_spi_dma_tx_init
 * 函 数 功 能：SPI DMA TX初始化（仅用于字模数据传输）
 * 传 入 参 数：无
 * 函 数 返 回：无
 * 作       者：雪碧的情人
 * 备       注：
**********************************************************/
void bsp_spi_dma_tx_init(void) 
{
    NVIC_InitTypeDef NVIC_InitStructure;

    // 1. 使能DMA时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

    // 2. 关闭DMA流
    DMA_Cmd(SPI_DMA_TX_STREAM, DISABLE);
    while (DMA_GetCmdStatus(SPI_DMA_TX_STREAM) != DISABLE);

    // 3. 配置DMA核心参数
    DMA_InitStructure.DMA_Channel = SPI_DMA_TX_CHANNEL;  							// 对应SPI_TX通道
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(BSP_SPI->DR);  			// SPI数据寄存器地址
    DMA_InitStructure.DMA_Memory0BaseAddr = 0;  									// 初始内存地址（后续动态修改）
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;  						// 内存→外设
    DMA_InitStructure.DMA_BufferSize = 0;  											// 初始传输长度（后续动态修改）
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  				// 外设地址不递增
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  						// 内存地址递增（字模连续）
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  		// 8位数据
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  				// 8位数据
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  									// 普通模式（一次传输后停止）
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; 							// 中等优先级
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;  						// 禁用FIFO
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

    DMA_Init(SPI_DMA_TX_STREAM, &DMA_InitStructure);

    // 4. 使能DMA传输完成中断
    DMA_ITConfig(SPI_DMA_TX_STREAM, DMA_IT_TC, ENABLE);

    // 5. 配置DMA中断优先级
    NVIC_InitStructure.NVIC_IRQChannel = SPI_DMA_TX_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;         
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // 6. 使能SPI DMA发送功能
    SPI_DMACmd(BSP_SPI, SPI_DMAReq_Tx, ENABLE);
}



uint8_t spi_read_write_byte(uint8_t dat)
{
    //等待发送缓冲区为空
    while(RESET == SPI_I2S_GetFlagStatus(BSP_SPI,  SPI_I2S_FLAG_TXE) );
    //通过SPI4发送一个字节数据 
    SPI_I2S_SendData(BSP_SPI, dat);
 
    //等待接收缓冲区不空标志
    while(RESET == SPI_I2S_GetFlagStatus(BSP_SPI,  SPI_I2S_FLAG_RXNE) );
    //读取并返回在SPI读取到的单字节数据
    return SPI_I2S_ReceiveData(BSP_SPI);
}

//读取芯片ID          
//读取设备ID
uint16_t W25Q128_readID(void)
{
    uint16_t  temp = 0;     
    //将CS端拉低为低电平     
    W25QXX_CS_ON(0);        
    //发送指令90h    
    spi_read_write_byte(0x90);//发送读取ID命令      
    //发送地址  000000H    
    spi_read_write_byte(0x00);             
    spi_read_write_byte(0x00);             
    spi_read_write_byte(0x00); 
        
    //接收数据
    //接收制造商ID
    temp |= spi_read_write_byte(0xFF)<<8;  
    //接收设备ID
    temp |= spi_read_write_byte(0xFF);        
    //恢复CS端为高电平
    W25QXX_CS_ON(1);      
    //返回ID                  
    return temp;
}

//发送写使能
void W25Q128_write_enable(void)   
{
    //拉低CS端为低电平
    W25QXX_CS_ON(0);                          
    //发送指令06h
    spi_read_write_byte(0x06);                  
    //拉高CS端为高电平
    W25QXX_CS_ON(1);
}

/**********************************************************
 * 函 数 名 称：W25Q128_wait_busy
 * 函 数 功 能：检测线路是否繁忙
 * 传 入 参 数：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
**********************************************************/
void W25Q128_wait_busy(void)   
{   
    unsigned char byte = 0;
    do
     { 
        //拉低CS端为低电平
        W25QXX_CS_ON(0); 
        //发送指令05h                           
        spi_read_write_byte(0x05);                
        //接收状态寄存器值
        byte = spi_read_write_byte(0Xff);       
        //恢复CS端为高电平
        W25QXX_CS_ON(1);      
     //判断BUSY位是否为1 如果为1说明在忙，重新读写BUSY位直到为0   
     }while( ( byte & 0x01 ) == 1 );  
}

/**********************************************************
 * 函 数 名 称：W25Q128_erase_sector
 * 函 数 功 能：擦除一个扇区
 * 传 入 参 数：addr=擦除的扇区号
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：addr=擦除的扇区号，范围=0~4096。

W25Q128将16M的容量分为256个块（Block),每个块大小为64K（64000）个字节，每个块又分为16个扇区（Sector),每个扇区4K个字节。
W25Q128的最小擦除单位为一个扇区，也就是每次必须擦除4K个字节。
**********************************************************/
void W25Q128_erase_sector(uint32_t addr)   
{
        //计算扇区号，一个扇区4KB=4096
        addr *= 4096;
        W25Q128_write_enable();  //写使能   
        W25Q128_wait_busy();     //判断忙，如果忙则一直等待
        //拉低CS端为低电平
        W25QXX_CS_ON(0);  
        //发送指令20h                                     
        spi_read_write_byte(0x20);
        //发送24位扇区地址的高8位                
        spi_read_write_byte((uint8_t)((addr)>>16));      
        //发送24位扇区地址的中8位    
        spi_read_write_byte((uint8_t)((addr)>>8));   
        //发送24位扇区地址的低8位    
        spi_read_write_byte((uint8_t)addr);
        //恢复CS端为高电平  
        W25QXX_CS_ON(1);                  
        //等待擦除完成                                                  
        W25Q128_wait_busy();   
}

/**********************************************************
 * 函 数 名 称：W25Q128_write
 * 函 数 功 能：写数据到W25Q128进行保存
 * 传 入 参 数：buffer=写入的数据内容  addr=写入地址  numbyte=写入数据的长度
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
**********************************************************/
void W25Q128_write(uint8_t* buffer, uint32_t addr, uint16_t numbyte)
{    
    unsigned int i = 0;
    //擦除扇区数据
    W25Q128_erase_sector(addr/4096);
    //写使能 
    W25Q128_write_enable();  
    //忙检测  
    W25Q128_wait_busy();    
    //写入数据
    //拉低CS端为低电平
    W25QXX_CS_ON(0);         
    //发送指令02h                              
    spi_read_write_byte(0x02);                 
    //发送写入的24位地址中的高8位   
    spi_read_write_byte((uint8_t)((addr)>>16));  
    //发送写入的24位地址中的中8位
    spi_read_write_byte((uint8_t)((addr)>>8));   
    //发送写入的24位地址中的低8位
    spi_read_write_byte((uint8_t)addr);   
    //根据写入的字节长度连续写入数据buffer
    for(i=0;i<numbyte;i++)
    {
        spi_read_write_byte(buffer[i]);  
    }
    //恢复CS端为高电平
    W25QXX_CS_ON(1);
    //忙检测 
    W25Q128_wait_busy();      
}

/**********************************************************
 * 函 数 名 称：W25Q128_read
 * 函 数 功 能：读取W25Q128的数据
 * 传 入 参 数：buffer=读出数据的保存地址  read_addr=读取地址   read_length=读去长度
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
**********************************************************/
void W25Q128_read(uint8_t* buffer,uint32_t read_addr,uint16_t read_length)   
{ 
        uint16_t i;                   
        //拉低CS端为低电平
        W25QXX_CS_ON(0);    
        //发送指令03h        
        spi_read_write_byte(0x03);  
        //发送24位读取数据地址的高8位                         
        spi_read_write_byte((uint8_t)((read_addr)>>16));     
        //发送24位读取数据地址的中8位      
        spi_read_write_byte((uint8_t)((read_addr)>>8));   
        //发送24位读取数据地址的低8位
        spi_read_write_byte((uint8_t)read_addr);   
        //根据读取长度读取出地址保存到buffer中
        for(i=0;i<read_length;i++)
        { 
            buffer[i]= spi_read_write_byte(0XFF);  
        }
        //恢复CS端为高电平
        W25QXX_CS_ON(1);                                    
}




/**********************************************************
 * 函 数 名 称：W25Q128_ContinuousWrite
 * 函 数 功 能：跨扇区连续写入（字模数据用DMA，指令/地址用SPI查询）
 * 传 入 参 数：buffer=写入数据；addr=起始地址；numbyte=总长度
 * 函 数 返 回：无
 * 作       者：雪碧的情人
 * 备       注：无
**********************************************************/

void W25Q128_ContinuousWrite(uint8_t* buffer, uint32_t addr, uint32_t numbyte) 
{
    printf("=== 写入开始：总长度=%d字节，起始地址=0x%X ===\n", numbyte, addr);
    if (buffer == NULL || numbyte == 0) return;

    uint32_t curr_addr = addr;      														// 当前写入地址
    uint32_t remain_len = numbyte;  														// 剩余写入长度
    uint8_t* curr_buf = buffer;     														// 当前数据指针
    uint32_t write_len;             														// 单次写入长度（≤64字节）
    uint32_t write_count = 0;       														// 已写入总字节数

    /************************** 第一步：批量擦除扇区 **************************/
    uint32_t start_sector = addr / W25Q128_SECTOR_SIZE;
    uint32_t end_sector = (addr + numbyte - 1) / W25Q128_SECTOR_SIZE;
    printf("需要擦除扇区：%d ~ %d（共%d个）\n", start_sector, end_sector, end_sector - start_sector + 1);
    for (uint32_t sec = start_sector; sec <= end_sector; sec++) 
    {
        printf("擦除扇区%d...\n", sec);
        W25Q128_erase_sector(sec);
        W25Q128_wait_busy();
        printf("扇区%d擦除完成\n", sec);
    }

    /************************** 第二步：分批次DMA写入 **************************/
    while (remain_len > 0) 
    {
        // 1. 计算单次写入长度
        write_len = W25Q128_PAGE_SIZE - (curr_addr & (W25Q128_PAGE_SIZE - 1));
        if (write_len > remain_len) write_len = remain_len;
        if (write_len > DMA_TX_BUFFER_SIZE) write_len = DMA_TX_BUFFER_SIZE;

        printf("写入：地址=0x%X，长度=%d字节，剩余=%d字节\n", curr_addr, write_len, remain_len);

        // 2. SPI指令+地址发送
        W25Q128_write_enable();
        W25Q128_wait_busy();

        W25QXX_CS_ON(0);
        spi_read_write_byte(0x02);  														// 发送页写入指令（0x02）
        spi_read_write_byte((uint8_t)(curr_addr >> 16));  									// 地址高8位
        spi_read_write_byte((uint8_t)(curr_addr >> 8));   									// 地址中8位
        spi_read_write_byte((uint8_t)curr_addr);          									// 地址低8位

        // 3. DMA传输字模数据（优化：超时重试）
        dma_tx_complete_flag = 0;  															// 清传输完成标志
        uint8_t retry = 3; 
        while (retry--) 
		{
            
            DMA_Cmd(SPI_DMA_TX_STREAM, DISABLE);
            while (DMA_GetCmdStatus(SPI_DMA_TX_STREAM) != DISABLE);
            DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)curr_buf;
            DMA_InitStructure.DMA_BufferSize = write_len;
            DMA_Init(SPI_DMA_TX_STREAM, &DMA_InitStructure);
            DMA_Cmd(SPI_DMA_TX_STREAM, ENABLE);

            
            uint32_t dma_timeout = 1000000;
            while (dma_tx_complete_flag == 0 && dma_timeout-- > 0);
            if (dma_tx_complete_flag == 1) 
			{
                break;  																	// 重试成功
            }
            printf("DMA传输重试：地址=0x%X，剩余重试次数=%d\n", curr_addr, retry);
        }

																							// 重试失败→报错返回
        if (retry == 0 && dma_tx_complete_flag == 0) 
		{
            printf("ERROR：DMA传输失败！地址=0x%X，长度=%d\n", curr_addr, write_len);
            W25QXX_CS_ON(1);
            return;
        }

        // 打印验证（传输数据+数据源）
        printf("  DMA传输完成：");
        for (uint32_t i = 0; i < write_len && i < 16; i++) 
		{
            printf("0x%02X ", curr_buf[i]);
        }
        printf("\n");
        printf("  数据源前8字节：");
        for (uint32_t i = 0; i < 8 && i < write_len; i++) 
		{
            printf("0x%02X ", curr_buf[i]);
        }
        printf("\n");

        // 4. 结束当前批次写入
        W25QXX_CS_ON(1);
        W25Q128_wait_busy();  																// 等待Flash内部写入完成

        // 5. 更新参数，准备下一批
        curr_addr += write_len;
        curr_buf += write_len;
        remain_len -= write_len;
        write_count += write_len;

        // 打印进度（每1KB打印一次）
        if (write_count % 1024 == 0) 
        {
            printf("=== 已写入%d字节（%.2f%%）===\n", write_count, (float)write_count/numbyte*100);
        }
    }

    printf("=== 写入结束：实际写入%d字节 ===\n", write_count);
}
/**********************************************************
 * 中 断 函 数：SPI_DMA_TX_IRQHandler
 * 函 数 功 能：DMA传输完成中断处理
 * 作       者：雪碧的情人
 * 备       注：
**********************************************************/
void SPI_DMA_TX_IRQHandler(void) 
{
    // 检查传输完成中断标志
    if (DMA_GetITStatus(SPI_DMA_TX_STREAM, DMA_IT_TCIF3) != RESET) 
	{
        DMA_ClearITPendingBit(SPI_DMA_TX_STREAM, DMA_IT_TCIF3);   // 清除中断标志
        DMA_Cmd(SPI_DMA_TX_STREAM, DISABLE);                      // 关闭DMA流
        while (DMA_GetCmdStatus(SPI_DMA_TX_STREAM) != DISABLE);   // 等待关闭完成
        dma_tx_complete_flag = 1;                                 // 置位传输完成标志
    }
}

