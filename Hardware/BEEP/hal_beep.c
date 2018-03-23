/**
  *********************************************************************************************************
  * @file    hal_beep.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2017-12-16
  * @brief   
  *********************************************************************************************************
  * @attention
  *
  *
  *
  *********************************************************************************************************
  */

#include "hal_beep.h"
#include "delay.h"

/**********************************************************************************************************
 @Function			void BEEP_Ctrl(u8 beepEnable)
 @Description			����������
 @Input				ON or OFF
 @Return				void
**********************************************************************************************************/
void BEEP_Ctrl(u8 beepEnable)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	BEEP_RCC_GPIO_CLK_ENABLE();
	
	GPIO_Initure.Pin = BEEP_PIN;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Initure.Pull = GPIO_NOPULL;
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;									//����
	HAL_GPIO_Init(BEEP_GPIOx, &GPIO_Initure);								//��ʼ��GPIO
	
	if (beepEnable == ON) {
		HAL_GPIO_WritePin(BEEP_GPIOx, BEEP_PIN, GPIO_PIN_SET);
	}
	else if (beepEnable == OFF) {
		HAL_GPIO_WritePin(BEEP_GPIOx, BEEP_PIN, GPIO_PIN_RESET);
	}
	
	GPIO_Initure.Pin = BEEP_PIN;
	GPIO_Initure.Speed = GPIO_SPEED_VERY_LOW;
	HAL_GPIO_Init(BEEP_GPIOx, &GPIO_Initure);								//��ʼ��GPIO
}

/**********************************************************************************************************
 @Function			void BEEP_CtrlRepeat(u16 nCount, u16 nMs)
 @Description			�����������ظ���
 @Input				nCount	: ����
					nMs		: ���
 @Return				void
**********************************************************************************************************/
void BEEP_CtrlRepeat(u16 nCount, u16 nMs)
{
	u16 index = 0;
	
	for (index = 0; index < nCount; index++) {
		BEEP(ON);
		Delay_MS(nMs);
		BEEP(OFF);
		Delay_MS(nMs);
	}
}

/********************************************** END OF FLEE **********************************************/
