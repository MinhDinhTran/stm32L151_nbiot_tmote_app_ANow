/**
  *********************************************************************************************************
  * @file    hal_qmc5883l.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2017-10-24
  * @brief   HAL QMC5883L Device
  *********************************************************************************************************
  * @attention
  *			
  *
  *
  *********************************************************************************************************
  */

#include "hal_qmc5883l.h"
#include "hal_iic.h"
#include "delay.h"
#include <stdlib.h>

QMC5883L_TypeDef		Qmc5883lData;										//QMC5883L���ݴ洢��

/**********************************************************************************************************
 @Function			static void QMC5883L_WriteByte(u8 ucRegAddr, u8 ucRegData)
 @Description			QMC5883Lд��1Byte���ݵ��Ĵ���( �ڲ����� )
 @Input				ucRegAddr : д���ַ
					ucRegData : д������
 @Return				void
**********************************************************************************************************/
static void QMC5883L_WriteByte(u8 ucRegAddr, u8 ucRegData)
{
	IIC_Start();														//������ʼ�ź�
	
	IIC_Send_Byte(QMC5883L_SLAVE_ADDRESS_W);								//����I2C�ӻ���ַд
	IIC_Wait_Ack();													//�ȴ�Ӧ��
	
	IIC_Send_Byte(ucRegAddr);											//���ʹ�д�����ݵ�ַ
	IIC_Wait_Ack();													//�ȴ�Ӧ��
	
	IIC_Send_Byte(ucRegData);											//������д������
	IIC_Wait_Ack();													//�ȴ�Ӧ��
	
	IIC_Stop();														//����ֹͣ�ź�
}

/**********************************************************************************************************
 @Function			static u8 QMC5883L_ReadByte(u8 ucRegAddr)
 @Description			QMC5883L��ȡ1Byte����( �ڲ����� )
 @Input				ucRegAddr : ��ȡ���ݵ�ַ
 @Return				ucRegAddr : ��ȡ����
**********************************************************************************************************/
static u8 QMC5883L_ReadByte(u8 ucRegAddr)
{
	u8 ucData;
	
	IIC_Start();														//������ʼ�ź�
	
	IIC_Send_Byte(QMC5883L_SLAVE_ADDRESS_W);								//����I2C�ӻ���ַд
	IIC_Wait_Ack();													//�ȴ�Ӧ��
	
	IIC_Send_Byte(ucRegAddr);											//���ʹ���ȡ���ݵ�ַ
	IIC_Wait_Ack();													//�ȴ�Ӧ��
	
	IIC_Start();														//������ʼ�ź�
	
	IIC_Send_Byte(QMC5883L_SLAVE_ADDRESS_R);								//����I2C�ӻ���ַ��
	IIC_Wait_Ack();													//�ȴ�Ӧ��
	
	ucData = IIC_Read_Byte(0);											//��ȡ����
	
	IIC_Stop();														//����ֹͣ�ź�
	
	return ucData;
}

/**********************************************************************************************************
 @Function			void QMC5883L_Drdy_Init(void)
 @Description			QMC5883L��������PA11�ߵ�ƽ��ȡ
 @Input				void
 @Return				void
**********************************************************************************************************/
void QMC5883L_Drdy_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	QMC_DRDY_RCC_GPIO_CLK_ENABLE();										//ʹ��QMC DRDY GPIO ʱ��
	
	GPIO_Initure.Pin = QMC_DRDY_PIN;										//PA11
	GPIO_Initure.Mode = GPIO_MODE_INPUT;									//����
	GPIO_Initure.Pull = GPIO_PULLDOWN;										//����
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;									//����
	HAL_GPIO_Init(QMC_DRDY_GPIOx, &GPIO_Initure);
}

