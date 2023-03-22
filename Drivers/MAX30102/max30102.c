#include "max30102.h"
#include "myiic.h"
#include "algorithm.h"

uint32_t aun_ir_buffer[500]; //IR LED sensor data
int32_t n_ir_buffer_length;    //data length
uint32_t aun_red_buffer[500];    //Red LED sensor data
int32_t n_sp02; //SPO2 value
int8_t ch_spo2_valid;   //indicator to show if the SP02 calculation is valid
int32_t n_heart_rate;   //heart rate value
int8_t  ch_hr_valid;    //indicator to show if the heart rate calculation is valid
int32_t hrValidCnt = 0;
int32_t spo2ValidCnt = 0;
int32_t hrThrowOutSamp = 0;
int32_t spo2ThrowOutSamp = 0;
int32_t spo2Timeout = 0;
int32_t hrTimeout = 0;
int32_t hr_buf[16];
int32_t hrSum;
int32_t hrAvg;
int32_t spo2_buf[16];
int32_t spo2Sum;
int32_t spo2Avg;
int32_t spo2BuffFilled;
int32_t hrBuffFilled;

void max30102_reset(void);
uint8_t max30102_Bus_Write(uint8_t Register_Address, uint8_t Word_Data);
uint8_t max30102_Bus_Read(uint8_t Register_Address);
void maxim_max30102_read_fifo(uint32_t *pun_red_led, uint32_t *pun_ir_led);

void max30102_init(void)
{	
	IIC_Init();
	max30102_reset();
	
	max30102_Bus_Write(REG_INTR_ENABLE_1,0xc0);	// INTR setting
	max30102_Bus_Write(REG_INTR_ENABLE_2,0x00);
	max30102_Bus_Write(REG_FIFO_WR_PTR,0x00);  	//FIFO_WR_PTR[4:0]
	max30102_Bus_Write(REG_OVF_COUNTER,0x00);  	//OVF_COUNTER[4:0]
	max30102_Bus_Write(REG_FIFO_RD_PTR,0x00);  	//FIFO_RD_PTR[4:0]
	max30102_Bus_Write(REG_FIFO_CONFIG,0x0f);  	//sample avg = 1, fifo rollover=false, fifo almost full = 17
	max30102_Bus_Write(REG_MODE_CONFIG,0x03);  	//0x02 for Red only, 0x03 for SpO2 mode 0x07 multimode LED
	max30102_Bus_Write(REG_SPO2_CONFIG,0x27);  	// SPO2_ADC range = 4096nA, SPO2 sample rate (100 Hz), LED pulseWidth (400uS)  
	max30102_Bus_Write(REG_LED1_PA,0x3f);   	// Choose value for ~ 12.5mA for LED1
	max30102_Bus_Write(REG_LED2_PA,0x3f);   	// Choose value for ~ 12.5mA for LED2
	max30102_Bus_Write(REG_PILOT_PA,0x7f);   	// Choose value for ~ 25mA for Pilot LED
	
	HAL_Delay(100);
	
	n_ir_buffer_length = 500; //buffer length of 100 stores 5 seconds of samples running at 100sps
	//read the first 500 samples, and determine the signal range
	for(int i = 0; i < n_ir_buffer_length; ++i)
	{
		while(MAX30102_INT);   //wait until the interrupt pin asserts
		maxim_max30102_read_fifo((aun_red_buffer + i), (aun_ir_buffer + i));
	}
	//calculate heart rate and SpO2 after first 500 samples (first 5 seconds of samples)
	maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, 
																					&n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid); 
}

