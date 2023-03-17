#include "lcd_drv.h"

#define LCD_delay HAL_Delay

static void LCD_Rst(void)
{
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);
    LCD_delay(100);
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);
    LCD_delay(100);
}
void WriteComm(uint16_t CMD)
{
    *(__IO uint16_t *) (Bank1_LCD_C) = CMD;
}
void WriteData(uint16_t tem_data)
{
    *(__IO uint16_t *) (Bank1_LCD_D) = tem_data;
}

/**********************************************
Lcd��ʼ������
***********************************************/
void Lcd_Initialize(void)
{
    LCD_Rst();

    LCD_WR_REG(0x11,0x2004);
    LCD_WR_REG(0x13,0xCC00);
    LCD_WR_REG(0x15,0x2600);
    LCD_WR_REG(0x14,0x252A);
//	LCD_WR_REG(0x14,0x002A);
    LCD_WR_REG(0x12,0x0033);
    LCD_WR_REG(0x13,0xCC04);
    LCD_delay(1);
    LCD_WR_REG(0x13,0xCC06);
    LCD_delay(1);
    LCD_WR_REG(0x13,0xCC4F);
    LCD_delay(1);
    LCD_WR_REG(0x13,0x674F);
    LCD_WR_REG(0x11,0x2003);
    LCD_delay(1);
    LCD_WR_REG(0x30,0x2609);
    LCD_WR_REG(0x31,0x242C);
    LCD_WR_REG(0x32,0x1F23);
    LCD_WR_REG(0x33,0x2425);
    LCD_WR_REG(0x34,0x2226);
    LCD_WR_REG(0x35,0x2523);
    LCD_WR_REG(0x36,0x1C1A);
    LCD_WR_REG(0x37,0x131D);
    LCD_WR_REG(0x38,0x0B11);
    LCD_WR_REG(0x39,0x1210);
    LCD_WR_REG(0x3A,0x1315);
    LCD_WR_REG(0x3B,0x3619);
    LCD_WR_REG(0x3C,0x0D00);
    LCD_WR_REG(0x3D,0x000D);
    LCD_WR_REG(0x16,0x0007);
    LCD_WR_REG(0x02,0x0013);
    LCD_WR_REG(0x03,0x0008);
    LCD_WR_REG(0x01,0x1027);
    LCD_delay(1);
    LCD_WR_REG(0x08,0x0303);
    LCD_WR_REG(0x0A,0x000B);
    LCD_WR_REG(0x0B,0x0003);
    LCD_WR_REG(0x0C,0x0000);
    LCD_WR_REG(0x41,0x0000);
    LCD_WR_REG(0x50,0x0000);
    LCD_WR_REG(0x60,0x0005);
    LCD_WR_REG(0x70,0x000B);
    LCD_WR_REG(0x71,0x0000);
    LCD_WR_REG(0x78,0x0000);
    LCD_WR_REG(0x7A,0x0000);
    LCD_WR_REG(0x79,0x0007);
    LCD_WR_REG(0x07,0x0051);
    LCD_delay(1);
    LCD_WR_REG(0x07,0x0053);
    LCD_WR_REG(0x79,0x0000);
    WriteComm(0x22);

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);;

	Lcd_ColorBox(0, 0, 320, 240, 0);
}
/******************************************
��������Lcdд�����
���ܣ���Lcdָ��λ��д��Ӧ�����������
��ڲ�����Index ҪѰַ�ļĴ�����ַ
          ConfigTemp д������ݻ�����ֵ
******************************************/
void LCD_WR_REG(uint16_t Index,uint16_t CongfigTemp)
{
    *(__IO uint16_t *) (Bank1_LCD_C) = Index;
    *(__IO uint16_t *) (Bank1_LCD_D) = CongfigTemp;
}
/************************************************
��������Lcdд��ʼ����
���ܣ�����Lcd�������� ִ��д����
************************************************/
void Lcd_WR_Start(void)
{
    *(__IO uint16_t *) (Bank1_LCD_C) = 0x22;
}
/*************************************************
��������Lcd�����㶨λ����
���ܣ�ָ��320240Һ���ϵ�һ����Ϊд���ݵ���ʼ��
��ڲ�����x ���� 0~239
          y ���� 0~319
����ֵ����
*************************************************/
void Lcd_SetCursor(uint16_t x,uint16_t y)
{
    //�����ת
    LCD_WR_REG(0x20,y);//ˮƽ����
    LCD_WR_REG(0x21,x);//��ֱ����
}