/**********************************************************************************************************
 @Function			void QMC5883L_Drdy_DeInit(void)
 @Description			QMC5883L��������PA11����ʼ��
 @Input				void
 @Return				void
**********************************************************************************************************/
void QMC5883L_Drdy_DeInit(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	QMC_DRDY_RCC_GPIO_CLK_ENABLE();										//ʹ��QMC DRDY GPIO ʱ��
	
	GPIO_Initure.Pin = QMC_DRDY_PIN;										//PA11
	GPIO_Initure.Mode = GPIO_MODE_ANALOG;
	GPIO_Initure.Pull = GPIO_NOPULL;
	GPIO_Initure.Speed = GPIO_SPEED_VERY_LOW;
	HAL_GPIO_Init(QMC_DRDY_GPIOx, &GPIO_Initure);
}

/**********************************************************************************************************
 @Function			void QMC5883L_Init(void)
 @Description			QMC5883L��ʼ��
 @Input				void
 @Return				void
**********************************************************************************************************/
void QMC5883L_Init(void)
{
	IIC_Init();														//IIC��ʼ��
	
	QMC5883L_WriteByte(QMC5883L_CR2, QMC_SOFT_REST);							//��λQMC5883L
	
	QMC5883L_WriteByte(0x0B, 0x01);
	QMC5883L_WriteByte(0x20, 0x40);
	QMC5883L_WriteByte(0x21, 0x01);
	
	/* 64�˲�, 8��˹��Χ, 200Hz���, ��ʼ��ΪStandBy */
	QMC5883L_WriteByte(QMC5883L_CR1, QMC_OSR_64 | QMC_RANGE_8G | QMC_RATES_200HZ | QMC_MODE_STANDBY);
	/* �����ж�ʹ��, ���ݶ�ȡ��ָ���Զ�ƫתʧ�� */
	QMC5883L_WriteByte(QMC5883L_CR2, QMC_INT_ENABLE | QMC_POINT_ROLL_DISABLE);
	
	QMC5883L_Drdy_Init();												//QMC5883L��������PA11�ߵ�ƽ��ȡ
}

/**********************************************************************************************************
 @Function			void QMC5883L_ReadData(void)
 @Description			QMC5883L��ȡ����
 @Input				void
 @Return				void
**********************************************************************************************************/
void QMC5883L_ReadData(void)
{
	u8 ucReadBuf[QMC_SAMPLE_TIMES][6];
	int16_t magdata_x[QMC_SAMPLE_TIMES], magdata_y[QMC_SAMPLE_TIMES], magdata_z[QMC_SAMPLE_TIMES];
	u32 timeover = 0;
	u8 index = 0;
	u8 sample_times = 0;
	
	while (1) {
		if (QMC_DRDY_READ() == 1) {
			for (index = 0; index < 6; index++) {
				ucReadBuf[sample_times][index] = QMC5883L_ReadByte(QMC_DATA_OUT_X_L + index);
			}
			sample_times++;
			timeover = 0;
		}
		else {
			timeover++;
			if (timeover >= 1000) {										//��ʱ����
				break;
			}
		}
		
		if (sample_times >= QMC_SAMPLE_TIMES) {
			for (index = 0; index < QMC_SAMPLE_TIMES; index++) {
				magdata_x[index] = (int16_t)(ucReadBuf[index][1] << 8) | ucReadBuf[index][0];
				magdata_y[index] = (int16_t)(ucReadBuf[index][3] << 8) | ucReadBuf[index][2];
				magdata_z[index] = (int16_t)(ucReadBuf[index][5] << 8) | ucReadBuf[index][4];
			}
			
			for (index = 1; index < QMC_SAMPLE_TIMES; index++) {
				if (abs(magdata_x[0] - magdata_x[index]) > QMC_DEVIATION_MAX) {
					sample_times = 0;
					break;
				}
				else if (abs(magdata_y[0] - magdata_y[index]) > QMC_DEVIATION_MAX) {
					sample_times = 0;
					break;
				}
				else if (abs(magdata_z[0] - magdata_z[index]) > QMC_DEVIATION_MAX) {
					sample_times = 0;
					break;
				}
			}
			break;
		}
		Delay_US(100);
	}
	
	Qmc5883lData.X_Now = 0;
	Qmc5883lData.Y_Now = 0;
	Qmc5883lData.Z_Now = 0;
	
	if (sample_times >= QMC_SAMPLE_TIMES) {
		for (index = 0; index < QMC_SAMPLE_TIMES; index++) {
			Qmc5883lData.X_Now += (((int16_t)(ucReadBuf[index][1] << 8)|ucReadBuf[index][0])+QMC_SAMPLE_TIMES/2)/QMC_SAMPLE_TIMES;
			Qmc5883lData.Y_Now += (((int16_t)(ucReadBuf[index][3] << 8)|ucReadBuf[index][2])+QMC_SAMPLE_TIMES/2)/QMC_SAMPLE_TIMES;
			Qmc5883lData.Z_Now += (((int16_t)(ucReadBuf[index][5] << 8)|ucReadBuf[index][4])+QMC_SAMPLE_TIMES/2)/QMC_SAMPLE_TIMES;
		}
	}
}

