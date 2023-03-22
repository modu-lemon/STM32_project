/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "gpio.h"
#include "usart.h"
#include <ctype.h>
#include "tim.h"
#include "rtc.h"
#include "GUI.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lcd_drv.h"
#include "touch.h"
#include "DIALOG.h"
#include "MPU6050.h"
#include "math.h"
#include "DS18B20.h"
#include "max30102.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define WIFI_NAME  "28-517"
#define WIFI_PSW   "shanxianglu123..!!"
#define GUI_CLOCK  1  //ʱ����ʾ����
#define GUI_SHOW   2  //Ѫ��������ʾ����
#define GUI_DATA   3  //����ͬ������
#define UP         1  //�����ϼ�
#define DOWN       4  //�����¼�
#define LEFT 	   2  //�������
#define RIGHT 	   3  //�����Ҽ�
#define SWITCH	   5  //�����л���
#define FUNC 	   6  //�������ܼ�
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

uint8_t rx1_buf[512];	// ����1���ջ���     
uint8_t rx6_buf[512];	// ����6���ջ���     
uint8_t weather=0;         // 0������  1������ 4���� 5���������� 6���������� 7����󲿷ֶ��� 8���ϴ󲿷ֶ���
                           // 9�� 10����  11������ 12��������б��� 13С�� 14���� 15���� 16���� 17���� 18�ش���
													 // 19���� 20���ѩ 21��ѩ 22Сѩ 23��ѩ 24��ѩ 25��ѩ 26���� 27��ɳ 28ɳ���� 29ǿɳ����
													 // 30�� 31�� 32�� 33��� 34쫷� 35�ȴ��籩 36����� 37�� 38�� 99δ֪
uint8_t temp_high = 0;
uint8_t temp_low = 0;

int cid = -1;			// �ͻ���ID��

uint32_t g_beeptick=0;
uint8_t wea_flag=0;
uint8_t tmp_flag=0;
uint8_t g_bset=0;
uint8_t g_bset_old=0;
uint8_t g_bsay=0;
uint8_t gui_idx=0;
uint8_t gui_idx_old=0;
uint8_t alarm_sec=0;
uint8_t alarm_min=0;
uint8_t alarm_hour=0;
uint8_t set_alarm=0;
uint8_t alarm_flag=0;
uint8_t weathernum=0;
uint8_t temperature=0;
uint8_t day=5;
uint8_t stop_flag=0;
uint32_t times=0;
uint8_t beep_flag=0;
uint8_t time_flag=0;
uint8_t first_connect=0;
uint16_t real_year = 0, real_month = 0, real_day = 0, real_hour = 0, real_minute = 0, real_second = 0;
uint16_t old_year = 0, old_mon = 0, old_day = 0, old_hour = 0, old_min = 0, old_sec = 0;
float real_temp = 0; //��ǰ�¶�
float last_temp = 0; //��һ���¶�

