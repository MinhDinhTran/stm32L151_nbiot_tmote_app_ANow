/**
  *********************************************************************************************************
  * @file    stm32l1xx_hal_msp.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2017-11-01
  * @brief   �ص�����
  *********************************************************************************************************
  * @attention
  *			
  *
  *
  *********************************************************************************************************
  */

#include "sys.h"
#include "usart.h"
#include "hal_rtc.h"
#include "hal_vbat.h"
#include "hal_temperature.h"
#include "radar_adc.h"
#include "radar_dac.h"

/**********************************************************************************************************
 @Function			void HAL_MspInit(void)
 @Description			HAL_MspInit
 @Input				void
 @Return				void
**********************************************************************************************************/
void HAL_MspInit(void)
{
	
}

/**********************************************************************************************************
 @Function			void HAL_MspDeInit(void)
 @Description			HAL_MspDeInit
 @Input				void
 @Return				void
**********************************************************************************************************/
void HAL_MspDeInit(void)
{
	
}


/**********************************************************************************************************
 @Function			void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
 @Description			��ʱ���ײ�����, ����ʱ��, �����ж����ȼ�
					�˺����ᱻHAL_TIM_Base_Init()��������
 @Input				htim : TIM���
 @Return				void
**********************************************************************************************************/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM2) {
		__HAL_RCC_TIM2_CLK_ENABLE();										//ʹ��TIM2ʱ��
		HAL_NVIC_SetPriority(TIM2_IRQn, 2, 1);								//�����ж����ȼ�, ǿռ���ȼ�2, �����ȼ�1
		HAL_NVIC_EnableIRQ(TIM2_IRQn);									//����TIM2�ж�
	}
}

/**********************************************************************************************************
 @Function			void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim)
 @Description			��ʱ���ײ�����, �ر�ʱ��
					�˺����ᱻHAL_TIM_Base_DeInit()��������
 @Input				htim : TIM���
 @Return				void
**********************************************************************************************************/
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM2) {
		__HAL_RCC_TIM2_CLK_DISABLE();										//ʧ��TIM2ʱ��
		HAL_NVIC_DisableIRQ(TIM2_IRQn);									//�ر�TIM2�ж�
	}
}


/**********************************************************************************************************
 @Function			void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
 @Description			ADC�ײ�����
					�˺����ᱻHAL_ADC_Init()��������
 @Input				hadc : ADC���
 @Return				void
**********************************************************************************************************/
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	if (hadc == &RADAR_ADC_Handler) {										//�״�ADC
		/* ADC����ʱ��ʹ�� */
		RADAR_ADCx_CLK_ENABLE();
		
		/* �״�ADת��ͨ������ʱ��ʹ�� */
		RADAR_ADCx_CHANNEL_GPIO_CLK_ENABLE();
		
		/* �״�DMAʱ��ʹ�� */
		__HAL_RCC_DMA1_CLK_ENABLE();
		
		/* �״�ADC�������� */
		GPIO_Initure.Pin = RADAR_ADCx_CHANNEL_PIN;
		GPIO_Initure.Mode = GPIO_MODE_ANALOG;
		GPIO_Initure.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(RADAR_ADCx_CHANNEL_GPIO_PORT, &GPIO_Initure);
		
		/* Peripheral DMA init*/
		RADAR_ADC_DMA_Handler.Instance = DMA1_Channel1;
		RADAR_ADC_DMA_Handler.Init.Direction = DMA_PERIPH_TO_MEMORY;
		RADAR_ADC_DMA_Handler.Init.PeriphInc = DMA_PINC_DISABLE;
		RADAR_ADC_DMA_Handler.Init.MemInc = DMA_MINC_ENABLE;
		RADAR_ADC_DMA_Handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
		RADAR_ADC_DMA_Handler.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
		RADAR_ADC_DMA_Handler.Init.Mode = DMA_CIRCULAR;
		RADAR_ADC_DMA_Handler.Init.Priority = DMA_PRIORITY_LOW;
		HAL_DMA_Init(&RADAR_ADC_DMA_Handler);
		
		__HAL_LINKDMA(hadc, DMA_Handle, RADAR_ADC_DMA_Handler);
	}
	
	if (hadc == &VBAT_ADC_Handler) {										//��ص�ѹADC
		/* Enable ADC */
		VBAT_ADC_RCC_ADC1_CLK_ENABLE();
		
		/* Enable GPIO */
		VBAT_ADC_RCC_GPIO_CLK_ENABLE();
		
		/* ADC Pin Configuration */
		GPIO_Initure.Pin = VBAT_ADC_PIN;
		GPIO_Initure.Mode = GPIO_MODE_ANALOG;
		GPIO_Initure.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(VBAT_ADC_GPIOx, &GPIO_Initure);
	}
	
	if (hadc == &TEMPERATURE_ADC_Handler) {
		/* Enable ADC */
		TEMPERATURE_ADC_RCC_ADC1_CLK_ENABLE();
	}
}

