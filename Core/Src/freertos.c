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
#define GUI_CLOCK  1  //时钟显示界面
#define GUI_SHOW   2  //血氧心率显示界面
#define GUI_DATA   3  //数据同步界面
#define UP         1  //按键上键
#define DOWN       4  //按键下键
#define LEFT 	   2  //按键左键
#define RIGHT 	   3  //按键右键
#define SWITCH	   5  //按键切换键
#define FUNC 	   6  //按键功能键
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

uint8_t rx1_buf[512];	// 串口1接收缓冲     
uint8_t rx6_buf[512];	// 串口6接收缓冲     
uint8_t weather=0;         // 0白天晴  1晚上晴 4多云 5白天晴间多云 6晚上晴间多云 7白天大部分多云 8晚上大部分多云
                           // 9阴 10阵雨  11雷阵雨 12雷阵雨伴有冰雹 13小雨 14中雨 15大雨 16暴雨 17大暴雨 18特大暴雨
													 // 19冻雨 20雨夹雪 21阵雪 22小雪 23中雪 24大雪 25暴雪 26浮尘 27扬沙 28沙尘暴 29强沙尘暴
													 // 30雾 31霾 32风 33大风 34飓风 35热带风暴 36龙卷风 37冷 38热 99未知
uint8_t temp_high = 0;
uint8_t temp_low = 0;

int cid = -1;			// 客户端ID号

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
float real_temp = 0; //当前温度
float last_temp = 0; //上一次温度

//extern const GUI_FONT GUI_FontHZ_KaiTi_24;
extern const GUI_FONT GUI_FontHZ_KaiTi_24AA4;
extern GUI_CONST_STORAGE GUI_BITMAP bmhdulogo;  
extern GUI_CONST_STORAGE GUI_BITMAP bmqingtian;    //0-3晴天
extern GUI_CONST_STORAGE GUI_BITMAP bmduoyun;      //4-8 多云
extern GUI_CONST_STORAGE GUI_BITMAP bmyintian;     //9阴天
extern GUI_CONST_STORAGE GUI_BITMAP bmzhenyu;      //10-12阵雨
extern GUI_CONST_STORAGE GUI_BITMAP bmyu;          //13-19雨
extern GUI_CONST_STORAGE GUI_BITMAP bmxue;         //20-25雪
extern GUI_CONST_STORAGE GUI_BITMAP bmwumai;       //30 31雾霾
extern GUI_CONST_STORAGE GUI_BITMAP bmweizhi;      // 未知
extern GUI_CONST_STORAGE GUI_BITMAP bmbk1; //背景
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

void SendATCmd(char *cmd, int waitms)   // 发送AT指令给串口6
{ 
	if (NULL!=cmd)
	{
		HAL_UART_Transmit_DMA(&huart6,(uint8_t *)cmd,strlen(cmd));	
		osDelay(waitms);  // 延时等待ESP01模块应答时间
	}
}