/**********************************************
��������Lcd��ѡ����
���ܣ�ѡ��Lcd��ָ���ľ�������

ע�⣺xStart��yStart��Xend��Yend������Ļ����ת���ı䣬λ���Ǿ��ο���ĸ���

��ڲ�����xStart x�������ʼ��
          ySrart y�������ʼ��
          Xend   y�������ֹ��
          Yend   y�������ֹ��
����ֵ����
***********************************************/
void BlockWrite(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend)
{
	uint16_t t;
	while (Xstart >= 320)
		Xstart -= 320;
	while (Ystart >= 240)
		Ystart -= 240;
	Xstart = 320 - Xstart - 1;
	Ystart = 240 - Ystart - 1;

	while (Xend >= 320)
		Xend -= 320;
	while (Yend >= 240)
		Yend -= 240;
	Xend = 320 - Xend - 1;
	Yend = 240 - Yend - 1;
	
	if (Yend < Ystart)
	{
		t = Ystart;
		Ystart = Yend;
		Yend = t;
	}
	if (Xend < Xstart)
	{
		t = Xstart;
		Xstart = Xend;
		Xend = t;
	}

	//x��y����
	LCD_WR_REG(0x0046,(Yend<<8)|Ystart);//ˮƽGRAM��ֹ����ʼλ��
	LCD_WR_REG(0x0047,Xend);//��ֱGRAM��ֹλ��
	LCD_WR_REG(0x0048,Xstart);//��ֱ GRAM��ʼλ��

	Lcd_SetCursor(Xstart, Ystart);

	WriteComm(0x022);
}
uint16_t GetPoint(uint16_t x,uint16_t y)
{
	if (x < 320)
		x = 320 - x;
	if (y < 240)
		y = 240 - y;
	Lcd_SetCursor(x, y);
	WriteComm(0x022);
	x = *(__IO uint16_t *) (Bank1_LCD_D);
	return  *(__IO uint16_t *) (Bank1_LCD_D);
}
/**********************************************
��������Lcd��ѡ����
���ܣ�ѡ��Lcd��ָ���ľ�������

ע�⣺xStart�� yStart������Ļ����ת���ı䣬λ���Ǿ��ο���ĸ���

��ڲ�����xStart x�������ʼ��
          ySrart y�������ֹ��
          xLong Ҫѡ�����ε�x���򳤶�
          yLong  Ҫѡ�����ε�y���򳤶�
����ֵ����
***********************************************/
void Lcd_ColorBox(uint16_t xStart,uint16_t yStart,uint16_t xLong,uint16_t yLong,uint16_t Color)
{
    uint32_t temp;

    BlockWrite(xStart,xStart+xLong-1,yStart,yStart+yLong-1);
    for (temp=0; temp<xLong*yLong; temp++)
    {
        *(__IO uint16_t *) (Bank1_LCD_D) = Color;
    }
}

/******************************************
��������Lcdͼ�����100*100
���ܣ���Lcdָ��λ�����ͼ��
��ڲ�����Index ҪѰַ�ļĴ�����ַ
          ConfigTemp д������ݻ�����ֵ
******************************************/
void LCD_Fill_Pic(uint16_t x, uint16_t y,uint16_t pic_H, uint16_t pic_V, const unsigned char* pic)
{
	int cnt = pic_H * pic_V;
	uint16_t *pDat = (uint16_t *)pic;
	BlockWrite(x, x + pic_H - 1, y, y + pic_V - 1);
	while (cnt-- > 0)
			*(__IO uint16_t *) (Bank1_LCD_D) = *pDat++;
}
void LCD_Fill(uint16_t x, uint16_t y,uint16_t pic_H, uint16_t pic_V,  uint16_t Color)
{
	int cnt = pic_H * pic_V;
	//uint16_t *pDat = (uint16_t *)pic;
	BlockWrite(x, x + pic_H - 1, y, y + pic_V - 1);
	while (cnt-- > 0)
			*(__IO uint16_t *) (Bank1_LCD_D) = Color;
}

//��ָ�������ϴ�һ����
void DrawPixel(uint16_t x, uint16_t y, uint16_t Color)
{
    BlockWrite(x,x,y,y);

    *(__IO uint16_t *) (Bank1_LCD_D) = Color;
}

