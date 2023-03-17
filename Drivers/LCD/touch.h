//*************************************************
//���ߣ�XING
//������������������
//����ʱ�䣺2010/11/08  
//����޸�ʱ�䣺
//*************************************************
/*
  �޸���ʷ:
  1,2010/11/23, 10:01,�������ģ��SPI����

*/
#ifndef  __TOUCH_H__
#define  __TOUCH_H__

#include "main.h"

// A/D ͨ��ѡ�������ֺ͹����Ĵ���
// #define	CHX 	0xd0 	//ͨ��Y+��ѡ�������	
// #define	CHY 	0x90	//ͨ��X+��ѡ�������
#define	CHX 	0x90 	//ͨ��Y+��ѡ�������	
#define	CHY 	0xd0	//ͨ��X+��ѡ�������

#define TP_CS(a)	\
						if (a)	\
						HAL_GPIO_WritePin(T_CS_GPIO_Port, T_CS_Pin, GPIO_PIN_SET);	\
						else		\
						HAL_GPIO_WritePin(T_CS_GPIO_Port, T_CS_Pin, GPIO_PIN_RESET)

void Touch_GPIO_Config(void);

int GUI_TOUCH_X_MeasureX(void); 
int GUI_TOUCH_X_MeasureY(void);

unsigned int TOUCH_X(void); 
unsigned int TOUCH_Y(void); 
void TP_GetAdXY(unsigned int *x,unsigned int *y);

char IsPressed(uint16_t x, uint16_t y,uint16_t x0, uint16_t y0, uint16_t lenth, uint16_t width,char* pressed);
char IsPressed_V2(uint16_t x, uint16_t y,uint16_t x0, uint16_t y0, uint16_t lenth, uint16_t width,char* pressed);
char IsPressed_V3(uint16_t x, uint16_t y,uint16_t x0, uint16_t y0, uint16_t lenth, uint16_t width,char* pressed);
uint8_t Touch_Poll(int32_t *x,int32_t *y);
void TouchProcess(void);


#endif                                     