void IIC_WriteBytes(uint8_t WriteAddr, uint8_t* data, uint8_t dataLength)
{		
	uint8_t i;	
  IIC_Start();  

	IIC_Send_Byte(WriteAddr);	    //发送写命令
	IIC_Wait_Ack();
	
	for(i = 0; i < dataLength; ++i)
	{
		IIC_Send_Byte(data[i]);
		IIC_Wait_Ack();
	}				    	   
  
	IIC_Stop();//产生一个停止条件 
	HAL_Delay(10);	 
}

 void IIC_ReadBytes(uint8_t deviceAddr, uint8_t writeAddr, uint8_t* data, uint8_t dataLength)
{		
	uint8_t i;	
  IIC_Start();  

	IIC_Send_Byte(deviceAddr);	    //发送写命令
	IIC_Wait_Ack();
	IIC_Send_Byte(writeAddr);
	IIC_Wait_Ack();
	IIC_Send_Byte(deviceAddr | 0X01);//进入接收模式			   
	IIC_Wait_Ack();
	
	for(i = 0; i < dataLength - 1; ++i)
		data[i] = IIC_Read_Byte(1);

	data[dataLength-1] = IIC_Read_Byte(0);	
  IIC_Stop();//产生一个停止条件 
	HAL_Delay(10);
}

void IIC_Read_One_Byte(uint8_t daddr, uint8_t addr, uint8_t* data)
{				  	  	    																 
  IIC_Start();  
	
	IIC_Send_Byte(daddr);	   //发送写命令
	IIC_Wait_Ack();
	IIC_Send_Byte(addr);//发送地址
	IIC_Wait_Ack();		 
	IIC_Start();  	 	   
	IIC_Send_Byte(daddr | 0X01);//进入接收模式			   
	IIC_Wait_Ack();	 
  *data = IIC_Read_Byte(0);		   
  IIC_Stop();//产生一个停止条件	    
}

void IIC_Write_One_Byte(uint8_t daddr, uint8_t addr, uint8_t data)
{				   	  	    																 
  IIC_Start();  
	
	IIC_Send_Byte(daddr);	    //发送写命令
	IIC_Wait_Ack();
	IIC_Send_Byte(addr);//发送地址
	IIC_Wait_Ack();	   	 										  		   
	IIC_Send_Byte(data);     //发送字节							   
	IIC_Wait_Ack();  		    	   
  IIC_Stop();//产生一个停止条件 
	HAL_Delay(10);
}

void max30102_reset(void)
{
	max30102_Bus_Write(REG_MODE_CONFIG,0x40);
	max30102_Bus_Write(REG_MODE_CONFIG,0x40);
}

uint8_t max30102_Bus_Write(uint8_t Register_Address, uint8_t Word_Data)
{

	/* 采用串行EEPROM随即读取指令序列，连续读取若干字节 */

	/* 第1步：发起I2C总线启动信号 */
	IIC_Start();

	/* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
	IIC_Send_Byte(max30102_WR_address | I2C_WR);	/* 此处是写指令 */

	/* 第3步：发送ACK */
	if (IIC_Wait_Ack() != 0)
	{
		goto cmd_fail;	/* EEPROM器件无应答 */
	}

	/* 第4步：发送字节地址 */
	IIC_Send_Byte(Register_Address);
	if (IIC_Wait_Ack() != 0)
	{
		goto cmd_fail;	/* EEPROM器件无应答 */
	}
	
	/* 第5步：开始写入数据 */
	IIC_Send_Byte(Word_Data);

	/* 第6步：发送ACK */
	if (IIC_Wait_Ack() != 0)
	{
		goto cmd_fail;	/* EEPROM器件无应答 */
	}

	/* 发送I2C总线停止信号 */
	IIC_Stop();
	return 1;	/* 执行成功 */

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
	/* 发送I2C总线停止信号 */
	IIC_Stop();
	return 0;
}



uint8_t max30102_Bus_Read(uint8_t Register_Address)
{
	uint8_t  data;


	/* 第1步：发起I2C总线启动信号 */
	IIC_Start();

	/* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
	IIC_Send_Byte(max30102_WR_address | I2C_WR);	/* 此处是写指令 */

	/* 第3步：发送ACK */
	if (IIC_Wait_Ack() != 0)
	{
		goto cmd_fail;	/* EEPROM器件无应答 */
	}

	/* 第4步：发送字节地址， */
	IIC_Send_Byte((uint8_t)Register_Address);
	if (IIC_Wait_Ack() != 0)
	{
		goto cmd_fail;	/* EEPROM器件无应答 */
	}
	

	/* 第6步：重新启动I2C总线。下面开始读取数据 */
	IIC_Start();

	/* 第7步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
	IIC_Send_Byte(max30102_WR_address | I2C_RD);	/* 此处是读指令 */

	/* 第8步：发送ACK */
	if (IIC_Wait_Ack() != 0)
	{
		goto cmd_fail;	/* EEPROM器件无应答 */
	}

	/* 第9步：读取数据 */
	{
		data = IIC_Read_Byte(0);	/* 读1个字节 */

		IIC_NAck();	/* 最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1) */
	}
	/* 发送I2C总线停止信号 */
	IIC_Stop();
	return data;	/* 执行成功 返回data值 */

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
	/* 发送I2C总线停止信号 */
	IIC_Stop();
	return 0;
}

