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
 @Description			RCC_ResetFlag_GetStatus	: 获取复位标志位
 @Input				void
 @Return				RCC_RESET_FLAG_TypeDef	: 标志位
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
 @Description			低功耗控制IO初始化
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
 @Description			Model Power 初始化 PA8 : 0 导通 1 : 截止
 @Input				void
 @Return				void
**********************************************************************************************************/
static void ModelPower_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	MODEL_POWER_RCC_GPIO_CLK_ENABLE();
	
	GPIO_Initure.Pin = MODEL_POWER_PIN;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;								//推挽输出
	GPIO_Initure.Pull = GPIO_NOPULL;
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;									//高速
	HAL_GPIO_Init(MODEL_POWER_GPIOx, &GPIO_Initure);							//初始化GPIO
	
	MODELPOWER(OFF);													//初始化关闭电源
}

/**********************************************************************************************************
 @Function			static void RaderPower_Init(void)
 @Description			Rader Power 初始化 PB3 : 1 导通 0 : 截止
 @Input				void
 @Return				void
**********************************************************************************************************/
static void RaderPower_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	RADER_POWER_RCC_GPIO_CLK_ENABLE();
	
	GPIO_Initure.Pin = RADER_POWER_PIN;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;								//推挽输出
	GPIO_Initure.Pull = GPIO_NOPULL;
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;									//高速
	HAL_GPIO_Init(RADER_POWER_GPIOx, &GPIO_Initure);							//初始化GPIO
	
	RADERPOWER(OFF);
}

/**********************************************************************************************************
 @Function			static void NBIOTPower_Init(void)
 @Description			NBIOT Power 初始化 PA0 : 1 导通 0 : 截止
 @Input				void
 @Return				void
**********************************************************************************************************/
static void NBIOTPower_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	NBIOT_POWER_RCC_GPIO_CLK_ENABLE();
	
	GPIO_Initure.Pin = NBIOT_POWER_PIN;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;								//推挽输出
	GPIO_Initure.Pull = GPIO_NOPULL;
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;									//高速
	HAL_GPIO_Init(NBIOT_POWER_GPIOx, &GPIO_Initure);							//初始化GPIO
	
	NBIOTPOWER(OFF);
}

/**********************************************************************************************************
 @Function			static void VbatPower_Init(void)
 @Description			VBAT VOLTAGE Power 初始化 PA12 : 0 导通 1 : 截止
 @Input				void
 @Return				void
**********************************************************************************************************/
static void VbatPower_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	VBAT_POWER_RCC_GPIO_CLK_ENABLE();
	
	GPIO_Initure.Pin = VBAT_POWER_PIN;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;								//推挽输出
	GPIO_Initure.Pull = GPIO_NOPULL;
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;									//高速
	HAL_GPIO_Init(VBAT_POWER_GPIOx, &GPIO_Initure);							//初始化GPIO
	
	VBATPOWER(OFF);
}

/**********************************************************************************************************
 @Function			void PowerCtrlIO_Init(void)
 @Description			电源控制IO初始化
 @Input				void
 @Return				void
**********************************************************************************************************/
void PowerCtrlIO_Init(void)
{
	ModelPower_Init();													//模块电源初始化
	RaderPower_Init();													//雷达电源初始化
	NBIOTPower_Init();													//NBIOT电源初始化
	VbatPower_Init();													//电压检测电源初始化
	
	MODELPOWER(OFF);													//关闭模块总电源
	RADERPOWER(OFF);													//关闭雷达电源
	NBIOTPOWER(OFF);													//关闭NBIOT电源
	VBATPOWER(OFF);													//关闭电源电压检测电源
}

/********************************************** END OF FLEE **********************************************/
