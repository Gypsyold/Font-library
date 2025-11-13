#include "EPD_GUI.h"
#include "EPD_Font.h"

PAINT Paint;


/*******************************************************************
		函数说明：创建图片缓存数组
		接口说明：*image  要传入的图片数组
               Width  图片宽度
               Heighe 图片长度
               Rotate 屏幕显示方向
               Color  显示颜色
		返回值：  无
*******************************************************************/
void Paint_NewImage(u8 *image,u16 Width,u16 Height,u16 Rotate,u16 Color)
{
	Paint.Image = 0x00;
	Paint.Image = image;
	Paint.color = Color;  
	Paint.widthMemory = Width;
	Paint.heightMemory = Height;  
	Paint.widthByte = (Width % 8 == 0)? (Width / 8 ): (Width / 8 + 1);
	Paint.heightByte = Height;     
	Paint.rotate = Rotate;
	if(Rotate==0||Rotate==180) 
	{
		Paint.width=Height;
		Paint.height=Width;
	} 
	else 
	{
		Paint.width = Width;
		Paint.height = Height;
	}
}				 

/*******************************************************************
		函数说明：清空缓冲区 
		接口说明：Color  像素点颜色参数
		返回值：  无
*******************************************************************/
void Paint_Clear(u8 Color)
{
	
	u16 X,Y;
	u32 Addr;
	for(Y=0;Y<Paint.heightByte;Y++) 
	{
		for(X=0;X<Paint.widthByte;X++) 
		{   
			Addr=X+Y*Paint.widthByte;//8 pixel =  1 byte
			Paint.Image[Addr]=Color;
		}
	 }
}


/*******************************************************************
		函数说明：点亮一个像素点
		接口说明：Xpoint 像素点x坐标参数
              Ypoint 像素点Y坐标参数
              Color  像素点颜色参数
		返回值：  无
*******************************************************************/
void Paint_SetPixel(u16 Xpoint,u16 Ypoint,u16 Color)
{
	u16 X, Y;
	u32 Addr;
	u8 Rdata;
    switch(Paint.rotate) 
		{
			case 0:
					X=Ypoint;
					Y=Xpoint;		
					break;
			case 90:
					X=Xpoint;
					Y=Paint.heightMemory-Ypoint-1;
					break;
			case 180:
					X=Paint.widthMemory-Ypoint-1;
					Y=Paint.heightMemory-Xpoint-1;
					break;
			case 270:
					X=Paint.widthMemory-Xpoint-1;
					Y=Ypoint;
					break;
				default:
						return;
    }
		Addr=X/8+Y*Paint.widthByte;
    Rdata=Paint.Image[Addr];
    if(Color==BLACK)
    {    
			Paint.Image[Addr]=Rdata&~(0x80>>(X % 8)); //将对应数据位置0
		}
    else
		{
      Paint.Image[Addr]=Rdata|(0x80>>(X % 8));   //将对应数据位置1  
		}
}


