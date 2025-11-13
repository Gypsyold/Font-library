#include "bsp_uart.h" 
#include "stdio.h"

void uart1_init(uint32_t __Baud)
{


	RCC_AHB1PeriphClockCmd(BSP_USART_TX_RCC,ENABLE);      // 开启端口时钟
	RCC_APB2PeriphClockCmd(BSP_USART_RCC, ENABLE);    //开启串口1的时钟
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//将IO引脚复用配置成串口模式
	GPIO_PinAFConfig(BSP_USART_TX_PORT,BSP_USART_TX_AF_PIN,BSP_USART_AF);
	GPIO_PinAFConfig(BSP_USART_RX_PORT,BSP_USART_RX_AF_PIN,BSP_USART_AF);	

	
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin           = BSP_USART_TX_PIN;	//TX引脚
	GPIO_InitStructure.GPIO_Mode          = GPIO_Mode_AF;		//IO口用作串口引脚要配置为复用模式
	GPIO_InitStructure.GPIO_Speed         = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType         = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd          = GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin           = BSP_USART_RX_PIN;	//RX引脚
	GPIO_InitStructure.GPIO_Mode          = GPIO_Mode_AF;		//IO口用作串口引脚要配置为复用模式
	GPIO_InitStructure.GPIO_Speed         = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType         = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd          = GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);	



	//定义串口结构体变量
	USART_InitTypeDef USART_InitStructure;	

	USART_DeInit(BSP_USART);//解除串口1的其他配置

	USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate              = __Baud;//设置波特率
	USART_InitStructure.USART_WordLength            = USART_WordLength_8b;//字节长度为8bit
	USART_InitStructure.USART_StopBits              = USART_StopBits_1;//1个停止位
	USART_InitStructure.USART_Parity                = USART_Parity_No ;//没有校验位
	USART_InitStructure.USART_Mode                  = USART_Mode_Rx | USART_Mode_Tx;//串口配置为收发模式
	USART_InitStructure.USART_HardwareFlowControl   = USART_HardwareFlowControl_None; //不提供流控
	USART_Init(BSP_USART,&USART_InitStructure);//将相关参数初始化给串口1
	
	USART_ClearFlag(BSP_USART,USART_FLAG_RXNE);//初始化配置时清除接收标志位
	
	USART_Cmd(BSP_USART,ENABLE);//开启串口1
	
	USART_ITConfig(BSP_USART,USART_IT_RXNE,ENABLE);//初始配置接收中断
	
	
	NVIC_InitTypeDef NVIC_InitStructure;//中断控制结构体变量定义

	NVIC_InitStructure.NVIC_IRQChannel                    = USART1_IRQn;//中断通道指定为USART1
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority  = 0;//主优先级为0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority         = 1;//次优先级为1
	NVIC_InitStructure.NVIC_IRQChannelCmd                 = ENABLE;//确定使能
	NVIC_Init(&NVIC_InitStructure);//初始化配置此中断通道
	
	
}


void usart_send_data(uint8_t ucch)
{
    USART_SendData(BSP_USART, (uint8_t)ucch);

        // 等待发送数据缓冲区标志置位
    while( RESET == USART_GetFlagStatus(BSP_USART, USART_FLAG_TXE) ){}
}

void usart_send_String(uint8_t *ucstr)
{
      while(ucstr && *ucstr)  // 地址为空或者值为空跳出
      {
        usart_send_data(*ucstr++);
      }
}


#if !defined(__MICROLIB)
//不使用微库的话就需要添加下面的函数
#if (__ARMCLIB_VERSION <= 6000000)
//如果编译器是AC5  就定义下面这个结构体
struct __FILE
{
        int handle;
};
#endif

FILE __stdout;

//定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
        x = x;
}
#endif

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
      USART_SendData(BSP_USART, (uint8_t)ch);

      while( RESET == USART_GetFlagStatus(BSP_USART, USART_FLAG_TXE) ){}

      return ch;
}


