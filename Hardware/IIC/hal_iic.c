/**
  *********************************************************************************************************
  * @file    hal_iic.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2017-10-24
  * @brief   HAL IO Simulation IIC
  *********************************************************************************************************
  * @attention
  *			
  *
  *
  *********************************************************************************************************
  */

#include "hal_iic.h"
#include "delay.h"
#include "platform_config.h"

/**********************************************************************************************************
 @Function			void IIC_Init(void)
 @Description			IIC��ʼ��
 @Input				void
 @Return				void
 @attention			I2C1_SCL ----> PB6
					I2C1_SDA ----> PB7
**********************************************************************************************************/
void IIC_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	IIC_SCL_RCC_GPIO_CLK_ENABLE();										//ʹ��GPIOBʱ��
	IIC_SDA_RCC_GPIO_CLK_ENABLE();
	
	/* PB6,7��ʼ������ */
	GPIO_Initure.Pin = IIC_SCL_PIN;										//PB6
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;								//�������
	GPIO_Initure.Pull = GPIO_PULLUP;										//����
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;									//����
	HAL_GPIO_Init(IIC_SCL_GPIOx, &GPIO_Initure);
	
	GPIO_Initure.Pin = IIC_SDA_PIN;										//PB7
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;								//�������
	GPIO_Initure.Pull = GPIO_PULLUP;										//����
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;									//����
	HAL_GPIO_Init(IIC_SDA_GPIOx, &GPIO_Initure);
	
	IIC_SDA(HIGH);
	IIC_SCL(HIGH);
}

/**********************************************************************************************************
 @Function			void IIC_Start(void)
 @Description			����IIC��ʼ�ź�
 @Input				void
 @Return				void
 @attention			SCL : HIGH
					SDA : HIGH -> LOW
**********************************************************************************************************/
void IIC_Start(void)
{
	SDA_OUT();														//sda�����
	IIC_SDA(HIGH);
	IIC_SCL(HIGH);
#ifndef SYSTEMCLOCK
	#error No Define SYSTEMCLOCK!
#else
#if (SYSTEMCLOCK == 4194000)
	__NOP();
#elif (SYSTEMCLOCK == 32000000)
	Delay_US(4);
#else
	#error SYSTEMCLOCK Define Error
#endif
#endif
	IIC_SDA(LOW);														//START : when CLK is high,DATA change form high to low
#ifndef SYSTEMCLOCK
	#error No Define SYSTEMCLOCK!
#else
#if (SYSTEMCLOCK == 4194000)
	__NOP();
#elif (SYSTEMCLOCK == 32000000)
	Delay_US(4);
#else
	#error SYSTEMCLOCK Define Error
#endif
#endif
	IIC_SCL(LOW);														//ǯסI2C����,׼�����ͻ�������� 
}

/**********************************************************************************************************
 @Function			void IIC_Stop(void)
 @Description			����IICֹͣ�ź�
 @Input				void
 @Return				void
 @attention			SCL : HIGH
					SDA : LOW -> HIGH
**********************************************************************************************************/
void IIC_Stop(void)
{
	SDA_OUT();														//sda�����
	IIC_SCL(LOW);
	IIC_SDA(LOW);														//STOP:when CLK is high DATA change form low to high
#ifndef SYSTEMCLOCK
	#error No Define SYSTEMCLOCK!
#else
#if (SYSTEMCLOCK == 4194000)
	__NOP();
#elif (SYSTEMCLOCK == 32000000)
	Delay_US(4);
#else
	#error SYSTEMCLOCK Define Error
#endif
#endif
	IIC_SCL(HIGH); 
	IIC_SDA(HIGH);														//����I2C���߽����ź�
#ifndef SYSTEMCLOCK
	#error No Define SYSTEMCLOCK!
#else
#if (SYSTEMCLOCK == 4194000)
	__NOP();
#elif (SYSTEMCLOCK == 32000000)
	Delay_US(4);
#else
	#error SYSTEMCLOCK Define Error
#endif
#endif
}