/*******************************************************************
		函数说明：划线函数
		接口说明：Xstart 像素x起始坐标参数
              Ystart 像素Y起始坐标参数
							Xend   像素x结束坐标参数
              Yend   像素Y结束坐标参数
              Color  像素点颜色参数
		返回值：  无
*******************************************************************/
void EPD_DrawLine(u16 Xstart,u16 Ystart,u16 Xend,u16 Yend,u16 Color)
{   
	u16 Xpoint, Ypoint;
	int dx, dy;
	int XAddway,YAddway;
	int Esp;
	char Dotted_Len;
  Xpoint = Xstart;
  Ypoint = Ystart;
  dx = (int)Xend - (int)Xstart >= 0 ? Xend - Xstart : Xstart - Xend;
  dy = (int)Yend - (int)Ystart <= 0 ? Yend - Ystart : Ystart - Yend;
  XAddway = Xstart < Xend ? 1 : -1;
  YAddway = Ystart < Yend ? 1 : -1;
  Esp = dx + dy;
  Dotted_Len = 0;
  for (;;) {
        Dotted_Len++;
            Paint_SetPixel(Xpoint, Ypoint, Color);
        if (2 * Esp >= dy) {
            if (Xpoint == Xend)
                break;
            Esp += dy;
            Xpoint += XAddway;
        }
        if (2 * Esp <= dx) {
            if (Ypoint == Yend)
                break;
            Esp += dx;
            Ypoint += YAddway;
        }
    }
}
/*******************************************************************
		函数说明：画矩形函数
		接口说明：Xstart 矩形x起始坐标参数
              Ystart 矩形Y起始坐标参数
							Xend   矩形x结束坐标参数
              Yend   矩形Y结束坐标参数
              Color  像素点颜色参数
              mode   矩形是否进行填充
		返回值：  无
*******************************************************************/
void EPD_DrawRectangle(u16 Xstart,u16 Ystart,u16 Xend,u16 Yend,u16 Color,u8 mode)
{
	u16 i;
    if (mode)
			{
        for(i = Ystart; i < Yend; i++) 
				{
          EPD_DrawLine(Xstart,i,Xend,i,Color);
        }
      }
		else 
		 {
        EPD_DrawLine(Xstart, Ystart, Xend, Ystart, Color);
        EPD_DrawLine(Xstart, Ystart, Xstart, Yend, Color);
        EPD_DrawLine(Xend, Yend, Xend, Ystart, Color);
        EPD_DrawLine(Xend, Yend, Xstart, Yend, Color);
		 }
}
/*******************************************************************
		函数说明：画圆函数
		接口说明：X_Center 圆心x起始坐标参数
              Y_Center 圆心Y坐标参数
							Radius   圆形半径参数
              Color  像素点颜色参数
              mode   圆形是否填充显示
		返回值：  无
*******************************************************************/
void EPD_DrawCircle(u16 X_Center,u16 Y_Center,u16 Radius,u16 Color,u8 mode)
{
	int Esp, sCountY;
	u16 XCurrent, YCurrent;
  XCurrent = 0;
  YCurrent = Radius;
  Esp = 3 - (Radius << 1 );
    if (mode) {
        while (XCurrent <= YCurrent ) { //Realistic circles
            for (sCountY = XCurrent; sCountY <= YCurrent; sCountY ++ ) {
                Paint_SetPixel(X_Center + XCurrent, Y_Center + sCountY, Color);//1
                Paint_SetPixel(X_Center - XCurrent, Y_Center + sCountY, Color);//2
                Paint_SetPixel(X_Center - sCountY, Y_Center + XCurrent, Color);//3
                Paint_SetPixel(X_Center - sCountY, Y_Center - XCurrent, Color);//4
                Paint_SetPixel(X_Center - XCurrent, Y_Center - sCountY, Color);//5
                Paint_SetPixel(X_Center + XCurrent, Y_Center - sCountY, Color);//6
                Paint_SetPixel(X_Center + sCountY, Y_Center - XCurrent, Color);//7
                Paint_SetPixel(X_Center + sCountY, Y_Center + XCurrent, Color);
            }
            if ((int)Esp < 0 )
                Esp += 4 * XCurrent + 6;
            else {
                Esp += 10 + 4 * (XCurrent - YCurrent );
                YCurrent --;
            }
            XCurrent ++;
        }
    } else { //Draw a hollow circle
        while (XCurrent <= YCurrent ) {
            Paint_SetPixel(X_Center + XCurrent, Y_Center + YCurrent, Color);//1
            Paint_SetPixel(X_Center - XCurrent, Y_Center + YCurrent, Color);//2
            Paint_SetPixel(X_Center - YCurrent, Y_Center + XCurrent, Color);//3
            Paint_SetPixel(X_Center - YCurrent, Y_Center - XCurrent, Color);//4
            Paint_SetPixel(X_Center - XCurrent, Y_Center - YCurrent, Color);//5
            Paint_SetPixel(X_Center + XCurrent, Y_Center - YCurrent, Color);//6
            Paint_SetPixel(X_Center + YCurrent, Y_Center - XCurrent, Color);//7
            Paint_SetPixel(X_Center + YCurrent, Y_Center + XCurrent, Color);//0
            if ((int)Esp < 0 )
                Esp += 4 * XCurrent + 6;
            else {
                Esp += 10 + 4 * (XCurrent - YCurrent );
                YCurrent --;
            }
            XCurrent ++;
        }
    }
}