/**********************************************************************************************************
 @Function			void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
 @Description			ADC�ײ�����
					�˺����ᱻHAL_ADC_DeInit()��������
 @Input				hadc : ADC���
 @Return				void
**********************************************************************************************************/
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
	if (hadc == &RADAR_ADC_Handler) {										//�״�ADC
		/* Disable ADC */
		RADAR_ADCx_CLK_DISABLE();
		
		/* Deinitialize ADC GPIO */
		HAL_GPIO_DeInit(RADAR_ADCx_CHANNEL_GPIO_PORT, RADAR_ADCx_CHANNEL_PIN);
		
		/* Deinitialize ADC DMA */
		HAL_DMA_DeInit(hadc->DMA_Handle);
	}
	
	if (hadc == &VBAT_ADC_Handler) {										//��ص�ѹADC
		/* Disable ADC */
		VBAT_ADC_RCC_ADC1_CLK_DISABLE();
		
		/* Deinitialize ADC GPIO */
		HAL_GPIO_DeInit(VBAT_ADC_GPIOx, VBAT_ADC_PIN);
		
		/* Deinitialize ADC DMA */
		HAL_DMA_DeInit(hadc->DMA_Handle);
	}
	
	if (hadc == &TEMPERATURE_ADC_Handler) {
		/* Disable ADC */
		TEMPERATURE_ADC_RCC_ADC1_CLK_DISABLE();
	}
}


/**********************************************************************************************************
 @Function			void HAL_DAC_MspInit(DAC_HandleTypeDef* hdac)
 @Description			DAC�ײ�����
					�˺����ᱻHAL_DAC_Init()��������
 @Input				hdac : DAC���
 @Return				void
**********************************************************************************************************/
void HAL_DAC_MspInit(DAC_HandleTypeDef* hdac)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	if (hdac == &RADAR_DAC_Handler) {										//�״�DAC
		/* DAC����ʱ��ʹ�� */
		RADAR_DACx_CLK_ENABLE();
		
		/* DACͨ�����Ŷ˿�ʱ��ʹ�� */
		RADAR_DACx_CHANNEL_GPIO_CLK_ENABLE();
		
		/* DACͨ���������� */
		GPIO_Initure.Pin = RADAR_DACx_CHANNEL_PIN;
		GPIO_Initure.Mode = GPIO_MODE_ANALOG;
		GPIO_Initure.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(RADAR_DACx_CHANNEL_GPIO_PORT, &GPIO_Initure);
	}
}

/**********************************************************************************************************
 @Function			void HAL_DAC_MspDeInit(DAC_HandleTypeDef* hdac)
 @Description			DAC�ײ�����
					�˺����ᱻHAL_DAC_DeInit()��������
 @Input				hdac : DAC���
 @Return				void
**********************************************************************************************************/
void HAL_DAC_MspDeInit(DAC_HandleTypeDef* hdac)
{
	if (hdac == &RADAR_DAC_Handler) {										//�״�DAC
		/* ��λDAC */
		RADAR_DACx_FORCE_RESET();
		RADAR_DACx_RELEASE_RESET();
		
		/* ����DAC����ʱ�� */
		RADAR_DACx_CLK_DISABLE();
		
		/* ����ʼ��DACͨ������ */
		HAL_GPIO_DeInit(RADAR_DACx_CHANNEL_GPIO_PORT, RADAR_DACx_CHANNEL_PIN);
		
		/* DMA����������ʼ�� */
		HAL_DMA_DeInit(hdac->DMA_Handle2);
	}
}


/**********************************************************************************************************
 @Function			void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
 @Description			RTC�ײ�����, ʱ������
					�˺����ᱻHAL_RTC_Init()��������
 @Input				hrtc : RTC���
 @Return				void
**********************************************************************************************************/
void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{
	if (hrtc->Instance == RTC) {
		RCC_OscInitTypeDef RCC_OscInitStruct;
		RCC_PeriphCLKInitTypeDef PeriphCLKInitStruct;
		
		__HAL_RCC_PWR_CLK_ENABLE();										//ʹ�ܵ�Դʱ��PWR
		HAL_PWR_EnableBkUpAccess();										//ȡ����������д����
		
		RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;				//LSE����
		RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;						//DISABLE PLL
		RCC_OscInitStruct.LSEState = RCC_LSE_ON;							//RTCʹ��LSE
		HAL_RCC_OscConfig(&RCC_OscInitStruct);
		
		PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;			//����ΪRTC
		PeriphCLKInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;			//RTCʱ��ԴΪLSE
		HAL_RCCEx_PeriphCLKConfig(&PeriphCLKInitStruct);
		
		__HAL_RCC_RTC_ENABLE();											//RTCʱ��ʹ��
		
		HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 2, 0);
		HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
	}
}

