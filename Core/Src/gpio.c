/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LCD_BL_Pin|I2C_SCL_Pin|I2C_SDA_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, LCD_RST_Pin|BT_EN_Pin|BT_STA_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, SER_Pin|DISEN_Pin|A1_Pin|A2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, DISLK_Pin|SCK_Pin|A3_Pin|A0_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(beep_GPIO_Port, beep_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(DATA_GPIO_Port, DATA_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PEPin PEPin PEPin PEPin */
  GPIO_InitStruct.Pin = K2_Pin|K3_Pin|K4_Pin|K1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PEPin PEPin */
  GPIO_InitStruct.Pin = K5_Pin|K6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PBPin PBPin */
  GPIO_InitStruct.Pin = LCD_BL_Pin|beep_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PDPin PDPin PDPin */
  GPIO_InitStruct.Pin = LCD_RST_Pin|BT_EN_Pin|BT_STA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PCPin PCPin PCPin PCPin */
  GPIO_InitStruct.Pin = SER_Pin|DISEN_Pin|A1_Pin|A2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PAPin PAPin PAPin PAPin */
  GPIO_InitStruct.Pin = DISLK_Pin|SCK_Pin|A3_Pin|A0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PBPin PBPin */
  GPIO_InitStruct.Pin = I2C_SCL_Pin|I2C_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = DATA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DATA_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* USER CODE BEGIN 2 */
void setled(uint8_t sta)
{
//	HAL_GPIO_WritePin(L1_GPIO_Port,L1_Pin,(sta & 0x01) ? GPIO_PIN_RESET : GPIO_PIN_SET);
//	HAL_GPIO_WritePin(L2_GPIO_Port,L2_Pin,(sta & 0x02) ? GPIO_PIN_RESET : GPIO_PIN_SET);
//	HAL_GPIO_WritePin(L3_GPIO_Port,L3_Pin,(sta & 0x04) ? GPIO_PIN_RESET : GPIO_PIN_SET);
//	HAL_GPIO_WritePin(L4_GPIO_Port,L4_Pin,(sta & 0x08) ? GPIO_PIN_RESET : GPIO_PIN_SET);
//	HAL_GPIO_WritePin(L5_GPIO_Port,L5_Pin,(sta & 0x10) ? GPIO_PIN_RESET : GPIO_PIN_SET);
//	HAL_GPIO_WritePin(L6_GPIO_Port,L6_Pin,(sta & 0x20) ? GPIO_PIN_RESET : GPIO_PIN_SET);
//	HAL_GPIO_WritePin(L7_GPIO_Port,L7_Pin,(sta & 0x40) ? GPIO_PIN_RESET : GPIO_PIN_SET);
//	HAL_GPIO_WritePin(L8_GPIO_Port,L8_Pin,(sta & 0x80) ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

uint8_t keyscan()
{
	uint8_t key = 0;
	if(HAL_GPIO_ReadPin(K5_GPIO_Port,K5_Pin) == GPIO_PIN_SET)
	{
		key = 5;
		HAL_Delay(50);		
		if(HAL_GPIO_ReadPin(K5_GPIO_Port,K5_Pin) == GPIO_PIN_SET)
		{
			while(HAL_GPIO_ReadPin(K5_GPIO_Port,K5_Pin) == GPIO_PIN_SET);
			return key;
		}
		else
			return 0;
	}
	if(HAL_GPIO_ReadPin(K4_GPIO_Port,K4_Pin) == GPIO_PIN_RESET)
	{
		key = 4;
		HAL_Delay(50);
		if(HAL_GPIO_ReadPin(K4_GPIO_Port,K4_Pin) == GPIO_PIN_RESET)
		{
			while(HAL_GPIO_ReadPin(K4_GPIO_Port,K4_Pin) == GPIO_PIN_RESET);
			return key;
		}
		else
			return 0;
	}
	if(HAL_GPIO_ReadPin(K1_GPIO_Port,K1_Pin) == GPIO_PIN_RESET)
	{
		key = 1;
		HAL_Delay(50);
		if(HAL_GPIO_ReadPin(K1_GPIO_Port,K1_Pin) == GPIO_PIN_RESET)
		{
			while(HAL_GPIO_ReadPin(K1_GPIO_Port,K1_Pin) == GPIO_PIN_RESET);
			return key;
		}
		else
			return 0;
	}
	if(HAL_GPIO_ReadPin(K2_GPIO_Port,K2_Pin) == GPIO_PIN_RESET)
	{
		key = 2;
		HAL_Delay(50);
		if(HAL_GPIO_ReadPin(K2_GPIO_Port,K2_Pin) == GPIO_PIN_RESET)
		{
			while(HAL_GPIO_ReadPin(K2_GPIO_Port,K2_Pin) == GPIO_PIN_RESET);
			return key;
		}
		else
			return 0;
	}
	if(HAL_GPIO_ReadPin(K3_GPIO_Port,K3_Pin) == GPIO_PIN_RESET)
	{
		key = 3;
		HAL_Delay(50);
		if(HAL_GPIO_ReadPin(K3_GPIO_Port,K3_Pin) == GPIO_PIN_RESET)
		{
			while(HAL_GPIO_ReadPin(K3_GPIO_Port,K3_Pin) == GPIO_PIN_RESET);
			return key;
		}
		else
			return 0;
	}
	if(HAL_GPIO_ReadPin(K6_GPIO_Port,K6_Pin) == GPIO_PIN_SET)
	{
		key = 6;
		HAL_Delay(50);		
		if(HAL_GPIO_ReadPin(K6_GPIO_Port,K6_Pin) == GPIO_PIN_SET)
		{
			while(HAL_GPIO_ReadPin(K6_GPIO_Port,K6_Pin) == GPIO_PIN_SET);
			return key;
		}
		else
			return 0;
	}
	return 0;
}

// 单个数码管显示
void Write595(uint8_t sel, uint8_t num, uint8_t bdot)
{
	// 共阴数码管，'0'～'7'，'8'～'F'
	static const uint8_t TAB[16] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 
																	0x7F,	0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
	
	// 74HC138关数码管显示
	HAL_GPIO_WritePin(A3_GPIO_Port, A3_Pin, GPIO_PIN_RESET);
																	
	uint8_t dat = TAB[num & 0x0F] | (bdot ? 0x80 : 0x00);
	if (' ' == num) dat = 0;				// 空格关闭显示
	else if ('.' == num) dat = 0x80;
	else if ('-' == num) dat = 0x40;
	else if (num > 0x0F) dat = num;	// 其余数值按实际段码显示
																	
	// 595串行移位输入段码
	for (uint8_t i = 0; i < 8; ++i)
	{
		HAL_GPIO_WritePin(SCK_GPIO_Port, SCK_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SER_GPIO_Port, SER_Pin, (dat & 0x80) ? GPIO_PIN_SET : GPIO_PIN_RESET);
		dat <<= 1;
		HAL_GPIO_WritePin(SCK_GPIO_Port, SCK_Pin, GPIO_PIN_SET);
	}
	// DISLK脉冲锁存8位输出?
	HAL_GPIO_WritePin(DISLK_GPIO_Port, DISLK_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(DISLK_GPIO_Port, DISLK_Pin, GPIO_PIN_SET);
	
	// 4位数码管片选?
	HAL_GPIO_WritePin(A0_GPIO_Port, A0_Pin, (sel & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(A1_GPIO_Port, A1_Pin, (sel & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin(A2_GPIO_Port, A2_Pin, GPIO_PIN_RESET);
	
	// 74HC138开数码管显示?
	HAL_GPIO_WritePin(A3_GPIO_Port, A3_Pin, GPIO_PIN_SET);
}

/* USER CODE END 2 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
