/**
  *********************************************************************************************************
  * @file    stm32l1xx_config.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2017-11-01
  * @brief   
  *********************************************************************************************************
  * @attention
  *			
  *
  *
  *********************************************************************************************************
  */

#include "stm32l1xx_config.h"

/**********************************************************************************************************
 @Function			RCC_RESET_FLAG_TypeDef RCC_ResetFlag_GetStatus(void)
 @Description			RCC_ResetFlag_GetStatus	: ��ȡ��λ��־λ
 @Input				void
 @Return				RCC_RESET_FLAG_TypeDef	: ��־λ
**********************************************************************************************************/
RCC_RESET_FLAG_TypeDef RCC_ResetFlag_GetStatus(void)
{
	RCC_RESET_FLAG_TypeDef ResetStatus = RCC_RESET_FLAG_NONE;
	
	if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET) {
		__HAL_RCC_CLEAR_RESET_FLAGS();
		ResetStatus = RCC_RESET_FLAG_IWDGRST;
	}
	else if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST) != RESET) {
		__HAL_RCC_CLEAR_RESET_FLAGS();
		ResetStatus = RCC_RESET_FLAG_SFTRST;
	}
	else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET) {
		__HAL_RCC_CLEAR_RESET_FLAGS();
		ResetStatus = RCC_RESET_FLAG_PORRST;
	}
	else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET) {
		__HAL_RCC_CLEAR_RESET_FLAGS();
		ResetStatus = RCC_RESET_FLAG_PINRST;
	}
	else if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST) != RESET) {
		__HAL_RCC_CLEAR_RESET_FLAGS();
		ResetStatus = RCC_RESET_FLAG_LPWRRST;
	}
	else if (__HAL_RCC_GET_FLAG(RCC_FLAG_OBLRST) != RESET) {
		__HAL_RCC_CLEAR_RESET_FLAGS();
		ResetStatus = RCC_RESET_FLAG_OBLRST;
	}
	else if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST) != RESET) {
		__HAL_RCC_CLEAR_RESET_FLAGS();
		ResetStatus = RCC_RESET_FLAG_WWDGRST;
	}
	
	return ResetStatus;
}

/**********************************************************************************************************
 @Function			void PowerCtrlIO_Init(void)
 @Description			�͹��Ŀ���IO��ʼ��
 @Input				void
 @Return				void
**********************************************************************************************************/
void LowPowerCtrlIO_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	
	GPIO_Initure.Pin = GPIOA_PIN;
	GPIO_Initure.Mode = GPIO_MODE_ANALOG;
	GPIO_Initure.Pull = GPIO_NOPULL;
	GPIO_Initure.Speed = GPIO_SPEED_VERY_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_Initure);
	
	GPIO_Initure.Pin = GPIOB_PIN;
	HAL_GPIO_Init(GPIOB, &GPIO_Initure);
	
	GPIO_Initure.Pin = GPIOC_PIN;
	HAL_GPIO_Init(GPIOC, &GPIO_Initure);
	
	GPIO_Initure.Pin = GPIOD_PIN;
	HAL_GPIO_Init(GPIOD, &GPIO_Initure);
	
	GPIO_Initure.Pin = GPIOE_PIN;
	HAL_GPIO_Init(GPIOE, &GPIO_Initure);
	
	GPIO_Initure.Pin = GPIOH_PIN;
	HAL_GPIO_Init(GPIOH, &GPIO_Initure);
	
	GPIO_Initure.Pin = GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Initure.Pull = GPIO_PULLDOWN;
	GPIO_Initure.Speed = GPIO_SPEED_VERY_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_Initure);
	
	__HAL_RCC_GPIOA_CLK_DISABLE();
	__HAL_RCC_GPIOB_CLK_DISABLE();
	__HAL_RCC_GPIOC_CLK_DISABLE();
	__HAL_RCC_GPIOD_CLK_DISABLE();
	__HAL_RCC_GPIOE_CLK_DISABLE();
	__HAL_RCC_GPIOH_CLK_DISABLE();
}