/**********************************************************************************************************
 @Function			void QMC5883L_ClearInsideData(void)
 @Description			QMC5883L����ڲ�����ȡ���ݻ���
 @Input				void
 @Return				void
**********************************************************************************************************/
void QMC5883L_ClearInsideData(void)
{
	u8 index = 0;
	
	if (QMC_DRDY_READ() == 1) {
		for (index = 0; index < 6; index++) {
			QMC5883L_ReadByte(QMC_DATA_OUT_X_L + index);
		}
	}
}

/**********************************************************************************************************
 @Function			void QMC5883L_Mode_Selection(u8 mode)
 @Description			QMC5883L����ģʽѡ��
 @Input				mode : QMC_MODE_STANDBY/QMC_MODE_CONTINOUS
 @Return				void
**********************************************************************************************************/
void QMC5883L_Mode_Selection(u8 mode)
{
	u8 reg = 0;
	
	reg = QMC5883L_ReadByte(QMC5883L_CR1);
	if (mode == QMC_MODE_STANDBY) {
		QMC5883L_ClearInsideData();										//Stadbyģʽ�������QMC����ȡ����
		QMC5883L_WriteByte(QMC5883L_CR1, (reg & 0xFC) | QMC_MODE_STANDBY);
	}
	else if (mode == QMC_MODE_CONTINOUS) {
		QMC5883L_WriteByte(QMC5883L_CR1, (reg & 0xFC) | QMC_MODE_CONTINOUS);
	}
}

/**********************************************************************************************************
 @Function			void QMC5883L_Rates_Selection(u8 rates)
 @Description			QMC5883L�����������ѡ��
 @Input				rates : QMC_RATES_10HZ/QMC_RATES_50HZ/QMC_RATES_100HZ/QMC_RATES_200HZ
 @Return				void
**********************************************************************************************************/
void QMC5883L_Rates_Selection(u8 rates)
{
	u8 reg = 0;
	
	reg = QMC5883L_ReadByte(QMC5883L_CR1);
	if (rates == QMC_RATES_10HZ) {
		QMC5883L_WriteByte(QMC5883L_CR1, (reg & 0xF3) | QMC_RATES_10HZ);
	}
	else if (rates == QMC_RATES_50HZ) {
		QMC5883L_WriteByte(QMC5883L_CR1, (reg & 0xF3) | QMC_RATES_50HZ);
	}
	else if (rates == QMC_RATES_100HZ) {
		QMC5883L_WriteByte(QMC5883L_CR1, (reg & 0xF3) | QMC_RATES_100HZ);
	}
	else if (rates == QMC_RATES_200HZ) {
		QMC5883L_WriteByte(QMC5883L_CR1, (reg & 0xF3) | QMC_RATES_200HZ);
	}
}

