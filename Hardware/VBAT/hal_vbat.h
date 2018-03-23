#ifndef __HAL_VBAT_ADC_H
#define   __HAL_VBAT_ADC_H

#include "sys.h"

#define VBAT_ADC_GPIOx					GPIOB
#define VBAT_ADC_PIN					GPIO_PIN_1
#define VBAT_ADC_RCC_GPIO_CLK_ENABLE()		__HAL_RCC_GPIOB_CLK_ENABLE()
#define VBAT_ADC_RCC_GPIO_CLK_DISABLE()		__HAL_RCC_GPIOB_CLK_DISABLE()
#define VBAT_ADC_RCC_ADC1_CLK_ENABLE()		__HAL_RCC_ADC1_CLK_ENABLE()
#define VBAT_ADC_RCC_ADC1_CLK_DISABLE()		__HAL_RCC_ADC1_CLK_DISABLE()

extern ADC_HandleTypeDef		VBAT_ADC_Handler;								//��ص�ѹADC���

void VBAT_ADC_Init(void);												//��ѹADC��ʼ��
u16  VBAT_ADC_Read(u32 timeout);											//��Դ��ѹADC��ȡ

#endif