/**********************************************************************************************************
 @Function			static void ModelPower_Init(void)
 @Description			Model Power ��ʼ�� PA8 : 0 ��ͨ 1 : ��ֹ
 @Input				void
 @Return				void
**********************************************************************************************************/
static void ModelPower_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	MODEL_POWER_RCC_GPIO_CLK_ENABLE();
	
	GPIO_Initure.Pin = MODEL_POWER_PIN;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;								//�������
	GPIO_Initure.Pull = GPIO_NOPULL;
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;									//����
	HAL_GPIO_Init(MODEL_POWER_GPIOx, &GPIO_Initure);							//��ʼ��GPIO
	
	MODELPOWER(OFF);													//��ʼ���رյ�Դ
}

/**********************************************************************************************************
 @Function			static void RaderPower_Init(void)
 @Description			Rader Power ��ʼ�� PB3 : 1 ��ͨ 0 : ��ֹ
 @Input				void
 @Return				void
**********************************************************************************************************/
static void RaderPower_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	RADER_POWER_RCC_GPIO_CLK_ENABLE();
	
	GPIO_Initure.Pin = RADER_POWER_PIN;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;								//�������
	GPIO_Initure.Pull = GPIO_NOPULL;
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;									//����
	HAL_GPIO_Init(RADER_POWER_GPIOx, &GPIO_Initure);							//��ʼ��GPIO
	
	RADERPOWER(OFF);
}

/**********************************************************************************************************
 @Function			static void NBIOTPower_Init(void)
 @Description			NBIOT Power ��ʼ�� PA0 : 1 ��ͨ 0 : ��ֹ
 @Input				void
 @Return				void
**********************************************************************************************************/
static void NBIOTPower_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	NBIOT_POWER_RCC_GPIO_CLK_ENABLE();
	
	GPIO_Initure.Pin = NBIOT_POWER_PIN;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;								//�������
	GPIO_Initure.Pull = GPIO_NOPULL;
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;									//����
	HAL_GPIO_Init(NBIOT_POWER_GPIOx, &GPIO_Initure);							//��ʼ��GPIO
	
	NBIOTPOWER(OFF);
}

/**********************************************************************************************************
 @Function			static void VbatPower_Init(void)
 @Description			VBAT VOLTAGE Power ��ʼ�� PA12 : 0 ��ͨ 1 : ��ֹ
 @Input				void
 @Return				void
**********************************************************************************************************/
static void VbatPower_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	VBAT_POWER_RCC_GPIO_CLK_ENABLE();
	
	GPIO_Initure.Pin = VBAT_POWER_PIN;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;								//�������
	GPIO_Initure.Pull = GPIO_NOPULL;
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;									//����
	HAL_GPIO_Init(VBAT_POWER_GPIOx, &GPIO_Initure);							//��ʼ��GPIO
	
	VBATPOWER(OFF);
}

/**********************************************************************************************************
 @Function			void PowerCtrlIO_Init(void)
 @Description			��Դ����IO��ʼ��
 @Input				void
 @Return				void
**********************************************************************************************************/
void PowerCtrlIO_Init(void)
{
	ModelPower_Init();													//ģ���Դ��ʼ��
	RaderPower_Init();													//�״��Դ��ʼ��
	NBIOTPower_Init();													//NBIOT��Դ��ʼ��
	VbatPower_Init();													//��ѹ����Դ��ʼ��
	
	MODELPOWER(OFF);													//�ر�ģ���ܵ�Դ
	RADERPOWER(OFF);													//�ر��״��Դ
	NBIOTPOWER(OFF);													//�ر�NBIOT��Դ
	VBATPOWER(OFF);													//�رյ�Դ��ѹ����Դ
}

/********************************************** END OF FLEE **********************************************/