void maxim_max30102_read_fifo(uint32_t *pun_red_led, uint32_t *pun_ir_led)
{
	uint32_t un_temp;
	unsigned char uch_temp;
	char ach_i2c_data[6];
	*pun_red_led=0;
	*pun_ir_led=0;

  
  //read and clear status register
	max30102_Bus_Read(REG_INTR_STATUS_1);
	max30102_Bus_Read(REG_INTR_STATUS_2);
  
	/* 第1步：发起I2C总线启动信号 */
  IIC_Start();								
	/* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
	IIC_Send_Byte(max30102_WR_address | I2C_WR);	/* 此处是写指令 */
	/* 第3步：发送ACK */
	if (IIC_Wait_Ack() != 0)						
		goto cmd_fail;	/* EEPROM器件无应答 */
	/* 第4步：发送字节地址， */
	IIC_Send_Byte((uint8_t)REG_FIFO_DATA);	 	
	if (IIC_Wait_Ack() != 0)
		goto cmd_fail;	/* EEPROM器件无应答 */

	/* 第6步：重新启动I2C总线。下面开始读取数据 */
	IIC_Start();								
	/* 第7步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
	IIC_Send_Byte(max30102_WR_address | I2C_RD);	/* 此处是读指令 */
	/* 第8步：发送ACK */
	if (IIC_Wait_Ack() != 0)						
		goto cmd_fail;	/* EEPROM器件无应答 */

	/* 第9步：读取数据 */
	un_temp = IIC_Read_Byte(1);
	un_temp <<= 16;
	*pun_red_led += un_temp;
	un_temp = IIC_Read_Byte(1);
	un_temp <<= 8;
	*pun_red_led += un_temp;
	un_temp = IIC_Read_Byte(1);
	*pun_red_led += un_temp;

	// 读取数据
	un_temp = IIC_Read_Byte(1);
	un_temp <<= 16;
	*pun_ir_led += un_temp;
	un_temp = IIC_Read_Byte(1);
	un_temp <<= 8;
	*pun_ir_led += un_temp;
	un_temp = IIC_Read_Byte(0);
	*pun_ir_led += un_temp;
	
	*pun_red_led&=0x03FFFF;  //Mask MSB [23:18]
  *pun_ir_led&=0x03FFFF;  //Mask MSB [23:18]

	/* 最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1) */
	/* 发送I2C总线停止信号 */
	IIC_Stop();  			/* 发送I2C总线停止信号 */
cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
	IIC_Stop(); 			/* 发送I2C总线停止信号 */
}

