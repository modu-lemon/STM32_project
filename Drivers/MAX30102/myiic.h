#ifndef __MAX30102_H
#define __MAX30102_H

#include "main.h"
void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
uint8_t IIC_Wait_Ack(void);
void IIC_Ack(void);
void IIC_NAck(void);
void IIC_Send_Byte(uint8_t  txd);
uint8_t IIC_Read_Byte(uint8_t ack);

#endif
















