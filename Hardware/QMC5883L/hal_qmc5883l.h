#ifndef __BSP_QMC5883L_H
#define   __BSP_QMC5883L_H

#include "sys.h"

#define QMC_DRDY_GPIOx					GPIOA
#define QMC_DRDY_PIN					GPIO_PIN_11
#define QMC_DRDY_RCC_GPIO_CLK_ENABLE()		__HAL_RCC_GPIOA_CLK_ENABLE()
#define QMC_DRDY_RCC_GPIO_CLK_DISABLE()		__HAL_RCC_GPIOA_CLK_DISABLE()
#define QMC_DRDY_READ()					HAL_GPIO_ReadPin(QMC_DRDY_GPIOx, QMC_DRDY_PIN)

#define QMC5883L_SLAVE_ADDRESS_W	0x1A										//I2C�ӻ���ַд
#define QMC5883L_SLAVE_ADDRESS_R	0x1B										//I2C�ӻ���ַ��

/* QMC�ڲ���ַ */
#define QMC_DATA_OUT_X_L			0x00										//X�����ݵ�8λ
#define QMC_DATA_OUT_X_H			0x01										//X�����ݸ�8λ
#define QMC_DATA_OUT_Y_L			0x02										//Y�����ݵ�8λ
#define QMC_DATA_OUT_Y_H			0x03										//Y�����ݸ�8λ
#define QMC_DATA_OUT_Z_L			0x04										//Z�����ݵ�8λ
#define QMC_DATA_OUT_Z_H			0x05										//Z�����ݸ�8λ

#define QMC_READY_BIT			0x06										//bit0 = 0 : No new data; bit0 = 1 : have new data.

#define QMC_TEMPERATURE_L		0x07										//�ڲ��¶����ݵ�8λ
#define QMC_TEMPERATURE_H		0x08										//�ڲ��¶����ݸ�8λ

#define QMC5883L_CR1			0x09										//QMC5883L���ƼĴ���1
#define QMC_MODE_STANDBY			(0<<0)									//����
#define QMC_MODE_CONTINOUS		(1<<0)									//��������
#define QMC_RATES_10HZ			(0<<2)									//�����������10Hz
#define QMC_RATES_50HZ			(1<<2)									//�����������50Hz
#define QMC_RATES_100HZ			(2<<2)									//�����������100Hz
#define QMC_RATES_200HZ			(3<<2)									//�����������200Hz
#define QMC_RANGE_2G			(0<<4)									//������Χ +-2��˹
#define QMC_RANGE_8G			(1<<4)									//������Χ +-8��˹
#define QMC_OSR_512				(0<<6)									//�����˲�������,��Ĵ�������������С,������Ӧ���ĸ���
#define QMC_OSR_256				(1<<6)
#define QMC_OSR_128				(2<<6)
#define QMC_OSR_64				(3<<6)

#define QMC5883L_CR2			0x0A										//QMC5883L���ƼĴ���2
#define QMC_INT_ENABLE			(0<<0)									//�����ж�ʹ��
#define QMC_INT_DISABLE			(1<<0)									//�����ж�ʧ��
#define QMC_POINT_ROLL_ENABLE		(1<<6)									//���ݶ�ȡ��ָ���Զ�ƫתʹ��
#define QMC_POINT_ROLL_DISABLE	(0<<6)									//���ݶ�ȡ��ָ���Զ�ƫתʧ��
#define QMC_SOFT_REST			(1<<7)									//�����λ

#define QMC_DEVIATION_MAX		100										//�������ֵ
#define QMC_SAMPLE_TIMES			5										//��������

typedef struct
{
	int16_t X_Now;														//��ǰ�ų�ֵ
	int16_t Y_Now;
	int16_t Z_Now;
}QMC5883L_TypeDef;
extern QMC5883L_TypeDef			Qmc5883lData;								//QMC5883L���ݴ洢��

void QMC5883L_Init(void);												//QMC5883L��ʼ��
void QMC5883L_Drdy_Init(void);											//QMC5883L��������PA11�ߵ�ƽ��ȡ
void QMC5883L_Drdy_DeInit(void);											//QMC5883L��������PA11����ʼ��
void QMC5883L_ReadData(void);												//QMC5883L��ȡ����
void QMC5883L_ClearInsideData(void);										//QMC5883L����ڲ�����ȡ���ݻ���
void QMC5883L_Mode_Selection(u8 mode);										//QMC5883L����ģʽѡ��
void QMC5883L_Rates_Selection(u8 rates);									//QMC5883L�����������ѡ��
void QMC5883L_Range_Selection(u8 range);									//QMC5883L������Χѡ��
void QMC5883L_Osr_Selection(u8 osr);										//QMC5883L�����˲�������ѡ��
void QMC5883L_Interrupt_Selection(u8 interrupt);								//QMC5883L�����ж�ѡ��
void QMC5883L_Softwart_Reset(void);										//QMC5883L��λ

#endif
