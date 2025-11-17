#ifndef _EPD_GUI_H_
#define _EPD_GUI_H_

#include "EPD.h"

typedef struct {
	u8 *Image;
	u16 width;
	u16 height;
	u16 widthMemory;
	u16 heightMemory;
	u16 color;
	u16 rotate;
	u16 widthByte;
	u16 heightByte;
	
}PAINT;
extern PAINT Paint;

//定义E-Paper显示方向 
/*******************
Rotaion:0-0度方向
Rotaion:90-90度方向
Rotaion:180-180度方向
Rotaion:270-270度方向
*******************/
#define Rotation 0  

// 字体大小枚举（支持扩展更多字体）
typedef enum {
    FONT_16X16 = 0,  // 16×16 字体
    FONT_24X24 = 1,  // 24×24 字体
    FONT_32X32 = 2   // 32×32 字体
} Font_Size;

// 字体参数结构体（存储每种字体的关键信息）
typedef struct {
    uint8_t width;    // 字体宽度（像素）
    uint8_t height;   // 字体高度（像素）
    uint16_t bytes;   // 每个字模的字节数（width×height/8）
} Font_Param;


void Paint_NewImage(u8 *image,u16 Width,u16 Height,u16 Rotate,u16 Color); 					 //创建画布控制显示方向
void Paint_SetPixel(u16 Xpoint,u16 Ypoint,u16 Color);
void Paint_Clear(u8 Color);
void EPD_DrawLine(u16 Xstart,u16 Ystart,u16 Xend,u16 Yend,u16 Color);
void EPD_DrawRectangle(u16 Xstart,u16 Ystart,u16 Xend,u16 Yend,u16 Color,u8 mode);  //画矩形
void EPD_DrawCircle(u16 X_Center,u16 Y_Center,u16 Radius,u16 Color,u8 mode);        //画圆
void EPD_ShowChar(u16 x,u16 y,u16 chr,u16 size1,u16 color);                         //显示字符
void EPD_ShowString(u16 x,u16 y,u8 *chr,u16 size1,u16 color);                       //显示字符串
void EPD_ShowNum(u16 x,u16 y,u32 num,u16 len,u16 size1,u16 color);                  //显示数字
void EPD_ShowPicture(u16 x,u16 y,u16 sizex,u16 sizey,const u8 BMP[],u16 Color);			//显示图片      
void EPD_ClearWindows(u16 xs,u16 ys,u16 xe,u16 ye,u16 color);
void EPD_ShowFloatNum1(u16 x,u16 y,float num,u8 len,u8 pre,u8 sizey,u8 color);
void EPD_ShowWatch(u16 x,u16 y,float num,u8 len,u8 pre,u8 sizey,u8 color);

void EPD_ShowChinese(u16 x,u16 y,u8 *s,u8 sizey,u16 color);//显示汉字串
void EPD_ShowChinese12x12(u16 x,u16 y,u8 *s,u8 sizey,u16 color);//显示单个12x12汉字
void EPD_ShowChinese16x16(u16 x,u16 y,u8 *s,u8 sizey,u16 color);//显示单个16x16汉字
void EPD_ShowChinese24x24(u16 x,u16 y,u8 *s,u8 sizey,u16 color);//显示单个24x24汉字
void EPD_ShowChinese32x32(u16 x,u16 y,u8 *s,u8 sizey,u16 color);//显示单个32x32汉字
void EPD_ShowChineseString_flash(u16 x, u16 y, u8 *str, Font_Size font_size, u16 color);

#endif



