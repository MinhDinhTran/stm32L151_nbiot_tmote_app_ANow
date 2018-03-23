/**
  *********************************************************************************************************
  * @file    radar_adc.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2017-10-31
  * @brief   
  *********************************************************************************************************
  * @attention
  *			
  *
  *
  *********************************************************************************************************
  */

#include "radar_dac.h"

DAC_HandleTypeDef RADAR_DAC_Handler;										//Radar DAC���

/**********************************************************************************************************
 @Function			void Radar_DAC_Init(void)
 @Description			�״�DAC��ʼ��
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radar_DAC_Init(void)
{
	DAC_ChannelConfTypeDef DAC_ChanelConfig;
	
	/* DAC��ʼ�� */
	RADAR_DAC_Handler.Instance = RADAR_DACx;
	HAL_DAC_Init(&RADAR_DAC_Handler);
	
	/* DACͨ��������� */
	DAC_ChanelConfig.DAC_Trigger = DAC_TRIGGER_SOFTWARE;
	DAC_ChanelConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
	HAL_DAC_ConfigChannel(&RADAR_DAC_Handler, &DAC_ChanelConfig, RADAR_DACx_CHANNEL);
}

/**********************************************************************************************************
 @Function			void Radar_DAC_DeInit(void)
 @Description			�״�DAC��λ
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radar_DAC_DeInit(void)
{
	HAL_DAC_DeInit(&RADAR_DAC_Handler);
}

/********************************************** END OF FLEE **********************************************/