void SendESP01Str(char *str)     // 发送字符串给串口6
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
					//时钟显示界面
					KeyDownClock(key);
					break;
				case GUI_SHOW:
					//血氧心率显示界面
					KeyDownShow(key);
					break;
				case GUI_DATA:
					//数据同步界面
					KeyDownData(key);
					break;
				default:
					//Logo显示
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
		//获取星期几
		GetWeekDay();		
		switch(gui_idx)
		{
			case GUI_CLOCK:
				//时钟显示界面
				DrawClock();
				break;
			case GUI_SHOW:
				//血氧心率显示界面
				DrawShow();
				break;
			case GUI_DATA:
				//数据同步界面
				DrawData();
				break;
			default:
				//Logo显示
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
	SendATCmd("+++", 500);				// 退出透传模式
	SendATCmd("AT\r\n", 500);			// 测试ESP01模块是否存在
	SendATCmd("ATE0\r\n", 500);			// 关闭模块回显
	SendATCmd("AT+GMR\r\n", 500);		// 查看模块版本信息
	SendATCmd("AT+CWMODE=3\r\n", 500);	// 开启STA+AP模式
	SendATCmd("AT+CIPMUX=0\r\n", 500);	// 关闭多连接
	
	char str[200];
	char weather_web[] = "GET https://api.seniverse.com/v3/weather/daily.json?key=SU8mkZy6Bwvor3FMJ&location=hangzhou&language=en&unit=c&start=0&days=1\r\n";
	char time_web[] = "GET http://quan.suning.com/getSysTime.do\r\n";
	sprintf(str, "AT+CWJAP=\"%s\",\"%s\"\r\n", WIFI_NAME, WIFI_PSW);
	SendATCmd(str, 10000);	// 连接无线路由器或者手机热点，等待10秒
	// 连接时间服务器
	SendATCmd("AT+CIPSTART=\"TCP\",\"quan.suning.com\",80\r\n", 2000);
	SendATCmd("AT+CIPMODE=1\r\n", 500);	// 开启透传模式
	SendATCmd("AT+CIPSEND\r\n", 500);	// 开始透传	
	SendATCmd(time_web, 3000);
	SendATCmd("+++", 500);				// 退出透传模式
	
	
	
	SendATCmd("AT+RST", 500);
	SendATCmd("AT\r\n", 500);			// 测试ESP01模块是否存在
	SendATCmd("ATE0\r\n", 500);			// 关闭模块回显
	SendATCmd("AT+GMR\r\n", 500);		// 查看模块版本信息
	SendATCmd("AT+CWMODE=3\r\n", 500);	// 开启STA+AP模式
	SendATCmd("AT+CIPMUX=0\r\n", 500);	// 关闭多连接
	
	sprintf(str, "AT+CWJAP=\"%s\",\"%s\"\r\n", WIFI_NAME, WIFI_PSW);
	SendATCmd(str, 10000);	// 连接无线路由器或者手机热点，等待10秒
	
	for(;;)
	{
		if (wea_flag != 3)
		{
			if (first_connect == 0)
			{
				// 连接OneNET的HTTP服务器
				SendATCmd("AT+CIPSTART=\"TCP\",\"116.62.81.138\",80\r\n", 2000);
				printf("connet weather server!!");
				SendATCmd("AT+CIPMODE=1\r\n", 500);	// 开启透传模式
				SendATCmd("AT+CIPSEND\r\n", 500);	// 开始透传		
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
  uint8_t dat[512];  // 临时数组
  for(;;) {
	  
	// 串口1接收数据处理
	if (osMessageQueueGet(QueueUart1Handle, dat, NULL, 10) == osOK) {
		printf("UART1: %s", dat);  // 串口1打印回显
      HAL_UART_Transmit_DMA(&huart6, dat, strlen((char *)dat));	// 转发串口6
		}

		// 串口6接收数据处理
	   	if (osMessageQueueGet(QueueUart2Handle, dat, NULL, 10) == osOK) {
			printf("UART6: %s", dat);	// 串口1打印显示
		 if((rx6_buf[0] == '{' && rx6_buf[2] == 'r') || (rx6_buf[0] == '{' && rx6_buf[2] == 's'))
		 {
			 
				for(i = 0; i < 200; i++)
				{
					printf("%d",rx6_buf[i]);
				}
				
				//AnalyseWeather();
		 }

			
			char *str = (char *)dat;
			if (strstr(str, "CONNECT") > str) // 如果有客户端连接进来，取ID号
				cid = atoi(str);
			if (strstr(str, "CLOSED") > str)  // 如果有客户端连接断开，关闭ID号
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
	if (huart == &huart1) { // 串口1接收数据处理
	  HAL_UART_DMAStop(huart);  // 停止DMA请求
	  rx1_buf[Size] = '\0';      // 末尾加字符串结束符
	  osMessageQueuePut(QueueUart1Handle, rx1_buf, NULL, 0);  // 发送消息到队列1
	  // 重启串口1的DMA接收
	  __HAL_UNLOCK(huart);
	  HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx1_buf, sizeof(rx1_buf));
	}
	if (huart == &huart6) { // 串口6接收数据处理
		 HAL_UART_DMAStop(huart);  // 停止DMA请求
		 rx6_buf[Size] = '\0';      // 末尾加字符串结束符
		 osMessageQueuePut(QueueUart2Handle, rx6_buf, NULL, 0); // 发送消息到队列2
		 // 重启串口6的DMA接收
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
	if (weather <= 3) //晴天
	{
		if (weathernum != 1)
			GUI_ClearRect(200, 70, 251, 121);
		GUI_DrawBitmap(&bmqingtian, 200, 70);   //天气图片位置  96*96
		weathernum = 1;
	}
	else if (weather >= 4 && weather <= 8 && weathernum != 2) //多云
	{
		GUI_ClearRect(200, 70, 260, 117);
		GUI_DrawBitmap(&bmduoyun, 200, 70);
		weathernum = 2;
	}
	else if (weather == 9) //阴
	{
		if(weathernum != 3)
			GUI_ClearRect(200, 70, 260, 110);
		GUI_DrawBitmap(&bmyintian, 200, 70);
		weathernum = 3;
	}
	else if (weather >= 10 && weather <= 25) //雨
	{
		if(weathernum != 4)
			GUI_ClearRect(200, 70, 256, 124);
		GUI_DrawBitmap(&bmyu, 200, 70);
		weathernum = 4;
	}
	else if (weather >= 30) //雾霾
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
			strcat(buf, "一");
			break;
		case 2:
			strcat(buf, "二");
			break;		
		case 3:
			strcat(buf, "三");
			break;
		case 4:
			strcat(buf, "四");
			break;
		case 5:
			strcat(buf, "五");
			break;
		case 6:
			strcat(buf, "六");
			break;
		case 7:
			strcat(buf, "日");
			break;
		default:
			break;	
	}
}

//计算星期几
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
	//吉姆拉尔森公式算星期几
	day = (RTC_Dat + 2 * mon + 3 * (mon + 1) / 5 + year + year / 4 - year / 100 + year / 400 + 1) % 7;
	if(day==0)
	{
		day=7;
	}
	//day:1-7分别对应星期一到星期日
}

void DrawLogo(void)
{
	//计算中心位置
	uint16_t sw = GUI_GetScreenSizeX();
	uint16_t sh = GUI_GetScreenSizeY();
	GUI_SetBkColor(GUI_LIGHTCYAN);
	GUI_Clear();
	//显示校徽
	GUI_DrawBitmap(&bmhdulogo, 0, 164);
//	osDelay(1000);
//	GUI_Clear();
	//显示课程名、姓名学号等信息
	GUI_SetFont(&GUI_FontHZ_KaiTi_24AA4);
//	GUI_SetBkColor(GUI_GREEN);
//	GUI_Clear();
	GUI_SetColor(GUI_RED);
	GUI_DispStringHCenterAt("电子设计综合实践\n血氧心率测试仪\n陈泽森 20041411\n丁羿然 20071213", sw / 2, sh / 2 - 80);
	osDelay(3000);
	GUI_Clear();
	GUI_SetBkColor(GUI_GREEN);
	GUI_Clear();
	//切换至时钟界面
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
	
	
	switch (g_bset) //判断调整的是年月日时分秒中的哪一个
	{
		default:
			//不改动的情况
			GUI_SetBkColor(GUI_YELLOW);
			GUI_SetColor(GUI_DARKBLUE);
			GUI_DispStringHCenterAt("时钟显示", sw / 2, 0);		
			GUI_SetBkColor(GUI_GREEN);				
			sprintf(buf, "%4d年%2d月%2d日  星期", RTC_Year, RTC_Mon, RTC_Dat);
			ShowWeekByCN(buf, day);
			GUI_DispStringAt(buf, 1, 30);
			sprintf(buf, "%2d时%2d分%2d秒", RTC_Hour, RTC_Min, RTC_Sec);
			GUI_DispStringAt(buf, 1, 60);

		
			if (real_temp < 50 && real_temp != last_temp)
			{
				sprintf(buf, "温度:%.0f", real_temp);
				last_temp = real_temp;
			}
			else
			{
				sprintf(buf, "温度:%.0f", last_temp);
			}
			GUI_DispStringAt(buf, 1, 90);
			if (wea_flag == 3) //已经接收到天气
			{
				WeatherDisplay();
				switch (weathernum)
				{
					case 1:sprintf(buf, "实时天气:晴天"); GUI_DispStringAt(buf, 1, 120); break;
					case 2:sprintf(buf, "实时天气:多云"); GUI_DispStringAt(buf, 1, 120); break;
					case 3:sprintf(buf, "实时天气:阴天"); GUI_DispStringAt(buf, 1, 120); break;
					case 4:sprintf(buf, "实时天气:阵雨"); GUI_DispStringAt(buf, 1, 120); break;
					case 5:sprintf(buf, "实时天气:雾霾"); GUI_DispStringAt(buf, 1, 120); break;
					default:break;
				}
				sprintf(buf, "最高温度:%d  最低温度:%d", temp_high, temp_low); GUI_DispStringAt(buf, 1, 150);		
			}
			break;
		case 1:	//修改年
			if (g_bset_old == 0 || gui_idx_old == 2)
			{
				GUI_Clear();
				GUI_SetBkColor(GUI_CYAN);
				g_bset_old = g_bset;
				gui_idx_old = gui_idx;
				GUI_SetColor(GUI_DARKBLUE);
				GUI_SetBkColor(GUI_YELLOW);								
			}
			GUI_DispStringHCenterAt("时钟设置", sw / 2, 0);		
			if (osKernelGetTickCount() % 1000 > 800)
			{
				sprintf(buf, "    年%2d月%2d日  星期", RTC_Mon, RTC_Dat);
				ShowWeekByCN(buf, day);
			}
			else
			{
				sprintf(buf, "%4d年%2d月%2d日  星期", RTC_Year, RTC_Mon, RTC_Dat);
				ShowWeekByCN(buf, day);
			}
			GUI_DispStringAt(buf, 1, 30);
			sprintf(buf, "%2d时%2d分%2d秒", RTC_Hour, RTC_Min, RTC_Sec);
			GUI_DispStringAt(buf, 1, 60);
			break;
		case 2: //修改月
			if (g_bset_old == 0 || gui_idx_old == 2)
			{
				GUI_Clear();
				GUI_SetBkColor(GUI_CYAN);
				g_bset_old = g_bset;
				gui_idx_old = gui_idx;
				GUI_SetColor(GUI_DARKBLUE);
				GUI_SetBkColor(GUI_YELLOW);
				GUI_DispStringHCenterAt("时钟设置", sw / 2, 0);					
			}			
			if (osKernelGetTickCount() % 1000 > 800)
			{
				sprintf(buf, "%4d年  月%2d日  星期", RTC_Year, RTC_Dat);
				ShowWeekByCN(buf, day);
			}
			else
			{
				sprintf(buf, "%4d年%2d月%2d日  星期", RTC_Year, RTC_Mon, RTC_Dat);
				ShowWeekByCN(buf, day);
			}
			GUI_DispStringAt(buf, 1, 30);
			sprintf(buf, "%2d时%2d分%2d秒", RTC_Hour, RTC_Min, RTC_Sec);
			GUI_DispStringAt(buf, 1, 60);
			break;
		case 3: //修改日
			if (g_bset_old == 0 || gui_idx_old == 2)
			{
				GUI_Clear();
				GUI_SetBkColor(GUI_CYAN);
				g_bset_old = g_bset;
				gui_idx_old = gui_idx;
				GUI_SetColor(GUI_DARKBLUE);
				GUI_SetBkColor(GUI_YELLOW);
				GUI_DispStringHCenterAt("时钟设置", sw / 2, 0);					
			}			
			if (osKernelGetTickCount() % 1000 > 800)
			{
				sprintf(buf, "%4d年%2d月  日  星期", RTC_Year, RTC_Mon);
				ShowWeekByCN(buf, day);
			}
			else
			{
				sprintf(buf, "%4d年%2d月%2d日  星期", RTC_Year, RTC_Mon, RTC_Dat);
				ShowWeekByCN(buf, day);
			}
			GUI_DispStringAt(buf, 1, 30);
			sprintf(buf, "%2d时%2d分%2d秒", RTC_Hour, RTC_Min, RTC_Sec);
			GUI_DispStringAt(buf, 1, 60);
			break;
		case 4: //修改时
			if (g_bset_old == 0 || gui_idx_old == 2)
			{
				GUI_Clear();
				GUI_SetBkColor(GUI_CYAN);
				g_bset_old = g_bset;
				gui_idx_old = gui_idx;
				GUI_SetColor(GUI_DARKBLUE);
				GUI_SetBkColor(GUI_YELLOW);
				GUI_DispStringHCenterAt("时钟设置", sw / 2, 0);					
			}			
			sprintf(buf, "%4d年%2d月%2d日  星期", RTC_Year, RTC_Mon, RTC_Dat);
			ShowWeekByCN(buf, day);
			GUI_DispStringAt(buf, 1, 30);
			if (osKernelGetTickCount() % 1000 > 800)
			{
			sprintf(buf, "  时%2d分%2d秒", RTC_Min, RTC_Sec);
			}
			else
			{
				sprintf(buf, "%2d时%2d分%2d秒", RTC_Hour, RTC_Min, RTC_Sec);
			}
			GUI_DispStringAt(buf, 1, 60);
			break;
		case 5: //修改分
			if (g_bset_old == 0 || gui_idx_old == 2)
			{
				GUI_Clear();
				GUI_SetBkColor(GUI_CYAN);
				g_bset_old = g_bset;
				gui_idx_old = gui_idx;
				GUI_SetColor(GUI_DARKBLUE);
				GUI_SetBkColor(GUI_YELLOW);
				GUI_DispStringHCenterAt("时钟设置", sw / 2, 0);					
			}			
			sprintf(buf, "%4d年%2d月%2d日  星期", RTC_Year, RTC_Mon, RTC_Dat);
			ShowWeekByCN(buf, day);
			GUI_DispStringAt(buf, 1, 30);
			if (osKernelGetTickCount() % 1000 > 800)
			{
			sprintf(buf, "%2d时  分%2d秒", RTC_Hour, RTC_Sec);
			}
			else
			{
				sprintf(buf, "%2d时%2d分%2d秒", RTC_Hour, RTC_Min, RTC_Sec);
			}
			GUI_DispStringAt(buf, 1, 60);
			break;
		case 6: //修改秒
			if (g_bset_old == 0 || gui_idx_old == 2)
			{
				GUI_Clear();
				GUI_SetBkColor(GUI_CYAN);
				g_bset_old = g_bset;
				gui_idx_old = gui_idx;
				GUI_SetColor(GUI_DARKBLUE);
				GUI_SetBkColor(GUI_YELLOW);
				GUI_DispStringHCenterAt("时钟设置", sw / 2, 0);					
			}		
			sprintf(buf, "%4d年%2d月%2d日  星期", RTC_Year, RTC_Mon, RTC_Dat);
			ShowWeekByCN(buf, day);
			GUI_DispStringAt(buf, 1, 30);
			if (osKernelGetTickCount() % 1000 > 800)
			{
			sprintf(buf, "%2d时%2d分  秒", RTC_Hour, RTC_Min);
			}
			else
			{
				sprintf(buf, "%2d时%2d分%2d秒", RTC_Hour, RTC_Min, RTC_Sec);
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
				 GUI_DispStringHCenterAt("天气模式",sw/2,0);
				 GUI_SetBkColor(GUI_CYAN);
				 GUI_ClearRect(0,30,320,180);			
				 set_alarm=0;
				 break;				
		default:break; 
	}

	if (hrAvg > 0 && spo2Avg > 0 && (n_heart_rate < 190) && (n_heart_rate > 40))
	{
		printf("hrAvg:%d spo2Avg:%d n_heart_rate:%d\r\n", hrAvg, spo2Avg, n_heart_rate);
		//切换至血氧心率显示界面
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
	GUI_DispStringHCenterAt("血氧心率显示界面", sw / 2, 0);		
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
		//切换至时钟界面
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
			if(g_bset) //在调整时间模式下
			{
				switch(g_bset) //判断调整的是年月日时分秒中的哪一个
				{
					default:
						break;
					case 1:
						++RTC_Year;
						//考虑闰年2月29日特殊情况
						if (RTC_Mon == 2 && CheckIfLeapYear(RTC_Year-1) && RTC_Dat == 29)
							RTC_Dat = 28;
						//获取星期几
						GetWeekDay();
						break;
					case 2:
						if (RTC_Mon < 12) ++RTC_Mon;
						else RTC_Mon = 1;
						//考虑1月29、30、31日的特殊情况
						if (RTC_Mon == 2 && (RTC_Dat == 31 || RTC_Dat == 30 || RTC_Dat == 29))
						{
							if (CheckIfLeapYear(RTC_Year)) RTC_Dat = 29;
							else RTC_Dat = 28;							
						}
						//考虑3、5、8、10月31日的特殊情况
						if (RTC_Mon == 4 || RTC_Mon == 6 || RTC_Mon == 9 || RTC_Mon == 11)
						{
							if (RTC_Dat == 31)
							{
								RTC_Dat = 30;
							}
						}
						//获取星期几
						GetWeekDay();
						break;
					case 3:
						if (RTC_Dat < 31) ++RTC_Dat;
						else RTC_Dat = 1;
						//考虑闰年的2月29日以及平年的2月28日的特殊情况
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
						//考虑4、6、9、10月30日的特殊情况
						if (RTC_Mon == 4 || RTC_Mon == 6 || RTC_Mon == 9 || RTC_Mon == 10)
						{
							if (RTC_Dat == 31) RTC_Dat = 1;
						}							
						//获取星期几
						GetWeekDay();
						break;
					case 4:
						//调整时
						if (RTC_Hour < 23) ++RTC_Hour;
						else RTC_Hour = 0;
						break;
					case 5:
						//调整分
						if (RTC_Min < 59) ++RTC_Min;
						else RTC_Min = 0;
						break;
					case 6:
						//调整秒
						if (RTC_Sec < 59) ++RTC_Sec;
						else RTC_Sec = 0;
						break;
				}
			}						
			break;
		case DOWN:
			if(g_bset) //在调整时间模式下
			{
				switch(g_bset) //判断调整的是年月日时分秒中的哪一个
				{
					default:
						break;
					case 1:
						--RTC_Year;
						//考虑闰年2月29日特殊情况
						if (RTC_Mon == 2 && CheckIfLeapYear(RTC_Year+1) && RTC_Dat == 29)
							RTC_Dat = 28;
						//获取星期几
						GetWeekDay();															
						break;
					case 2:
						if(RTC_Mon > 1) --RTC_Mon;
						else RTC_Mon = 12;
						//考虑3月29、30、31日的特殊情况
						if (RTC_Mon == 2 && (RTC_Dat == 31 || RTC_Dat == 30 || RTC_Dat == 29))
						{
							if (CheckIfLeapYear(RTC_Year)) RTC_Dat = 29;
							else RTC_Dat = 28;
						}
						//考虑5、7、10、12月31日的特殊情况
						if (RTC_Mon == 4 || RTC_Mon == 6 || RTC_Mon == 9 || RTC_Mon == 11)
						{
							if (RTC_Dat == 31)
							{
								RTC_Dat = 30;
							}
						}
						//获取星期几
						GetWeekDay();							
						break;
					case 3:
						if(RTC_Dat > 1) --RTC_Dat;
						else RTC_Dat = 31;
						//考虑闰年的2月29日以及平年的2月28日的特殊情况
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
						//考虑4、6、9、10月30日的特殊情况
						if (RTC_Mon == 4 || RTC_Mon == 6 || RTC_Mon == 9 || RTC_Mon == 10)
						{
							if (RTC_Dat == 31) RTC_Dat = 30;
						}							
						//获取星期几
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
				//时钟正常走模式与调整模式的切换
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

