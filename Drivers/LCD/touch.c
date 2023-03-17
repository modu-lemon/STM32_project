#include "touch.h"
#include "spi.h"
#include "GUI.h"


void GUI_TOUCH_X_ActivateX(void) {}
void GUI_TOUCH_X_ActivateY(void) {}

/****************************************************************************
* ��    �ƣ�void SpiDelay(unsigned int DelayCnt) 
* ��    �ܣ�SPI1 д��ʱ����
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/  
void SpiDelay(unsigned int DelayCnt)
{
// unsigned int i;
// for(i=0;i < DelayCnt;i++);
}
/****************************************************************************
* ��    �ƣ�unsigned char SPI_WriteByte(unsigned char data) 
* ��    �ܣ�SPI1 д����
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/  
unsigned char SPI_WriteByte(unsigned char data) 
{ 
	unsigned char Data = 0; 
  // Send the byte  
  if (HAL_SPI_Transmit(&hspi2, &data, 1, 10) == HAL_OK)
	{
		if (HAL_SPI_Receive(&hspi2, &Data, 1, 10) == HAL_OK)
		{
			return Data; 
		}
	}
	
  // Return the shifted data 
  return Data; 
}  

uint8_t SPI_ReadWriteByte(uint8_t TxData) { 
	uint8_t In_res=0;
	uint8_t Out_res=0;
	In_res = TxData;
	HAL_SPI_TransmitReceive(&hspi2, &In_res, &Out_res, 1, 1); 
	return Out_res; 
}

uint16_t SPI_Flash_ReadID(void)
{
	uint16_t Temp = 0;	  
	TP_CS(0); 		    
	SPI_ReadWriteByte(0x90);//���Ͷ�ȡID����	    
	SPI_ReadWriteByte(0x00); 	    
	SPI_ReadWriteByte(0x00); 	    
	SPI_ReadWriteByte(0x00); 	 			   
	Temp|=SPI_ReadWriteByte(0xFF)<<8;  
	Temp|=SPI_ReadWriteByte(0xFF);	 
	TP_CS(1); 	    
	return Temp;
} 

/****************************************************************************
* ��    �ƣ�uint16_t TPReadX(void) 
* ��    �ܣ�������X�����ݶ���
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/  
uint16_t TPReadY(void)
{ 
   uint16_t x=0;
   TP_CS(0);	            //ѡ��XPT2046 
//   SpiDelay(1);					//��ʱ
   SPI_ReadWriteByte(0xD0);		//����X���ȡ��־
//   SpiDelay(1);					//��ʱ
   x=SPI_ReadWriteByte(0x00);	//������ȡ16λ������ 
   x<<=8;
   x+=SPI_ReadWriteByte(0x00);
//   SpiDelay(1);					//��ֹXPT2046
   TP_CS(1); 					    								  
   x = x>>3;						//��λ�����12λ����Ч����0-4095
   return (x);
}
/****************************************************************************
* ��    �ƣ�uint16_t TPReadX(void)
* ��    �ܣ�������Y�����ݶ���
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/
uint16_t TPReadX(void)
{
   uint16_t y=0;
   TP_CS(0);	                        //ѡ��XPT2046 
//   SpiDelay(1);									//��ʱ
   SPI_ReadWriteByte(0x90);				//����Y���ȡ��־
//   SpiDelay(1);									//��ʱ
   y=SPI_ReadWriteByte(0x00);			//������ȡ16λ������ 
   y<<=8;
   y+=SPI_ReadWriteByte(0x00);
//   SpiDelay(1);					//��ֹXPT2046
   TP_CS(1); 					    								  
   y = y>>3;						//��λ�����12λ����Ч����0-4095
   return (y);
}

int  GUI_TOUCH_X_MeasureX(void) 
{
	unsigned char t=0,t1,count=0;
	unsigned short int databuffer[10]={5,7,9,3,2,6,4,0,3,1};//������
	unsigned short temp=0,X=0;	
 	
	while(/*GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0)==0&&*/count<10)//ѭ������10��
	{	   	  
		databuffer[count]=TPReadX();
		count++; 
	}  
	if(count==10)//һ��Ҫ����10������,������
	{  
	    do//������X��������
		{	
			t1=0;		  
			for(t=0;t<count-1;t++)
			{
				if(databuffer[t]>databuffer[t+1])//��������
				{
					temp=databuffer[t+1];
					databuffer[t+1]=databuffer[t];
					databuffer[t]=temp;
					t1=1; 
				}  
			}
		}while(t1); 	    		 	 		  
		X=(databuffer[3]+databuffer[4]+databuffer[5])/3;	  
	}
	return(X);  
}