void max30102ReadCalc(void)
{
	int i;
	//dumping the first 100 sets of samples in the memory and shift the last 400 sets of samples to the top
	for(i = 100; i < 500; ++i)
	{
		aun_red_buffer[i - 100] = aun_red_buffer[i];
		aun_ir_buffer[i - 100] = aun_ir_buffer[i];
	}
	//take 100 sets of samples before calculating the heart rate.
	for(i = 400; i < 500; ++i)
	{
		while(MAX30102_INT == 1);
		maxim_max30102_read_fifo((aun_red_buffer + i), (aun_ir_buffer + i));
	}
	maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, 
																					&n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);
	
	{
		//心率数据处理
		if ((ch_hr_valid == 1) && (n_heart_rate < 190) && (n_heart_rate > 40))//心率数据正常，再处理
		{
			hrTimeout = 0;

			// 如果异常，每5个有效样本中最多扔掉1个
			if (hrValidCnt == 4)
			{
				hrThrowOutSamp = 1;
				hrValidCnt = 0;
				for (i = 12; i < 16; i++)
				{
					if (n_heart_rate < hr_buf[i] + 10)
					{
						hrThrowOutSamp = 0;
						hrValidCnt   = 4;
					}
				}
			}
			else
				++hrValidCnt;

			if (hrThrowOutSamp == 0)
			{
				// 移动新样本到缓冲区
				for(i = 0; i < 15; i++)
					hr_buf[i] = hr_buf[i + 1];
				hr_buf[15] = n_heart_rate;

				// 更新缓冲区填充值
				if (hrBuffFilled < 16)
					++hrBuffFilled;

				// Take moving average
				hrSum = 0;
				if (hrBuffFilled < 2)
					hrAvg = 0;
				else if (hrBuffFilled < 4)
				{
					for(i = 14; i < 16; i++)
						hrSum = hrSum + hr_buf[i];
					hrAvg = hrSum >> 1;
				}
				else if (hrBuffFilled < 8)
				{
					for(i = 12; i < 16; i++)
						hrSum = hrSum + hr_buf[i];
					hrAvg = hrSum >> 2;
				}
				else if (hrBuffFilled < 16)
				{
					for(i = 8; i < 16; i++)
						hrSum = hrSum + hr_buf[i];
					hrAvg = hrSum >> 3;
				}
				else
				{
					for(i = 0; i < 16; i++)
						hrSum = hrSum + hr_buf[i];
					hrAvg = hrSum >> 4;
				}
			}
			hrThrowOutSamp = 0;
		}
		else
		{
			hrValidCnt = 0;
			if (hrTimeout == 4)
			{
				hrAvg = 0;
				hrBuffFilled = 0;
			}
			else
				hrTimeout++;
		}

		//血氧数据处理
		if ((ch_spo2_valid == 1) && (n_sp02 > 59))//血氧的数据正常，再处理
		{
			spo2Timeout = 0;

			//数据异常，舍弃一个
			if (spo2ValidCnt == 4)
			{
				spo2ThrowOutSamp = 1;
				spo2ValidCnt = 0;
				for (i = 12; i < 16; i++)
				{
					if (n_sp02 > spo2_buf[i] - 10)
					{
						spo2ThrowOutSamp = 0;
						spo2ValidCnt   = 4;
					}
				}
			}
			else
				++spo2ValidCnt;
			
			if (spo2ThrowOutSamp == 0)
			{
				// 更新缓冲区
				for(i = 0; i < 15; i++)
					spo2_buf[i] = spo2_buf[i + 1];
				spo2_buf[15] = n_sp02;

				// Update buffer fill value
				if (spo2BuffFilled < 16)
					++spo2BuffFilled;

				// Take moving average
				spo2Sum = 0;
				if (spo2BuffFilled < 2)
					spo2Avg = 0;
				else if (spo2BuffFilled < 4)
				{
					for(i = 14; i < 16; i++)
						spo2Sum = spo2Sum + spo2_buf[i];
					spo2Avg = spo2Sum >> 1;
				}
				else if (spo2BuffFilled < 8)
				{
					for(i = 12; i < 16; i++)
						spo2Sum = spo2Sum + spo2_buf[i];
					spo2Avg = spo2Sum >> 2;
				}
				else if (spo2BuffFilled < 16)
				{
					for(i = 8; i < 16; i++)
						spo2Sum = spo2Sum + spo2_buf[i];
					spo2Avg = spo2Sum >> 3;
				}
				else
				{
					for(i = 0; i < 16; i++)
						spo2Sum = spo2Sum + spo2_buf[i];
					spo2Avg = spo2Sum >> 4;
				}
			}
			spo2ThrowOutSamp = 0;
		}
		else
		{
			spo2ValidCnt = 0;
			if (spo2Timeout == 4)
			{
				spo2Avg = 0;
				spo2BuffFilled = 0;
			}
			else
				++spo2Timeout;
		}
	}
}