/**********************************************************************************************************
 @Function			void QMC5883L_Range_Selection(u8 range)
 @Description			QMC5883L������Χѡ��
 @Input				range : QMC_RANGE_2G/QMC_RANGE_8G
 @Return				void
**********************************************************************************************************/
void QMC5883L_Range_Selection(u8 range)
{
	u8 reg = 0;
	
	reg = QMC5883L_ReadByte(QMC5883L_CR1);
	if (range == QMC_RANGE_2G) {
		QMC5883L_WriteByte(QMC5883L_CR1, (reg & 0xCF) | QMC_RANGE_2G);
	}
	else if (range == QMC_RANGE_8G) {
		QMC5883L_WriteByte(QMC5883L_CR1, (reg & 0xCF) | QMC_RANGE_8G);
	}
}

/**********************************************************************************************************
 @Function			void QMC5883L_Osr_Selection(u8 osr)
 @Description			QMC5883L�����˲�������ѡ��
 @Input				osr : QMC_OSR_512/QMC_OSR_256/QMC_OSR_128/QMC_OSR_64
 @Return				void
**********************************************************************************************************/
void QMC5883L_Osr_Selection(u8 osr)
{
	u8 reg = 0;
	
	reg = QMC5883L_ReadByte(QMC5883L_CR1);
	if (osr == QMC_OSR_512) {
		QMC5883L_WriteByte(QMC5883L_CR1, (reg & 0x3F) | QMC_OSR_512);
	}
	else if (osr == QMC_OSR_256) {
		QMC5883L_WriteByte(QMC5883L_CR1, (reg & 0x3F) | QMC_OSR_256);
	}
	else if (osr == QMC_OSR_128) {
		QMC5883L_WriteByte(QMC5883L_CR1, (reg & 0x3F) | QMC_OSR_128);
	}
	else if (osr == QMC_OSR_64) {
		QMC5883L_WriteByte(QMC5883L_CR1, (reg & 0x3F) | QMC_OSR_64);
	}
}

/**********************************************************************************************************
 @Function			void QMC5883L_Interrupt_Selection(u8 interrupt)
 @Description			QMC5883L�����ж�ѡ��
 @Input				interrupt : QMC_INT_ENABLE/QMC_INT_DISABLE
 @Return				void
**********************************************************************************************************/
void QMC5883L_Interrupt_Selection(u8 interrupt)
{
	u8 reg = 0;
	
	reg = QMC5883L_ReadByte(QMC5883L_CR2);
	if (interrupt == QMC_INT_ENABLE) {
		QMC5883L_WriteByte(QMC5883L_CR2, (reg & 0xFE) | QMC_INT_ENABLE);
	}
	else if (interrupt == QMC_INT_DISABLE) {
		QMC5883L_WriteByte(QMC5883L_CR2, (reg & 0xFE) | QMC_INT_DISABLE);
	}
}

/**********************************************************************************************************
 @Function			void QMC5883L_Softwart_Reset(void)
 @Description			QMC5883L��λ
 @Input				void
 @Return				void
**********************************************************************************************************/
void QMC5883L_Softwart_Reset(void)
{
	QMC5883L_WriteByte(QMC5883L_CR2, QMC_SOFT_REST);							//��λQMC5883L
	
	QMC5883L_WriteByte(0x0B, 0x01);
	QMC5883L_WriteByte(0x20, 0x40);
	QMC5883L_WriteByte(0x21, 0x01);
	
	/* 64�˲�, 8��˹��Χ, 10Hz���, ��ʼ��ΪStandBy */
	QMC5883L_WriteByte(QMC5883L_CR1, QMC_OSR_64 | QMC_RANGE_8G | QMC_RATES_10HZ | QMC_MODE_STANDBY);
	/* �����ж�ʹ��, ���ݶ�ȡ��ָ���Զ�ƫתʧ�� */
	QMC5883L_WriteByte(QMC5883L_CR2, QMC_INT_ENABLE | QMC_POINT_ROLL_DISABLE);
}

/********************************************** END OF FLEE **********************************************/