/******************************************************************************
      函数说明：显示汉字串
      入口数据：x,y显示坐标
                *s 要显示的汉字串
                sizey 字号 
                color 文字颜色
      返回值：  无
******************************************************************************/
void EPD_ShowChinese(u16 x,u16 y,u8 *s,u8 sizey,u16 color)
{
	while(*s!=0)
	{
		if(sizey==12) EPD_ShowChinese12x12(x,y,s,sizey,color);
		else if(sizey==16) EPD_ShowChinese16x16(x,y,s,sizey,color);
		else if(sizey==24) EPD_ShowChinese24x24(x,y,s,sizey,color);
		else if(sizey==32) EPD_ShowChinese32x32(x,y,s,sizey,color);
		else return;
		s+=2;
		x+=sizey;
	}
}

/******************************************************************************
      函数说明：显示单个12x12汉字
      入口数据：x,y显示坐标
                *s 要显示的汉字
                sizey 字号
                color 文字颜色
      返回值：  无
******************************************************************************/
void EPD_ShowChinese12x12(u16 x,u16 y,u8 *s,u8 sizey,u16 color)
{
	u8 i,j;
	u16 k;
	u16 HZnum;//汉字数目
	u16 TypefaceNum;//一个字符所占字节大小
	u16 x0=x;
	TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;                    
	HZnum=sizeof(tfont12)/sizeof(typFNT_GB12);	//统计汉字数目
	for(k=0;k<HZnum;k++) 
	{
		if((tfont12[k].Index[0]==*(s))&&(tfont12[k].Index[1]==*(s+1)))
		{ 	
			for(i=0;i<TypefaceNum;i++)
			{
				for(j=0;j<8;j++)
				{	
						if(tfont12[k].Msk[i]&(0x01<<j))	Paint_SetPixel(x,y,color);//画一个点
						x++;
						if((x-x0)==sizey)
						{
							x=x0;
							y++;
							break;
						}
				}
			}
		}				  	
		continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}
} 

/******************************************************************************
      函数说明：显示单个16x16汉字
      入口数据：x,y显示坐标
                *s 要显示的汉字
                sizey 字号
                color 文字颜色
      返回值：  无
******************************************************************************/
void EPD_ShowChinese16x16(u16 x,u16 y,u8 *s,u8 sizey,u16 color)
{
	u8 i,j;
	u16 k;
	u16 HZnum;//汉字数目
	u16 TypefaceNum;//一个字符所占字节大小
	u16 x0=x;
  TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;
	HZnum=sizeof(tfont16)/sizeof(typFNT_GB16);	//统计汉字数目
	for(k=0;k<HZnum;k++) 
	{
		if ((tfont16[k].Index[0]==*(s))&&(tfont16[k].Index[1]==*(s+1)))
		{ 	
			for(i=0;i<TypefaceNum;i++)
			{
				for(j=0;j<8;j++)
				{	
						if(tfont16[k].Msk[i]&(0x01<<j))	Paint_SetPixel(x,y,color);//画一个点
						x++;
						if((x-x0)==sizey)
						{
							x=x0;
							y++;
							break;
						}
				}
			}
		}				  	
		continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}
} 