/**********************************************************************************************************
 @Function			u8 IIC_Wait_Ack(void)
 @Description			�ȴ�ACK�źŵ���
 @Input				void
 @Return				1 : ����Ӧ��ʧ��
					0 : ����Ӧ��ɹ�
**********************************************************************************************************/
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime = 0;
	
	SDA_IN();															//SDA����Ϊ����
	IIC_SDA(HIGH);__NOP();
	IIC_SCL(HIGH);__NOP();
	while (READ_SDA) {
		ucErrTime++;
		if (ucErrTime > 250) {
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL(LOW);														//ʱ�����0
	return 0;
}

/**********************************************************************************************************
 @Function			void IIC_Ack(void)
 @Description			����ACKӦ��
 @Input				void
 @Return				void
**********************************************************************************************************/
void IIC_Ack(void)
{
	IIC_SCL(LOW);
	SDA_OUT();														//sda�����
	IIC_SDA(LOW);
#ifndef SYSTEMCLOCK
	#error No Define SYSTEMCLOCK!
#else
#if (SYSTEMCLOCK == 4194000)
	__NOP();
#elif (SYSTEMCLOCK == 32000000)
	Delay_US(2);
#else
	#error SYSTEMCLOCK Define Error
#endif
#endif
	IIC_SCL(HIGH);
#ifndef SYSTEMCLOCK
	#error No Define SYSTEMCLOCK!
#else
#if (SYSTEMCLOCK == 4194000)
	__NOP();
#elif (SYSTEMCLOCK == 32000000)
	Delay_US(2);
#else
	#error SYSTEMCLOCK Define Error
#endif
#endif
	IIC_SCL(LOW);
}

/**********************************************************************************************************
 @Function			void IIC_NAck(void)
 @Description			������ACKӦ��
 @Input				void
 @Return				void
**********************************************************************************************************/
void IIC_NAck(void)
{
	SDA_OUT();														//sda�����
	IIC_SCL(LOW);
	IIC_SDA(HIGH);
#ifndef SYSTEMCLOCK
	#error No Define SYSTEMCLOCK!
#else
#if (SYSTEMCLOCK == 4194000)
	__NOP();
#elif (SYSTEMCLOCK == 32000000)
	Delay_US(2);
#else
	#error SYSTEMCLOCK Define Error
#endif
#endif
	IIC_SCL(HIGH);
#ifndef SYSTEMCLOCK
	#error No Define SYSTEMCLOCK!
#else
#if (SYSTEMCLOCK == 4194000)
	__NOP();
#elif (SYSTEMCLOCK == 32000000)
	Delay_US(2);
#else
	#error SYSTEMCLOCK Define Error
#endif
#endif
	IIC_SCL(LOW);
}

/**********************************************************************************************************
 @Function			void IIC_Send_Byte(u8 txd)
 @Description			IIC����һ���ֽ�
 @Input				txd : ��������
 @Return				void
**********************************************************************************************************/
void IIC_Send_Byte(u8 txd)
{
	u8 t;
	
	SDA_OUT();														//sda�����
	IIC_SCL(LOW);														//����ʱ�ӿ�ʼ���ݴ���
	for (t = 0; t < 8; t++)
	{
		IIC_SDA((txd & 0x80) >> 7);
		txd <<= 1;
#ifndef SYSTEMCLOCK
	#error No Define SYSTEMCLOCK!
#else
#if (SYSTEMCLOCK == 4194000)
		__NOP();
#elif (SYSTEMCLOCK == 32000000)
		Delay_US(2);
#else
	#error SYSTEMCLOCK Define Error
#endif
#endif
		IIC_SCL(HIGH);
#ifndef SYSTEMCLOCK
	#error No Define SYSTEMCLOCK!
#else
#if (SYSTEMCLOCK == 4194000)
		__NOP();
#elif (SYSTEMCLOCK == 32000000)
		Delay_US(2);
#else
	#error SYSTEMCLOCK Define Error
#endif
#endif
		IIC_SCL(LOW);
#ifndef SYSTEMCLOCK
	#error No Define SYSTEMCLOCK!
#else
#if (SYSTEMCLOCK == 4194000)
		__NOP();
#elif (SYSTEMCLOCK == 32000000)
		Delay_US(2);
#else
	#error SYSTEMCLOCK Define Error
#endif
#endif
	}
}

/**********************************************************************************************************
 @Function			u8 IIC_Read_Byte(unsigned char ack)
 @Description			��1���ֽ�, ack=1,����ACK, ack=0,����nACK
 @Input				ack
 @Return				��������
**********************************************************************************************************/
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i, receive = 0;
	
	SDA_IN();															//SDA����Ϊ����
	for (i = 0; i < 8; i++)
	{
		IIC_SCL(LOW);
#ifndef SYSTEMCLOCK
	#error No Define SYSTEMCLOCK!
#else
#if (SYSTEMCLOCK == 4194000)
		__NOP();
#elif (SYSTEMCLOCK == 32000000)
		Delay_US(2);
#else
	#error SYSTEMCLOCK Define Error
#endif
#endif
		IIC_SCL(HIGH);
		receive <<= 1;
		if (READ_SDA) receive++;
#ifndef SYSTEMCLOCK
	#error No Define SYSTEMCLOCK!
#else
#if (SYSTEMCLOCK == 4194000)
		__NOP();
#elif (SYSTEMCLOCK == 32000000)
		Delay_US(1);
#else
	#error SYSTEMCLOCK Define Error
#endif
#endif
	}
	if (!ack)
		IIC_NAck();													//����nACK
	else
		IIC_Ack();													//����ACK
	
	return receive;
}

/********************************************** END OF FLEE **********************************************/
