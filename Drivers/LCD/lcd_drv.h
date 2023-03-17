#ifndef __LCD_DRV_H
#define __LCD_DRV_H	   						  

#include "main.h"

#define   BLACK                0x0000                // ��ɫ��    0,   0,   0 //
#define   BLUE                 0x001F                // ��ɫ��    0,   0, 255 //
#define   GREEN                0x07E0                // ��ɫ��    0, 255,   0 //
#define   CYAN                 0x07FF                // ��ɫ��    0, 255, 255 //
#define   RED                  0xF800                // ��ɫ��  255,   0,   0 //
#define   MAGENTA              0xF81F                // Ʒ�죺  255,   0, 255 //
#define   YELLOW               0xFFE0                // ��ɫ��  255, 255, 0   //
#define   WHITE                0xFFFF                // ��ɫ��  255, 255, 255 //
#define   NAVY                 0x000F                // ����ɫ��  0,   0, 128 //
#define   DGREEN               0x03E0                // ����ɫ��  0, 128,   0 //
#define   DCYAN                0x03EF                // ����ɫ��  0, 128, 128 //
#define   MAROON               0x7800                // ���ɫ��128,   0,   0 //
#define   PURPLE               0x780F                // ��ɫ��  128,   0, 128 //
#define   OLIVE                0x7BE0                // ����̣�128, 128,   0 //
#define   LGRAY                0xC618                // �Ұ�ɫ��192, 192, 192 //
#define   DGRAY                0x7BEF                // ���ɫ��128, 128, 128 //

//��Ļ��ת���� ���ְ��� ID[1:0]AM ����PDF�е����ö���
#define ID_AM  110
//��Ļ��ʼʱ��ʾ��ʽ��ע�⣺��IDelayʱ��ʾ��һ��������������ˢ�µ�
//��ʱ�����ֶ���ˢ�½��������  LCD_WR_REG(0x0007,0x0173);������ʾ

//Ӳ����ص��Ӻ���
#define Bank1_LCD_D    ((uint32_t)0x60020000)    //Disp Data ADDR
#define Bank1_LCD_C    ((uint32_t)0x60000000)	   //Disp Reg ADDR

#define Lcd_Light_ON		HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_SET);
#define Lcd_Light_OFF		HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_RESET);

//Lcd��ʼ������ͼ����ƺ���
void Lcd_Configuration(void);
void Lcd_Initialize(void);
void LCD_WR_REG(uint16_t Index,uint16_t CongfigTemp);
//Lcd�߼����ƺ���

void Lcd_ColorBox(uint16_t x,uint16_t y,uint16_t xLong,uint16_t yLong,uint16_t Color);
void DrawPixel(uint16_t x, uint16_t y, uint16_t Color);
uint16_t GetPoint(uint16_t x,uint16_t y);
void BlockWrite(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend);
void LCD_Fill_Pic(uint16_t x, uint16_t y,uint16_t pic_H, uint16_t pic_V, const unsigned char* pic);
void LCD_Fill(uint16_t x, uint16_t y,uint16_t pic_H, uint16_t pic_V,uint16_t Color);

#endif   		     