/******************************************************************************
      函数说明：显示单个24x24汉字
      入口数据：x,y显示坐标
                *s 要显示的汉字
                sizey 字号
                color 文字颜色
      返回值：  无
******************************************************************************/
void EPD_ShowChinese24x24(u16 x,u16 y,u8 *s,u8 sizey,u16 color)
{
	u8 i,j;
	u16 k;
	u16 HZnum;//汉字数目
	u16 TypefaceNum;//一个字符所占字节大小
	u16 x0=x;
	TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;
	HZnum=sizeof(tfont24)/sizeof(typFNT_GB24);	//统计汉字数目
	for(k=0;k<HZnum;k++) 
	{
		if ((tfont24[k].Index[0]==*(s))&&(tfont24[k].Index[1]==*(s+1)))
		{ 	
			for(i=0;i<TypefaceNum;i++)
			{
				for(j=0;j<8;j++)
				{	
					if(tfont24[k].Msk[i]&(0x01<<j))	Paint_SetPixel(x,y,color);//画一个点
					x++;
					if((x-x0)==sizey)
					{
						x=x0;
						y++;
						break;
					}
				}
			}
		}				  	
		continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}
} 

/******************************************************************************
      函数说明：显示单个32x32汉字
      入口数据：x,y显示坐标
                *s 要显示的汉字
                sizey 字号
                color 文字颜色
      返回值：  无
******************************************************************************/
void EPD_ShowChinese32x32(u16 x,u16 y,u8 *s,u8 sizey,u16 color)
{
	u8 i,j;
	u16 k;
	u16 HZnum;//汉字数目
	u16 TypefaceNum;//一个字符所占字节大小
	u16 x0=x;
	TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;
	HZnum=sizeof(tfont32)/sizeof(typFNT_GB32);	//统计汉字数目
	for(k=0;k<HZnum;k++) 
	{
		if ((tfont32[k].Index[0]==*(s))&&(tfont32[k].Index[1]==*(s+1)))
		{ 	
			for(i=0;i<TypefaceNum;i++)
			{
				for(j=0;j<8;j++)
				{	
						if(tfont32[k].Msk[i]&(0x01<<j))	Paint_SetPixel(x,y,color);//画一个点
						x++;
						if((x-x0)==sizey)
						{
							x=x0;
							y++;
							break;
						}
				}
			}
		}				  	
		continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}
}


/*******************************************************************
		函数说明：显示单个字符
		接口说明：x 		 字符x坐标参数
              y 		 字符Y坐标参数
							chr    要显示的字符
              size1  显示字符字号大小
              Color  像素点颜色参数
		返回值：  无
*******************************************************************/
void EPD_ShowChar(u16 x,u16 y,u16 chr,u16 size1,u16 color)
{
	u16 i,m,temp,size2,chr1;
	u16 x0,y0;
	x0=x,y0=y;
	if(size1==8)size2=6;
	else size2=(size1/8+((size1%8)?1:0))*(size1/2);  //得到字体一个字符对应点阵集所占的字节数
	chr1=chr-' ';  //计算偏移后的值
	for(i=0;i<size2;i++)
	{
		if(size1==8)
			  {temp=asc2_0806[chr1][i];} //调用0806字体
		else if(size1==12)
        {temp=asc2_1206[chr1][i];} //调用1206字体
		else if(size1==16)
        {temp=asc2_1608[chr1][i];} //调用1608字体
		else if(size1==24)
        {temp=asc2_2412[chr1][i];} //调用2412字体
		else if(size1==48)
        {temp=asc2_4824[chr1][i];} //调用2412字体
		else return;
		for(m=0;m<8;m++)
		{
			if(temp&0x01)Paint_SetPixel(x,y,color);
			else Paint_SetPixel(x,y,!color);
			temp>>=1;
			y++;
		}
		x++;
		if((size1!=8)&&((x-x0)==size1/2))
		{x=x0;y0=y0+8;}
		y=y0;
  }
}

