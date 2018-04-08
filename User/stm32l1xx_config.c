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
#include "platform_config.h"
#include "platform_map.h"
#include "inspectconfig.h"
#include "hal_iic.h"
#include "hal_rtc.h"
#include "hal_qmc5883l.h"
#include "delay.h"

extern __IO uint32_t uwTick;

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
 @Function			void ModulePowerReset_Init(void)
 @Description			模块复位电源控制
 @Input				void
 @Return				void
**********************************************************************************************************/
void ModulePowerReset_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	ModelPower_Init();													//模块电源初始化
	RaderPower_Init();													//雷达电源初始化
	NBIOTPower_Init();													//NBIOT电源初始化
	VbatPower_Init();													//电压检测电源初始化
	
	MODELPOWER(OFF);													//关闭模块总电源
	RADERPOWER(OFF);													//关闭雷达电源
	NBIOTPOWER(OFF);													//关闭NBIOT电源
	VBATPOWER(OFF);													//关闭电源电压检测电源
	
	IIC_SCL_RCC_GPIO_CLK_ENABLE();
	IIC_SDA_RCC_GPIO_CLK_ENABLE();
	
	GPIO_Initure.Pin = IIC_SCL_PIN;										//IIC_SCL
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;								//OUTPUT
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;									//HIGH
	HAL_GPIO_Init(IIC_SCL_GPIOx, &GPIO_Initure);
	
	GPIO_Initure.Pin = IIC_SDA_PIN;										//IIC_SDA
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;								//OUTPUT
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;									//HIGH
	HAL_GPIO_Init(IIC_SDA_GPIOx, &GPIO_Initure);
	
	HAL_GPIO_WritePin(IIC_SCL_GPIOx, IIC_SCL_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(IIC_SDA_GPIOx, IIC_SDA_PIN, GPIO_PIN_RESET);
	Delay_MS(3000);													//断电3秒
	HAL_GPIO_WritePin(IIC_SCL_GPIOx, IIC_SCL_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(IIC_SDA_GPIOx, IIC_SDA_PIN, GPIO_PIN_SET);
}

/**********************************************************************************************************
 @Function			void LowPowerEnterStop(void)
 @Description			进入低功耗stop模式
 @Input				void
 @Return				void
**********************************************************************************************************/
void LowPowerEnterStop(void)
{
	__HAL_RCC_PWR_CLK_ENABLE();
	SysTick->CTRL &= (~SysTick_CTRL_ENABLE_Msk);
	
	/* WAKE UP 中断唤醒配置1S */
	HAL_RTCEx_DeactivateWakeUpTimer(&RTC_Handler);
	HAL_RTCEx_SetWakeUpTimer_IT(&RTC_Handler, 0x800, RTC_WAKEUPCLOCK_RTCCLK_DIV16);
	
	/* QMC5883L数据读取 */
	if (InspectQmc5883lHandler.DataReady == INSPECT_QMC_DATAREADY) {				//QMC5883L有数据待读取
		InspectQmc5883lHandler.DataReady = INSPECT_QMC_DATAUNREADY;
		QMC5883L_ReadData_Simplify();
	}
	
	/* 进入STOP模式 */
	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
	
	/* 退出STOP继续运行 */
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	
	/* 根据扫描频率补差休眠时间 */
	if (InspectQmc5883lHandler.Configuration.mag_measure_freq == 0) {			//地磁扫描频率, 即休眠时间 10Hz
		uwTick += 100;
	}
	else if (InspectQmc5883lHandler.Configuration.mag_measure_freq == 1) {		//地磁扫描频率, 即休眠时间 50Hz
		uwTick += 20;
	}
	else if (InspectQmc5883lHandler.Configuration.mag_measure_freq == 2) {		//地磁扫描频率, 即休眠时间 100Hz
		uwTick += 10;
	}
	else if (InspectQmc5883lHandler.Configuration.mag_measure_freq == 3) {		//地磁扫描频率, 即休眠时间 200Hz
		uwTick += 5;
	}
	else {															//地磁扫描频率, 即休眠时间 50Hz
		uwTick += 20;
	}
	
	/* 系统时钟配置 */
#ifndef SYSTEMCLOCK
	#error No Define SYSTEMCLOCK!
#else
#if (SYSTEMCLOCK == SYSTEMCLOCKMSI)
	Stm32_MSIClock_Init(RCC_MSIRANGE_6);									//设置时钟MSI->4.194MHz
	Delay_Init(4194);													//延时初始化4.194MHz系统时钟
#elif (SYSTEMCLOCK == SYSTEMCLOCKHSI)
	Stm32_Clock_Init(RCC_PLLMUL_6, RCC_PLLDIV_3);							//设置时钟HSI->32MHz
	Delay_Init(32000);													//延时初始化32MHz系统时钟
#else
	#error SYSTEMCLOCK Define Error
#endif
#endif
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
	
	MODELPOWER(ON);													//开启模块总电源
	RADERPOWER(OFF);													//关闭雷达电源
	NBIOTPOWER(OFF);													//关闭NBIOT电源
	VBATPOWER(OFF);													//关闭电源电压检测电源
}

/********************************************** END OF FLEE **********************************************/