int  GUI_TOUCH_X_MeasureY(void) {
  	unsigned char t=0,t1,count=0;
	unsigned short int databuffer[10]={5,7,9,3,2,6,4,0,3,1};//������
	unsigned short temp=0,Y=0;	
 
    while(/*GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0)==0&&*/count<10)	//ѭ������10��
	{	   	  
		databuffer[count]=TPReadY();
		count++;  
	}  
	if(count==10)//һ��Ҫ����10������,������
	{  
	    do//������X��������
		{	
			t1=0;		  
			for(t=0;t<count-1;t++)
			{
				if(databuffer[t]>databuffer[t+1])//��������
				{
					temp=databuffer[t+1];
					databuffer[t+1]=databuffer[t];
					databuffer[t]=temp;
					t1=1; 
				}  
			}
		}while(t1); 	    		 	 		  
		Y=(databuffer[3]+databuffer[4]+databuffer[5])/3;	    
	}
	return(Y); 
}


//Left:3950  Right:200 Up:200  Down:3700
unsigned int TOUCH_X(void)
{
	unsigned int i;
	i=GUI_TOUCH_X_MeasureX();
//	if(i!=4095)
//	printf("x:%d\r\n",i);
	if(i<200||i>3950)return 0;
		else return (3950-i)/11.5;
} 
unsigned int TOUCH_Y(void)
{
	unsigned int i;
	i=GUI_TOUCH_X_MeasureY();
//	printf("y:%d\r\n",i);
	if(i<200||i>3750)return 0;
		else return (3750-i)/15.5;
}
uint8_t Touch_Poll(int32_t *x,int32_t *y)
{
	unsigned int tempf_x,tempf_y;
	if(TOUCH_X()&&TOUCH_Y())
	{
		tempf_x = TOUCH_X();
		tempf_y = TOUCH_Y();
		*x = (int32_t)tempf_x;
		*y = (int32_t)tempf_y;
		return 1;
	}
	else{
		return 0;
	}
}
void TouchProcess(void)
{
	GUI_PID_STATE State;
	unsigned int tempf_x,tempf_y;
	if(TOUCH_X()&&TOUCH_Y())
	{
		tempf_x = TOUCH_X();
		tempf_y = TOUCH_Y();		
		State.x = (int)tempf_x;
		State.y = (int)tempf_y;
		State.Pressed = 1;
		State.Layer = 0;
		GUI_PID_StoreState(&State);	
	}
	else
	{
		State.x = -1;
		State.y = -1;
		State.Pressed = 0;
		State.Layer = 0;	
		GUI_PID_StoreState(&State);		
	}
}
////�������ж�������������
//void Touch_Interrupt_Config(void)
//{
//  GPIO_InitTypeDef  GPIO_InitStructure; 
//  NVIC_InitTypeDef NVIC_InitStructure;
//  EXTI_InitTypeDef EXTI_InitStructure;
//
//
//  //++++++++++�������ж�����+++++++++++
//  // Configure GPIO Pin as input floating 
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(GPIOD, &GPIO_InitStructure);
//
//  // Connect EXTI Line to GPIO Pin
//  GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource8);
//  // Enable the EXTI8 Interrupt //
//  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQChannel;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
//
//  //���������ж�����ΪPD8
//  // Enable the EXTI Line8 Interrupt //
//  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//  EXTI_InitStructure.EXTI_Line = EXTI_Line8;
//  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//  EXTI_Init(&EXTI_InitStructure);
//
//} 