/*******************************************************************
		函数说明：显示字符串
		接口说明：x 		 字符串x坐标参数
              y 		 字符串Y坐标参数
							*chr    要显示的字符串
              size1  显示字符串字号大小
              Color  像素点颜色参数
		返回值：  无
*******************************************************************/
void EPD_ShowString(u16 x,u16 y,u8 *chr,u16 size1,u16 color)
{
	while(*chr!='\0')//判断是不是非法字符!
	{
		EPD_ShowChar(x,y,*chr,size1,color);
		chr++;
		x+=size1/2;
  }
}
/*******************************************************************
		函数说明：指数运算
		接口说明：m 底数
              n 指数
		返回值：  m的n次方
*******************************************************************/
u32 EPD_Pow(u16 m,u16 n)
{
	u32 result=1;
	while(n--)
	{
	  result*=m;
	}
	return result;
}
/*******************************************************************
		函数说明：显示整型数字
		接口说明：x 		 数字x坐标参数
              y 		 数字Y坐标参数
							num    要显示的数字
              len    数字的位数
              size1  显示字符串字号大小
              Color  像素点颜色参数
		返回值：  无
*******************************************************************/
void EPD_ShowNum(u16 x,u16 y,u32 num,u16 len,u16 size1,u16 color)
{
	u8 t,temp,m=0;
	if(size1==8)m=2;
	for(t=0;t<len;t++)
	{
		temp=(num/EPD_Pow(10,len-t-1))%10;
			if(temp==0)
			{
				EPD_ShowChar(x+(size1/2+m)*t,y,'0',size1,color);
      }
			else 
			{
			  EPD_ShowChar(x+(size1/2+m)*t,y,temp+'0',size1,color);
			}
  }
}
/*******************************************************************
		函数说明：显示浮点型数字
		接口说明：x 		 数字x坐标参数
              y 		 数字Y坐标参数
							num    要显示的浮点数
              len    数字的位数
              pre    浮点数的精度
              size1  显示字符串字号大小
              Color  像素点颜色参数
		返回值：  无
*******************************************************************/

void EPD_ShowFloatNum1(u16 x,u16 y,float num,u8 len,u8 pre,u8 sizey,u8 color)
{         	
	u8 t,temp,sizex;
	u16 num1;
	sizex=sizey/2;
	num1=num*EPD_Pow(10,pre);
	for(t=0;t<len;t++)
	{
		temp=(num1/EPD_Pow(10,len-t-1))%10;
		if(t==(len-pre))
		{
			EPD_ShowChar(x+(len-pre)*sizex,y,'.',sizey,color);
			t++;
			len+=1;
		}
	 	EPD_ShowChar(x+t*sizex,y,temp+48,sizey,color);
	}
}





//GUI显示秒表
void EPD_ShowWatch(u16 x,u16 y,float num,u8 len,u8 pre,u8 sizey,u8 color)
{         	
	u8 t,temp,sizex;
	u16 num1;
	sizex=sizey/2;
	num1=num*EPD_Pow(10,pre);
	for(t=0;t<len;t++)
	{
		temp=(num1/EPD_Pow(10,len-t-1))%10;
		if(t==(len-pre))
		{
			EPD_ShowChar(x+(len-pre)*sizex+(sizex/2-2),y-6,':',sizey,color);
			t++;
			len+=1;
		}
	 	EPD_ShowChar(x+t*sizex,y,temp+48,sizey,color);
	}
}



void EPD_ShowPicture(u16 x,u16 y,u16 sizex,u16 sizey,const u8 BMP[],u16 Color)
{
	u16 j=0,t;
	u16 i,n,temp;
	u16 y0;
	y0=y;
	sizex=sizex/8+((sizex%8)?1:0);
	for(n=0;n<sizey;n++)
	{
		for(i=0;i<sizex;i++)
		{
			temp=BMP[j];
			for(t=0;t<8;t++)
			{
			  if(temp&0x80)
			  {
				  Paint_SetPixel(x,y,!Color);
		   	}
			  else
			  {
				  Paint_SetPixel(x,y,Color);
		  	}
			  y++;
				temp<<=1;
			}
			if((y-y0)==sizey)
			{
				y=y0;
				x++;
			}
			j++;
		}
	}
}