/**********************************************************************************************************
 @Function			void HAL_RTC_MspDeInit(RTC_HandleTypeDef* hrtc)
 @Description			RTC�ײ�����, ʱ�ӹر�
					�˺����ᱻHAL_RTC_DeInit()��������
 @Input				hrtc : RTC���
 @Return				void
**********************************************************************************************************/
void HAL_RTC_MspDeInit(RTC_HandleTypeDef* hrtc)
{
	if (hrtc->Instance == RTC) {
		__HAL_RCC_PWR_CLK_DISABLE();										//ʧ�ܵ�Դʱ��PWR
		__HAL_RCC_RTC_DISABLE();											//RTCʱ��ʧ��
		HAL_NVIC_DisableIRQ(RTC_WKUP_IRQn);
	}
}


/**********************************************************************************************************
 @Function			void HAL_UART_MspInit(UART_HandleTypeDef *huart)
 @Description			UART�ײ��ʼ��, ʱ��ʹ��, ��������, �ж�����
					�˺����ᱻHAL_UART_Init()����
					USART1_TX : PA9
					USART1_RX : PA10
					USART2_TX : PA2
					USART2_RX : PA3
 @Input				huart:���ھ��
 @Return				void
**********************************************************************************************************/
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	if (huart->Instance == USART1)
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();										//ʹ��GPIOAʱ��
		__HAL_RCC_USART1_CLK_ENABLE();									//ʹ��USART1ʱ��
		
		GPIO_Initure.Pin = GPIO_PIN_9 | GPIO_PIN_10;							//PA9|PA10
		GPIO_Initure.Mode = GPIO_MODE_AF_PP;								//�����������
		GPIO_Initure.Pull = GPIO_PULLUP;									//����
		GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;							//����
		GPIO_Initure.Alternate = GPIO_AF7_USART1;							//����Ϊ����1
		HAL_GPIO_Init(GPIOA, &GPIO_Initure);								//��ʼ��PA9|PA10
		
		HAL_NVIC_SetPriority(USART1_IRQn, 1, 0);							//��ռ���ȼ�1, �����ȼ�0
		HAL_NVIC_EnableIRQ(USART1_IRQn);									//ʹ��USART1�ж�ͨ��
	}
	
	if (huart->Instance == USART2)
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();										//ʹ��GPIOAʱ��
		__HAL_RCC_USART2_CLK_ENABLE();									//ʹ��USART2ʱ��
		
		GPIO_Initure.Pin = GPIO_PIN_2 | GPIO_PIN_3;							//PA2|PA3
		GPIO_Initure.Mode = GPIO_MODE_AF_PP;								//�����������
		GPIO_Initure.Pull = GPIO_PULLUP;									//����
		GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;							//����
		GPIO_Initure.Alternate = GPIO_AF7_USART2;							//����Ϊ����2
		HAL_GPIO_Init(GPIOA, &GPIO_Initure);								//��ʼ��PA2|PA3
		
		HAL_NVIC_SetPriority(USART2_IRQn, 1, 1);							//��ռ���ȼ�1, �����ȼ�1
		HAL_NVIC_EnableIRQ(USART2_IRQn);									//ʹ��USART2�ж�ͨ��
	}
}

/**********************************************************************************************************
 @Function			void HAL_UART_MspInit(UART_HandleTypeDef *huart)
 @Description			UART�ײ�ָ���ʼ��, ʱ��ʧ��, ���Ÿ�λ, �жϹر�
					�˺����ᱻHAL_UART_DeInit()����
					USART1_TX : PA9
					USART1_RX : PA10
					USART2_TX : PA2
					USART2_RX : PA3
 @Input				huart:���ھ��
 @Return				void
**********************************************************************************************************/
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	if (huart->Instance == USART1)
	{
		__HAL_RCC_USART1_CLK_DISABLE();									//ʧ��USART1ʱ��
		
		GPIO_Initure.Pin = GPIO_PIN_9 | GPIO_PIN_10;							//PA9|PA10
		GPIO_Initure.Mode = GPIO_MODE_ANALOG;
		GPIO_Initure.Pull = GPIO_NOPULL;
		GPIO_Initure.Speed = GPIO_SPEED_VERY_LOW;
		HAL_GPIO_Init(GPIOA, &GPIO_Initure);								//��ʼ��PA9|PA10
		
		HAL_NVIC_DisableIRQ(USART1_IRQn);									//ʧ��USART1�ж�ͨ��
	}
	
	if (huart->Instance == USART2)
	{
		__HAL_RCC_USART2_CLK_DISABLE();									//ʧ��USART2ʱ��
		
		GPIO_Initure.Pin = GPIO_PIN_2 | GPIO_PIN_3;							//PA2|PA3
		GPIO_Initure.Mode = GPIO_MODE_ANALOG;
		GPIO_Initure.Pull = GPIO_NOPULL;
		GPIO_Initure.Speed = GPIO_SPEED_VERY_LOW;
		HAL_GPIO_Init(GPIOA, &GPIO_Initure);								//��ʼ��PA2|PA3
		
		HAL_NVIC_DisableIRQ(USART2_IRQn);									//ʧ��USART2�ж�ͨ��
	}
}

/********************************************** END OF FLEE **********************************************/