//extern const GUI_FONT GUI_FontHZ_KaiTi_24;
extern const GUI_FONT GUI_FontHZ_KaiTi_24AA4;
extern GUI_CONST_STORAGE GUI_BITMAP bmhdulogo;  
extern GUI_CONST_STORAGE GUI_BITMAP bmqingtian;    //0-3����
extern GUI_CONST_STORAGE GUI_BITMAP bmduoyun;      //4-8 ����
extern GUI_CONST_STORAGE GUI_BITMAP bmyintian;     //9����
extern GUI_CONST_STORAGE GUI_BITMAP bmzhenyu;      //10-12����
extern GUI_CONST_STORAGE GUI_BITMAP bmyu;          //13-19��
extern GUI_CONST_STORAGE GUI_BITMAP bmxue;         //20-25ѩ
extern GUI_CONST_STORAGE GUI_BITMAP bmwumai;       //30 31����
extern GUI_CONST_STORAGE GUI_BITMAP bmweizhi;      // δ֪
extern GUI_CONST_STORAGE GUI_BITMAP bmbk1; //����
extern GUI_CONST_STORAGE GUI_BITMAP bmnaoling;
extern GUI_CONST_STORAGE GUI_BITMAP bmsmoke;
/* USER CODE END Variables */
/* Definitions for TaskRTC */
osThreadId_t TaskRTCHandle;
const osThreadAttr_t TaskRTC_attributes = {
  .name = "TaskRTC",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for myTaskKey */
osThreadId_t myTaskKeyHandle;
const osThreadAttr_t myTaskKey_attributes = {
  .name = "myTaskKey",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for myTaskGUI */
osThreadId_t myTaskGUIHandle;
const osThreadAttr_t myTaskGUI_attributes = {
  .name = "myTaskGUI",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for myTaskUart */
osThreadId_t myTaskUartHandle;
const osThreadAttr_t myTaskUart_attributes = {
  .name = "myTaskUart",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for myTaskWea */
osThreadId_t myTaskWeaHandle;
const osThreadAttr_t myTaskWea_attributes = {
  .name = "myTaskWea",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for QueueUart1 */
osMessageQueueId_t QueueUart1Handle;
const osMessageQueueAttr_t QueueUart1_attributes = {
  .name = "QueueUart1"
};
/* Definitions for QueueUart2 */
osMessageQueueId_t QueueUart2Handle;
const osMessageQueueAttr_t QueueUart2_attributes = {
  .name = "QueueUart2"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void Beep(uint8_t tune);
void SystemClock_Config(void);
void AnalyseWeather(void);
void EnterSleepMode(void);
void EnterStopMode(void);
void WeatherDisplay(void);
void pause(void);
void GetWeekDay(void);
uint8_t CheckIfLeapYear(uint16_t year);
void ShowWeekByCN(char *buf, uint8_t day);
void DrawLogo(void);
void DrawClock(void);
void DrawShow(void);
void DrawData(void);
void KeyDownClock(uint8_t key);
void KeyDownShow(uint8_t key);
void KeyDownData(uint8_t key);
void AnalyseTime(void);

void SendATCmd(char *cmd, int waitms)   // ����ATָ�������6
{ 
	if (NULL!=cmd)
	{
		HAL_UART_Transmit_DMA(&huart6,(uint8_t *)cmd,strlen(cmd));	
		osDelay(waitms);  // ��ʱ�ȴ�ESP01ģ��Ӧ��ʱ��
	}
}

void SendESP01Str(char *str)     // �����ַ���������6
{ 
	if (NULL!=str)
		HAL_UART_Transmit_DMA(&huart6,(uint8_t *)str,strlen(str));
}
uint32_t HexStrToNum(char *str,int nLen)
{
	uint32_t num=0;
	int i;
	for(i=0;i<nLen;++i)
	{
		char ch=str[i];
		uint32_t ts=toupper(ch)-'0';
		if(ts>9)
			ts-=7;
		if(ts<=15)
			num=num*16+ts;
		else
			break;
	}
	return num;
}
/* USER CODE END FunctionPrototypes */

void StartTaskRTC(void *argument);
void StartTaskKey(void *argument);
void StartTaskGUI(void *argument);
void StartdefaultTask(void *argument);
void StartTaskUart(void *argument);
void StartTaskWea(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of QueueUart1 */
  QueueUart1Handle = osMessageQueueNew (8, 512, &QueueUart1_attributes);

  /* creation of QueueUart2 */
  QueueUart2Handle = osMessageQueueNew (8, 512, &QueueUart2_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of TaskRTC */
  TaskRTCHandle = osThreadNew(StartTaskRTC, NULL, &TaskRTC_attributes);

  /* creation of myTaskKey */
  myTaskKeyHandle = osThreadNew(StartTaskKey, NULL, &myTaskKey_attributes);

  /* creation of myTaskGUI */
  myTaskGUIHandle = osThreadNew(StartTaskGUI, NULL, &myTaskGUI_attributes);

  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartdefaultTask, NULL, &defaultTask_attributes);

  /* creation of myTaskUart */
  myTaskUartHandle = osThreadNew(StartTaskUart, NULL, &myTaskUart_attributes);

  /* creation of myTaskWea */
  myTaskWeaHandle = osThreadNew(StartTaskWea, NULL, &myTaskWea_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartTaskRTC */
/**
  * @brief  Function implementing the TaskRTC thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartTaskRTC */
void StartTaskRTC(void *argument)
{
  /* USER CODE BEGIN StartTaskRTC */
  /* Infinite loop */
	
	for(;;)
	{

		if (!g_bset && (time_flag == 0 || time_flag == 2))
		ReadRTCDateTime();			
		osDelay(500);
	}
  /* USER CODE END StartTaskRTC */
}

/* USER CODE BEGIN Header_StartTaskKey */
/**
* @brief Function implementing the myTaskKey thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskKey */
void StartTaskKey(void *argument)
{
  /* USER CODE BEGIN StartTaskKey */
  /* Infinite loop */
	for(;;)
	{
		uint8_t key = keyscan();
		if (key > 0)
		{
			while (keyscan())
			osDelay(1);
			switch(gui_idx)
			{
				case GUI_CLOCK:
					//ʱ����ʾ����
					KeyDownClock(key);
					break;
				case GUI_SHOW:
					//Ѫ��������ʾ����
					KeyDownShow(key);
					break;
				case GUI_DATA:
					//����ͬ������
					KeyDownData(key);
					break;
				default:
					//Logo��ʾ
					break;
			}		  
		}
		osDelay(1);
	}
  /* USER CODE END StartTaskKey */
}

/* USER CODE BEGIN Header_StartTaskGUI */
/**
* @brief Function implementing the myTaskGUI thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskGUI */
void StartTaskGUI(void *argument)
{
  /* USER CODE BEGIN StartTaskGUI */
  /* Infinite loop */
	osDelay(100);
	GUI_Init();
	ds18b20_init();
	for(;;)
	{
		GUI_Exec();
		TouchProcess();
		
		if (time_flag == 1)
		{
			SetRTCDate(real_year, real_month, real_day);
			SetRTCTime(real_hour, real_minute, real_second);
			printf("change!!!\n");

			time_flag = 2;
		}
		//��ȡ���ڼ�
		GetWeekDay();		
		switch(gui_idx)
		{
			case GUI_CLOCK:
				//ʱ����ʾ����
				DrawClock();
				break;
			case GUI_SHOW:
				//Ѫ��������ʾ����
				DrawShow();
				break;
			case GUI_DATA:
				//����ͬ������
				DrawData();
				break;
			default:
				//Logo��ʾ
				DrawLogo();
				break;
		}
		osDelay(100);
	}
  /* USER CODE END StartTaskGUI */
}

/* USER CODE BEGIN Header_StartdefaultTask */
/**
* @brief Function implementing the defaultTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartdefaultTask */
void StartdefaultTask(void *argument)
{
  /* USER CODE BEGIN StartdefaultTask */
  /* Infinite loop */
	osDelay(500);
	Beep(5);
	osDelay(200);
	SendATCmd("+++", 500);				// �˳�͸��ģʽ
	SendATCmd("AT\r\n", 500);			// ����ESP01ģ���Ƿ����
	SendATCmd("ATE0\r\n", 500);			// �ر�ģ�����
	SendATCmd("AT+GMR\r\n", 500);		// �鿴ģ��汾��Ϣ
	SendATCmd("AT+CWMODE=3\r\n", 500);	// ����STA+APģʽ
	SendATCmd("AT+CIPMUX=0\r\n", 500);	// �رն�����
	
	char str[200];
	char weather_web[] = "GET https://api.seniverse.com/v3/weather/daily.json?key=SU8mkZy6Bwvor3FMJ&location=hangzhou&language=en&unit=c&start=0&days=1\r\n";
	char time_web[] = "GET http://quan.suning.com/getSysTime.do\r\n";
	sprintf(str, "AT+CWJAP=\"%s\",\"%s\"\r\n", WIFI_NAME, WIFI_PSW);
	SendATCmd(str, 10000);	// ��������·���������ֻ��ȵ㣬�ȴ�10��
	// ����ʱ�������
	SendATCmd("AT+CIPSTART=\"TCP\",\"quan.suning.com\",80\r\n", 2000);
	SendATCmd("AT+CIPMODE=1\r\n", 500);	// ����͸��ģʽ
	SendATCmd("AT+CIPSEND\r\n", 500);	// ��ʼ͸��	
	SendATCmd(time_web, 3000);
	SendATCmd("+++", 500);				// �˳�͸��ģʽ
	
	
	
	SendATCmd("AT+RST", 500);
	SendATCmd("AT\r\n", 500);			// ����ESP01ģ���Ƿ����
	SendATCmd("ATE0\r\n", 500);			// �ر�ģ�����
	SendATCmd("AT+GMR\r\n", 500);		// �鿴ģ��汾��Ϣ
	SendATCmd("AT+CWMODE=3\r\n", 500);	// ����STA+APģʽ
	SendATCmd("AT+CIPMUX=0\r\n", 500);	// �رն�����
	
	sprintf(str, "AT+CWJAP=\"%s\",\"%s\"\r\n", WIFI_NAME, WIFI_PSW);
	SendATCmd(str, 10000);	// ��������·���������ֻ��ȵ㣬�ȴ�10��
	
	for(;;)
	{
		if (wea_flag != 3)
		{
			if (first_connect == 0)
			{
				// ����OneNET��HTTP������
				SendATCmd("AT+CIPSTART=\"TCP\",\"116.62.81.138\",80\r\n", 2000);
				printf("connet weather server!!");
				SendATCmd("AT+CIPMODE=1\r\n", 500);	// ����͸��ģʽ
				SendATCmd("AT+CIPSEND\r\n", 500);	// ��ʼ͸��		
				first_connect = 1;
			}
			SendESP01Str(weather_web);	
		}

//		printf("send");
		osDelay(200);
	}

  /* USER CODE END StartdefaultTask */
}

/* USER CODE BEGIN Header_StartTaskUart */
/**
* @brief Function implementing the myTaskUart thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskUart */
void StartTaskUart(void *argument)
{
  /* USER CODE BEGIN StartTaskUart */
  /* Infinite loop */
  HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx1_buf, sizeof(rx1_buf));
  HAL_UARTEx_ReceiveToIdle_DMA(&huart6, rx6_buf, sizeof(rx6_buf));
	
	uint8_t i;
  uint8_t dat[512];  // ��ʱ����
  for(;;) {
	  
	// ����1�������ݴ���
	if (osMessageQueueGet(QueueUart1Handle, dat, NULL, 10) == osOK) {
		printf("UART1: %s", dat);  // ����1��ӡ����
      HAL_UART_Transmit_DMA(&huart6, dat, strlen((char *)dat));	// ת������6
		}

		// ����6�������ݴ���
	   	if (osMessageQueueGet(QueueUart2Handle, dat, NULL, 10) == osOK) {
			printf("UART6: %s", dat);	// ����1��ӡ��ʾ
		 if((rx6_buf[0] == '{' && rx6_buf[2] == 'r') || (rx6_buf[0] == '{' && rx6_buf[2] == 's'))
		 {
			 
				for(i = 0; i < 200; i++)
				{
					printf("%d",rx6_buf[i]);
				}
				
				//AnalyseWeather();
		 }

			
			char *str = (char *)dat;
			if (strstr(str, "CONNECT") > str) // ����пͻ������ӽ�����ȡID��
				cid = atoi(str);
			if (strstr(str, "CLOSED") > str)  // ����пͻ������ӶϿ����ر�ID��
				cid = -1;
		}


	   osDelay(100);
  }
  /* USER CODE END StartTaskUart */
}

/* USER CODE BEGIN Header_StartTaskWea */
/**
* @brief Function implementing the myTaskWea thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskWea */
void StartTaskWea(void *argument)
{
  /* USER CODE BEGIN StartTaskWea */
  /* Infinite loop */
	max30102_init();
	for(;;)
	{
		max30102ReadCalc();	  
		if(rx6_buf[0] == '{' && rx6_buf[2] == 'r')
		{
			
			//printf("\nweather = %d\n",weather);
			if (wea_flag != 3)
				AnalyseWeather();
		}
		if(rx6_buf[0] == '{' && rx6_buf[2] == 's')
		{
			if (time_flag == 0)
				AnalyseTime();
		}
		osDelay(10);
	}
  /* USER CODE END StartTaskWea */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void Beep(uint8_t tune)
{
	const float tab_freq[] = { 
		0,261.632f,293.672f,329.636f, 349.237f, 392.005f, 440.010f, 493.895f
	};
	if(tune>0 && tune <= 21)
	{
		int key = (tune-1) / 7;
		while(tune > 7)
			tune -= 7;
		uint16_t arr = (uint16_t)((1000000.0f / tab_freq[tune]) / 2) - 1;
		if(1 == key)
			arr /= 2;
		else if(2 == key)
			arr /= 4;
		HAL_TIM_Base_Start_IT(&htim4);
		__HAL_TIM_SET_AUTORELOAD(&htim4, arr);
		g_beeptick = osKernelGetTickCount() + 100;
	}
	else
	{
		HAL_TIM_Base_Stop_IT(&htim4);
		g_beeptick = 0;
	}
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size){
	if (huart == &huart1) { // ����1�������ݴ���
	  HAL_UART_DMAStop(huart);  // ֹͣDMA����
	  rx1_buf[Size] = '\0';      // ĩβ���ַ���������
	  osMessageQueuePut(QueueUart1Handle, rx1_buf, NULL, 0);  // ������Ϣ������1
	  // ��������1��DMA����
	  __HAL_UNLOCK(huart);
	  HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx1_buf, sizeof(rx1_buf));
	}
	if (huart == &huart6) { // ����6�������ݴ���
		 HAL_UART_DMAStop(huart);  // ֹͣDMA����
		 rx6_buf[Size] = '\0';      // ĩβ���ַ���������
		 osMessageQueuePut(QueueUart2Handle, rx6_buf, NULL, 0); // ������Ϣ������2
		 // ��������6��DMA����
		 __HAL_UNLOCK(huart);
		 HAL_UARTEx_ReceiveToIdle_DMA(&huart6, rx6_buf, sizeof(rx6_buf));
	}
}

void EnterSleepMode(void){
	HAL_RTCEx_SetWakeUpTimer_IT(&hrtc,10,RTC_WAKEUPCLOCK_CK_SPRE_16BITS);
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;
	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON,PWR_SLEEPENTRY_WFI);
}
void EnterStopMode(void){
	HAL_RTCEx_SetWakeUpTimer_IT(&hrtc,10,RTC_WAKEUPCLOCK_CK_SPRE_16BITS);
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;
	HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON,PWR_SLEEPENTRY_WFI);
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	SystemClock_Config();
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;
	HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
	HAL_RTC_DeactivateAlarm(&hrtc,RTC_ALARM_A);
	Lcd_Light_ON
}
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc){
	SystemClock_Config();
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;
	HAL_RTCEx_DeactivateWakeUpTimer(hrtc);
	Lcd_Light_ON
}
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc){
	SystemClock_Config();
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;
	HAL_RTC_DeactivateAlarm(hrtc,RTC_ALARM_A);
	Lcd_Light_ON
}


void AnalyseWeather(void)
{
	uint16_t i;

	for(i = 0; i < 512; i++)
	{

		if(rx6_buf[i] == 'c' && rx6_buf[i+1] == 'o' && rx6_buf[i+2] == 'd' && rx6_buf[i+3] == 'e' && rx6_buf[i+4] == '_' && rx6_buf[i+5] == 'd')
		{
			if (wea_flag == 0 || wea_flag == 2)
			{
				i += 11;
				while(rx6_buf[i] >= '0' && rx6_buf[i] <= '9')
				{
					weather = weather * 10 + (rx6_buf[i] - '0'); 
//					weather = 1;
					i++;
					if (wea_flag == 0)
						wea_flag = 1;
					if (wea_flag == 2)
						wea_flag = 3;
					if(rx6_buf[i] == '"')
						break;
				}				
			}

		}
		
		if(rx6_buf[i] == 'h' && rx6_buf[i+1] == 'i' && rx6_buf[i+2] == 'g' && rx6_buf[i+3] == 'h')
		{
			if (wea_flag == 0 || wea_flag == 1)
			{
				i += 7;
				while(rx6_buf[i] >= '0' && rx6_buf[i] <= '9')
				{
					temp_high = temp_high * 10 + (rx6_buf[i] - '0'); 
					i++;
					if(rx6_buf[i] == '"')
						break;
				}
				i += 9;				
				while(rx6_buf[i] >= '0' && rx6_buf[i] <= '9')
				{
					temp_low = temp_low * 10 + (rx6_buf[i] - '0'); 
					i++;
					if (wea_flag == 0)
						wea_flag = 2;
					if (wea_flag == 1)
						wea_flag = 3;
					if(rx6_buf[i] == '"')
						break;
				}				
			}		
		}
		if (wea_flag == 3) break;
	}


	printf("\nwea_flag = %d\n", wea_flag);
	printf("\nweather = %d\n",weather);
	printf("\ntemperature_high = %d\n",temp_high);
	printf("\ntemperature_low = %d\n",temp_low);
}

void AnalyseTime(void)
{
	
	uint8_t i, j;
	for (i = 0; i < 255; i++)
	{
		if (rx6_buf[i] == 'T' && rx6_buf[i+1] == 'i' && rx6_buf[i+2] == 'm' && rx6_buf[i+3] == 'e' && rx6_buf[i+4] == '1')
		{
			i += 8;
			for (j = 0; j < 4; j++)
			{
				real_year = real_year * 10 + (rx6_buf[i + j] - '0');
			}
			for (j = 4; j < 6; j++)
			{
				real_month = real_month * 10 + (rx6_buf[i + j] - '0');
			}
			for (j = 6; j < 8; j++)
			{
				real_day = real_day * 10 + (rx6_buf[i + j] - '0');
			}
			for (j = 8; j < 10; j++)
			{
				real_hour = real_hour * 10 + (rx6_buf[i + j] - '0');
			}
			for (j = 10; j < 12; j++)
			{
				real_minute = real_minute * 10 + (rx6_buf[i + j] - '0');
			}	
			for (j = 12; j < 14; j++)
			{
				real_second = real_second * 10 + (rx6_buf[i + j] - '0');
			}			

			printf("real_year = %d\real_month = %d\real_day = %d\real_hour = %d\real_minute = %d\real_second = %d\n", real_year, real_month, real_day, real_hour, real_minute, real_second);
			time_flag = 1;
		}
	}
}

void WeatherDisplay(void)
{
	if (weather <= 3) //����
	{
		if (weathernum != 1)
			GUI_ClearRect(200, 70, 251, 121);
		GUI_DrawBitmap(&bmqingtian, 200, 70);   //����ͼƬλ��  96*96
		weathernum = 1;
	}
	else if (weather >= 4 && weather <= 8 && weathernum != 2) //����
	{
		GUI_ClearRect(200, 70, 260, 117);
		GUI_DrawBitmap(&bmduoyun, 200, 70);
		weathernum = 2;
	}
	else if (weather == 9) //��
	{
		if(weathernum != 3)
			GUI_ClearRect(200, 70, 260, 110);
		GUI_DrawBitmap(&bmyintian, 200, 70);
		weathernum = 3;
	}
	else if (weather >= 10 && weather <= 25) //��
	{
		if(weathernum != 4)
			GUI_ClearRect(200, 70, 256, 124);
		GUI_DrawBitmap(&bmyu, 200, 70);
		weathernum = 4;
	}
	else if (weather >= 30) //����
	{
		if(weathernum != 5)
			GUI_ClearRect(200, 70, 256, 120);
		GUI_DrawBitmap(&bmsmoke, 200, 70);
		weathernum = 5;
	}
	/*else
	{
		if(weathernum != 6)
			GUI_ClearRect(200,50,296,146);
		GUI_DrawBitmap(&bmsmoke,200,50);
		weathernum = 6;
	}*/
}

void pause(void)
{
	Beep(0);
	ReadRTCDateTime();
	GUI_SetBkColor(GUI_CYAN);
	GUI_DrawBitmap(&bmnaoling,220,60);

	if(gx>4500||gy>4500||gz>4500||osKernelGetTickCount()>(times+60000))
	{
		osDelay(900);
		if(gx>4500||gy>4500||gz>4500||osKernelGetTickCount()>(times+60000))
		{
			stop_flag=1;
			beep_flag=0;
		}
	}
	else
	{
		osDelay(50);
	}
	if(osKernelGetTickCount()>(times+60000))
	{
		beep_flag=0;
	}
	GUI_ClearRect(220,60,270,110);
}
uint8_t CheckIfLeapYear(uint16_t year)
{
	if (year % 400 == 0 || (year % 4 ==0 && year % 100 != 0))
		return 1;
	else return 0;
	
}
void ShowWeekByCN(char *buf, uint8_t day)
{
	switch(day)
	{
		case 1:
			strcat(buf, "һ");
			break;
		case 2:
			strcat(buf, "��");
			break;		
		case 3:
			strcat(buf, "��");
			break;
		case 4:
			strcat(buf, "��");
			break;
		case 5:
			strcat(buf, "��");
			break;
		case 6:
			strcat(buf, "��");
			break;
		case 7:
			strcat(buf, "��");
			break;
		default:
			break;	
	}
}

//�������ڼ�
void GetWeekDay(void)
{
	uint16_t mon, year;
	if (RTC_Mon == 1|| RTC_Mon == 2)
	{
		year = RTC_Year - 1;
		mon = RTC_Mon + 12;
	}
	else
	{
		year = RTC_Year;
		mon = RTC_Mon;									
	}
	//��ķ����ɭ��ʽ�����ڼ�
	day = (RTC_Dat + 2 * mon + 3 * (mon + 1) / 5 + year + year / 4 - year / 100 + year / 400 + 1) % 7;
	if(day==0)
	{
		day=7;
	}
	//day:1-7�ֱ��Ӧ����һ��������
}

void DrawLogo(void)
{
	//��������λ��
	uint16_t sw = GUI_GetScreenSizeX();
	uint16_t sh = GUI_GetScreenSizeY();
	GUI_SetBkColor(GUI_LIGHTCYAN);
	GUI_Clear();
	//��ʾУ��
	GUI_DrawBitmap(&bmhdulogo, 0, 164);
//	osDelay(1000);
//	GUI_Clear();
	//��ʾ�γ���������ѧ�ŵ���Ϣ
	GUI_SetFont(&GUI_FontHZ_KaiTi_24AA4);
//	GUI_SetBkColor(GUI_GREEN);
//	GUI_Clear();
	GUI_SetColor(GUI_RED);
	GUI_DispStringHCenterAt("��������ۺ�ʵ��\nѪ�����ʲ�����\n����ɭ 20041411\n����Ȼ 20071213", sw / 2, sh / 2 - 80);
	osDelay(3000);
	GUI_Clear();
	GUI_SetBkColor(GUI_GREEN);
	GUI_Clear();
	//�л���ʱ�ӽ���
	gui_idx_old = gui_idx;
	gui_idx = GUI_CLOCK;
}
void DrawClock(void)
{
	char buf[50];
	uint16_t sw = GUI_GetScreenSizeX();
	times = 0;
	real_temp = ds18b20_read();
	



	
//	BUTTON_Handle  btnUp=BUTTON_CreateEx(
//	40,190,100,40,0,WM_CF_SHOW,0,'U');
//	BUTTON_SetText(btnUp,"set alarm");
	
//	BUTTON_Handle  btnDown=BUTTON_CreateEx(
//	180,190,100,40,0,WM_CF_SHOW,0,'D');
//	BUTTON_SetText(btnDown,"weather");
	
	
	switch (g_bset) //�жϵ�������������ʱ�����е���һ��
	{
		default:
			//���Ķ������
			GUI_SetBkColor(GUI_YELLOW);
			GUI_SetColor(GUI_DARKBLUE);
			GUI_DispStringHCenterAt("ʱ����ʾ", sw / 2, 0);		
			GUI_SetBkColor(GUI_GREEN);				
			sprintf(buf, "%4d��%2d��%2d��  ����", RTC_Year, RTC_Mon, RTC_Dat);
			ShowWeekByCN(buf, day);
			GUI_DispStringAt(buf, 1, 30);
			sprintf(buf, "%2dʱ%2d��%2d��", RTC_Hour, RTC_Min, RTC_Sec);
			GUI_DispStringAt(buf, 1, 60);

		
			if (real_temp < 50 && real_temp != last_temp)
			{
				sprintf(buf, "�¶�:%.0f", real_temp);
				last_temp = real_temp;
			}
			else
			{
				sprintf(buf, "�¶�:%.0f", last_temp);
			}
			GUI_DispStringAt(buf, 1, 90);
			if (wea_flag == 3) //�Ѿ����յ�����
			{
				WeatherDisplay();
				switch (weathernum)
				{
					case 1:sprintf(buf, "ʵʱ����:����"); GUI_DispStringAt(buf, 1, 120); break;
					case 2:sprintf(buf, "ʵʱ����:����"); GUI_DispStringAt(buf, 1, 120); break;
					case 3:sprintf(buf, "ʵʱ����:����"); GUI_DispStringAt(buf, 1, 120); break;
					case 4:sprintf(buf, "ʵʱ����:����"); GUI_DispStringAt(buf, 1, 120); break;
					case 5:sprintf(buf, "ʵʱ����:����"); GUI_DispStringAt(buf, 1, 120); break;
					default:break;
				}
				sprintf(buf, "����¶�:%d  ����¶�:%d", temp_high, temp_low); GUI_DispStringAt(buf, 1, 150);		
			}
			break;
		case 1:	//�޸���
			if (g_bset_old == 0 || gui_idx_old == 2)
			{
				GUI_Clear();
				GUI_SetBkColor(GUI_CYAN);
				g_bset_old = g_bset;
				gui_idx_old = gui_idx;
				GUI_SetColor(GUI_DARKBLUE);
				GUI_SetBkColor(GUI_YELLOW);								
			}
			GUI_DispStringHCenterAt("ʱ������", sw / 2, 0);		
			if (osKernelGetTickCount() % 1000 > 800)
			{
				sprintf(buf, "    ��%2d��%2d��  ����", RTC_Mon, RTC_Dat);
				ShowWeekByCN(buf, day);
			}
			else
			{
				sprintf(buf, "%4d��%2d��%2d��  ����", RTC_Year, RTC_Mon, RTC_Dat);
				ShowWeekByCN(buf, day);
			}
			GUI_DispStringAt(buf, 1, 30);
			sprintf(buf, "%2dʱ%2d��%2d��", RTC_Hour, RTC_Min, RTC_Sec);
			GUI_DispStringAt(buf, 1, 60);
			break;
		case 2: //�޸���
			if (g_bset_old == 0 || gui_idx_old == 2)
			{
				GUI_Clear();
				GUI_SetBkColor(GUI_CYAN);
				g_bset_old = g_bset;
				gui_idx_old = gui_idx;
				GUI_SetColor(GUI_DARKBLUE);
				GUI_SetBkColor(GUI_YELLOW);
				GUI_DispStringHCenterAt("ʱ������", sw / 2, 0);					
			}			
			if (osKernelGetTickCount() % 1000 > 800)
			{
				sprintf(buf, "%4d��  ��%2d��  ����", RTC_Year, RTC_Dat);
				ShowWeekByCN(buf, day);
			}
			else
			{
				sprintf(buf, "%4d��%2d��%2d��  ����", RTC_Year, RTC_Mon, RTC_Dat);
				ShowWeekByCN(buf, day);
			}
			GUI_DispStringAt(buf, 1, 30);
			sprintf(buf, "%2dʱ%2d��%2d��", RTC_Hour, RTC_Min, RTC_Sec);
			GUI_DispStringAt(buf, 1, 60);
			break;
		case 3: //�޸���
			if (g_bset_old == 0 || gui_idx_old == 2)
			{
				GUI_Clear();
				GUI_SetBkColor(GUI_CYAN);
				g_bset_old = g_bset;
				gui_idx_old = gui_idx;
				GUI_SetColor(GUI_DARKBLUE);
				GUI_SetBkColor(GUI_YELLOW);
				GUI_DispStringHCenterAt("ʱ������", sw / 2, 0);					
			}			
			if (osKernelGetTickCount() % 1000 > 800)
			{
				sprintf(buf, "%4d��%2d��  ��  ����", RTC_Year, RTC_Mon);
				ShowWeekByCN(buf, day);
			}
			else
			{
				sprintf(buf, "%4d��%2d��%2d��  ����", RTC_Year, RTC_Mon, RTC_Dat);
				ShowWeekByCN(buf, day);
			}
			GUI_DispStringAt(buf, 1, 30);
			sprintf(buf, "%2dʱ%2d��%2d��", RTC_Hour, RTC_Min, RTC_Sec);
			GUI_DispStringAt(buf, 1, 60);
			break;
		case 4: //�޸�ʱ
			if (g_bset_old == 0 || gui_idx_old == 2)
			{
				GUI_Clear();
				GUI_SetBkColor(GUI_CYAN);
				g_bset_old = g_bset;
				gui_idx_old = gui_idx;
				GUI_SetColor(GUI_DARKBLUE);
				GUI_SetBkColor(GUI_YELLOW);
				GUI_DispStringHCenterAt("ʱ������", sw / 2, 0);					
			}			
			sprintf(buf, "%4d��%2d��%2d��  ����", RTC_Year, RTC_Mon, RTC_Dat);
			ShowWeekByCN(buf, day);
			GUI_DispStringAt(buf, 1, 30);
			if (osKernelGetTickCount() % 1000 > 800)
			{
			sprintf(buf, "  ʱ%2d��%2d��", RTC_Min, RTC_Sec);
			}
			else
			{
				sprintf(buf, "%2dʱ%2d��%2d��", RTC_Hour, RTC_Min, RTC_Sec);
			}
			GUI_DispStringAt(buf, 1, 60);
			break;
		case 5: //�޸ķ�
			if (g_bset_old == 0 || gui_idx_old == 2)
			{
				GUI_Clear();
				GUI_SetBkColor(GUI_CYAN);
				g_bset_old = g_bset;
				gui_idx_old = gui_idx;
				GUI_SetColor(GUI_DARKBLUE);
				GUI_SetBkColor(GUI_YELLOW);
				GUI_DispStringHCenterAt("ʱ������", sw / 2, 0);					
			}			
			sprintf(buf, "%4d��%2d��%2d��  ����", RTC_Year, RTC_Mon, RTC_Dat);
			ShowWeekByCN(buf, day);
			GUI_DispStringAt(buf, 1, 30);
			if (osKernelGetTickCount() % 1000 > 800)
			{
			sprintf(buf, "%2dʱ  ��%2d��", RTC_Hour, RTC_Sec);
			}
			else
			{
				sprintf(buf, "%2dʱ%2d��%2d��", RTC_Hour, RTC_Min, RTC_Sec);
			}
			GUI_DispStringAt(buf, 1, 60);
			break;
		case 6: //�޸���
			if (g_bset_old == 0 || gui_idx_old == 2)
			{
				GUI_Clear();
				GUI_SetBkColor(GUI_CYAN);
				g_bset_old = g_bset;
				gui_idx_old = gui_idx;
				GUI_SetColor(GUI_DARKBLUE);
				GUI_SetBkColor(GUI_YELLOW);
				GUI_DispStringHCenterAt("ʱ������", sw / 2, 0);					
			}		
			sprintf(buf, "%4d��%2d��%2d��  ����", RTC_Year, RTC_Mon, RTC_Dat);
			ShowWeekByCN(buf, day);
			GUI_DispStringAt(buf, 1, 30);
			if (osKernelGetTickCount() % 1000 > 800)
			{
			sprintf(buf, "%2dʱ%2d��  ��", RTC_Hour, RTC_Min);
			}
			else
			{
				sprintf(buf, "%2dʱ%2d��%2d��", RTC_Hour, RTC_Min, RTC_Sec);
			}
			GUI_DispStringAt(buf, 1, 60);
			break;
	}
	switch(GUI_GetKey())
	{
		case 'U':gui_idx=1; set_alarm=1; times=0; stop_flag=0; break;
		case 'D':gui_idx=2;
				 GUI_SetBkColor(GUI_CYAN);
				 GUI_ClearRect(0,0,320,30);
				 GUI_SetBkColor(GUI_BROWN);
				 GUI_SetColor(GUI_LIGHTGRAY);
				 GUI_DispStringHCenterAt("����ģʽ",sw/2,0);
				 GUI_SetBkColor(GUI_CYAN);
				 GUI_ClearRect(0,30,320,180);			
				 set_alarm=0;
				 break;				
		default:break; 
	}

	if (hrAvg > 0 && spo2Avg > 0 && (n_heart_rate < 190) && (n_heart_rate > 40))
	{
		printf("hrAvg:%d spo2Avg:%d n_heart_rate:%d\r\n", hrAvg, spo2Avg, n_heart_rate);
		//�л���Ѫ��������ʾ����
		gui_idx_old = gui_idx;
		gui_idx = GUI_SHOW;
		GUI_SetBkColor(GUI_LIGHTCYAN);
		GUI_Clear();
	}
	
}
void DrawShow(void)
{
	char buf[50];
	uint16_t sw = GUI_GetScreenSizeX();
//	GUI_SetBkColor(GUI_LIGHTCYAN);
//	GUI_Clear();
	GUI_SetBkColor(GUI_YELLOW);
	GUI_SetColor(GUI_DARKBLUE);
	GUI_DispStringHCenterAt("Ѫ��������ʾ����", sw / 2, 0);		
	GUI_SetBkColor(GUI_LIGHTCYAN);
	
//	max30102ReadCalc();
	if (hrAvg > 0 && spo2Avg > 0 && (n_heart_rate < 190) && (n_heart_rate > 40))
	{
		printf("hrAvg:%d spo2Avg:%d n_heart_rate:%d\r\n", hrAvg, spo2Avg, n_heart_rate);
		sprintf(buf, "hrAvg:%d  ", hrAvg);
		GUI_DispStringAt(buf, 1, 30);
		sprintf(buf, "spo2Avg:%d  ", spo2Avg);
		GUI_DispStringAt(buf, 1, 60);
		sprintf(buf, "n_heart_rate:%d  ",n_heart_rate);
		GUI_DispStringAt(buf, 1, 90);
	}	
	else
	{
		sprintf(buf, "hrAvg:   ");
		GUI_DispStringAt(buf, 1, 30);
		sprintf(buf, "spo2Avg:    ");
		GUI_DispStringAt(buf, 1, 60);
		sprintf(buf, "n_heart_rate:   ");
		GUI_DispStringAt(buf, 1, 90);		
		printf(". ");
		//�л���ʱ�ӽ���
		gui_idx_old = gui_idx;
		gui_idx = GUI_CLOCK;
		GUI_SetBkColor(GUI_GREEN);
		GUI_Clear();		
	}
		
    osDelay(1);	
	
//	BUTTON_Handle  btnUp=BUTTON_CreateEx(
//	40,190,100,40,0,WM_CF_SHOW,0,'U');
//	BUTTON_SetText(btnUp,"set alarm");
//	
//	BUTTON_Handle  btnDown=BUTTON_CreateEx(
//	180,190,100,40,0,WM_CF_SHOW,0,'D');
//	BUTTON_SetText(btnDown,"weather");

//	GUI_Exec();
//	TouchProcess();		
}
void DrawData(void)
{
	
}

void KeyDownClock(uint8_t key)
{

	switch (key)
	{
		case UP:	
			if(g_bset) //�ڵ���ʱ��ģʽ��
			{
				switch(g_bset) //�жϵ�������������ʱ�����е���һ��
				{
					default:
						break;
					case 1:
						++RTC_Year;
						//��������2��29���������
						if (RTC_Mon == 2 && CheckIfLeapYear(RTC_Year-1) && RTC_Dat == 29)
							RTC_Dat = 28;
						//��ȡ���ڼ�
						GetWeekDay();
						break;
					case 2:
						if (RTC_Mon < 12) ++RTC_Mon;
						else RTC_Mon = 1;
						//����1��29��30��31�յ��������
						if (RTC_Mon == 2 && (RTC_Dat == 31 || RTC_Dat == 30 || RTC_Dat == 29))
						{
							if (CheckIfLeapYear(RTC_Year)) RTC_Dat = 29;
							else RTC_Dat = 28;							
						}
						//����3��5��8��10��31�յ��������
						if (RTC_Mon == 4 || RTC_Mon == 6 || RTC_Mon == 9 || RTC_Mon == 11)
						{
							if (RTC_Dat == 31)
							{
								RTC_Dat = 30;
							}
						}
						//��ȡ���ڼ�
						GetWeekDay();
						break;
					case 3:
						if (RTC_Dat < 31) ++RTC_Dat;
						else RTC_Dat = 1;
						//���������2��29���Լ�ƽ���2��28�յ��������
						if (RTC_Mon == 2)
						{
							if (CheckIfLeapYear(RTC_Year) && RTC_Dat == 30)
							{
								RTC_Dat = 1;
							}
							if (CheckIfLeapYear(RTC_Year) == 0 && RTC_Dat == 29)
							{
								RTC_Dat = 1;
							}								
						}
						//����4��6��9��10��30�յ��������
						if (RTC_Mon == 4 || RTC_Mon == 6 || RTC_Mon == 9 || RTC_Mon == 10)
						{
							if (RTC_Dat == 31) RTC_Dat = 1;
						}							
						//��ȡ���ڼ�
						GetWeekDay();
						break;
					case 4:
						//����ʱ
						if (RTC_Hour < 23) ++RTC_Hour;
						else RTC_Hour = 0;
						break;
					case 5:
						//������
						if (RTC_Min < 59) ++RTC_Min;
						else RTC_Min = 0;
						break;
					case 6:
						//������
						if (RTC_Sec < 59) ++RTC_Sec;
						else RTC_Sec = 0;
						break;
				}
			}						
			break;
		case DOWN:
			if(g_bset) //�ڵ���ʱ��ģʽ��
			{
				switch(g_bset) //�жϵ�������������ʱ�����е���һ��
				{
					default:
						break;
					case 1:
						--RTC_Year;
						//��������2��29���������
						if (RTC_Mon == 2 && CheckIfLeapYear(RTC_Year+1) && RTC_Dat == 29)
							RTC_Dat = 28;
						//��ȡ���ڼ�
						GetWeekDay();															
						break;
					case 2:
						if(RTC_Mon > 1) --RTC_Mon;
						else RTC_Mon = 12;
						//����3��29��30��31�յ��������
						if (RTC_Mon == 2 && (RTC_Dat == 31 || RTC_Dat == 30 || RTC_Dat == 29))
						{
							if (CheckIfLeapYear(RTC_Year)) RTC_Dat = 29;
							else RTC_Dat = 28;
						}
						//����5��7��10��12��31�յ��������
						if (RTC_Mon == 4 || RTC_Mon == 6 || RTC_Mon == 9 || RTC_Mon == 11)
						{
							if (RTC_Dat == 31)
							{
								RTC_Dat = 30;
							}
						}
						//��ȡ���ڼ�
						GetWeekDay();							
						break;
					case 3:
						if(RTC_Dat > 1) --RTC_Dat;
						else RTC_Dat = 31;
						//���������2��29���Լ�ƽ���2��28�յ��������
						if (RTC_Mon == 2)
						{
							if (CheckIfLeapYear(RTC_Year) && RTC_Dat == 31)
							{
								RTC_Dat = 29;
							}
							if (CheckIfLeapYear(RTC_Year) == 0 && RTC_Dat == 31)
							{
								RTC_Dat = 28;
							}											
						}
						//����4��6��9��10��30�յ��������
						if (RTC_Mon == 4 || RTC_Mon == 6 || RTC_Mon == 9 || RTC_Mon == 10)
						{
							if (RTC_Dat == 31) RTC_Dat = 30;
						}							
						//��ȡ���ڼ�
						GetWeekDay();								
						break;
					case 4:
						if (RTC_Hour > 0) --RTC_Hour;
						else RTC_Hour = 23;
						break;
					case 5:
						if (RTC_Min > 0) --RTC_Min;
						else RTC_Min = 59;
						break;
					case 6:
						if (RTC_Sec > 0) --RTC_Sec;
						else RTC_Sec = 59;
						break;
				}
			}					
			break;
		case LEFT:
				g_bset_old = g_bset;
				if (g_bset)
				{
					if (g_bset > 1) --g_bset;
					else g_bset = 7;
				}
			break;
		case RIGHT:
				g_bset_old = g_bset;
				if (g_bset)
				{
					if (g_bset < 7) ++g_bset;
					else  g_bset = 1;
				}
			break;
		case SWITCH:
				g_bset_old = g_bset;
				if (g_bset)
				{
					SetRTCTime(RTC_Hour, RTC_Min, RTC_Sec);
					SetRTCDate(RTC_Year, RTC_Mon, RTC_Dat);
				}
				else
				{
					old_year = RTC_Year;
					old_mon = RTC_Mon;
					old_day = RTC_Dat;
					old_hour = RTC_Hour;
					old_min = RTC_Min;
					old_sec = RTC_Sec;
				}
				g_bset_old = g_bset;
				//ʱ��������ģʽ�����ģʽ���л�
				g_bset=!g_bset;
			break;
		case FUNC:
			if (g_bset)
			{
				SetRTCTime(old_hour, old_min, old_sec);
				SetRTCDate(old_year, old_mon, old_day);
				g_bset=!g_bset;
			}
			break;
	}	
}
void KeyDownShow(uint8_t key)
{
	
}
void KeyDownData(uint8_t key)
{
	
}



/* USER CODE END Application */

