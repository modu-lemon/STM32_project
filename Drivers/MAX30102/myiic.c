#include "myiic.h"

#define MAX_SCL		MAX_SCL_Pin
#define MAX_SDA		MAX_SDA_Pin
#define MAX_GPIO	MAX_SCL_GPIO_Port

#define MAX_SCL_H	HAL_GPIO_WritePin(MAX_GPIO, MAX_SCL,GPIO_PIN_SET)
#define MAX_SCL_L	HAL_GPIO_WritePin(MAX_GPIO, MAX_SCL,GPIO_PIN_RESET)

#define MAX_SDA_H	HAL_GPIO_WritePin(MAX_GPIO, MAX_SDA,GPIO_PIN_SET)
#define MAX_SDA_L	HAL_GPIO_WritePin(MAX_GPIO, MAX_SDA,GPIO_PIN_RESET)

#define MAX_SCL_READ	HAL_GPIO_ReadPin(MAX_GPIO, MAX_SCL)
#define MAX_SDA_READ	HAL_GPIO_ReadPin(MAX_GPIO, MAX_SDA)

//// 微秒延时
//void DWT_Init(void)
//{
//	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;	// 使能DWT外设
//	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk ; 			// 使能Cortex-M DWT CYCCNT寄存器
//	DWT->CYCCNT = 0; 								// DWT CYCCNT寄存器计数清0
//}

//void delay_us(uint32_t us) {
//	volatile uint32_t cycles = (SystemCoreClock / 1000000L) * us;
//	volatile uint32_t start = DWT->CYCCNT;
//	do  {
//	} while(DWT->CYCCNT - start < cycles);
//}

void IIC_Delay(void)
{
    uint32_t i;
    for(i=0;i<75;i++)
    {
    
    }
}


void IIC_Init(void)
{                         
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = MAX_SCL | MAX_SDA;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;   
	GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(MAX_GPIO, &GPIO_InitStruct);
	
	DWT_Init();
	
	MAX_SCL_H;
	MAX_SDA_H;
}

void SDA_IN(void) 
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = MAX_SDA;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;  
	GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(MAX_GPIO, &GPIO_InitStruct);
}

void SDA_OUT(void) 
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = MAX_SDA;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;     
	GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(MAX_GPIO, &GPIO_InitStruct);
}

void IIC_Start(void)
{
	SDA_OUT();      
	MAX_SCL_H;
	MAX_SDA_H;
  delay_us(4);
	MAX_SDA_L;
  delay_us(4);
	MAX_SCL_L;
}      

void IIC_Stop(void)
{
	SDA_OUT();
	MAX_SCL_L;
	MAX_SDA_L;

  delay_us(4);
	MAX_SCL_H;
	MAX_SDA_H;
  delay_us(4);
}                                       
 
void IIC_Ack(void)
{
	MAX_SCL_L;
	SDA_OUT();
	MAX_SDA_L;
  delay_us(2);
	MAX_SCL_H;	
  delay_us(2);
	MAX_SCL_L;
}

void IIC_NAck(void)
{
	MAX_SCL_L;
	SDA_OUT();
	MAX_SDA_H;
  delay_us(2);
	MAX_SCL_H;	
  delay_us(2);
	MAX_SCL_L;
}

void IIC_Send_Byte(uint8_t txd)
{                        
	uint8_t i;   
	SDA_OUT();
	MAX_SCL_L;
	
	for(i = 0; i < 8; ++i)
	{
		//准备数据
		if(txd & (1<<(7-i)))
			MAX_SDA_H;
		else
			MAX_SDA_L;
		delay_us(2);
		MAX_SCL_H;	
		delay_us(2);
		MAX_SCL_L;
	}
}         

uint8_t IIC_Wait_Ack(void)
{
	uint8_t errTime = 0;
	uint8_t ack = 0;
	SDA_IN();
	MAX_SDA_H;
	delay_us(1);
	MAX_SCL_H;
	delay_us(1);
	
	while (MAX_SDA_READ)
	{
		++errTime;
		if (errTime > 250)
		{
			IIC_Stop();
			return 1;
		}
	}

	MAX_SCL_L;
	return 0;
} 

uint8_t  IIC_Read_Byte(uint8_t ack)
{
	uint8_t i, receive = 0;
	SDA_IN();
	MAX_SCL_L;
	
	for(i = 0; i < 8; ++i)
	{
		delay_us(2);
		MAX_SCL_H;	
		delay_us(2);
		MAX_SCL_L;

		if(MAX_SDA_READ)
			receive |= (0x01 << (7 - i));
	}

	if (!ack)
		IIC_NAck();
	else
		IIC_Ack();
	return receive;
}
