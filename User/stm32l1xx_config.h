#ifndef __STM32L1XX_CONFIG_H
#define   __STM32L1XX_CONFIG_H

#include "sys.h"

#define	GPIOA_PIN						GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_2  | GPIO_PIN_3  | GPIO_PIN_4  | GPIO_PIN_5  | \
									GPIO_PIN_6  | GPIO_PIN_7  | GPIO_PIN_8  | GPIO_PIN_9  | GPIO_PIN_10 | GPIO_PIN_11 | \
									GPIO_PIN_12 | GPIO_PIN_15
#define	GPIOB_PIN						GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_2  | GPIO_PIN_3  | GPIO_PIN_4  | GPIO_PIN_5  | \
									GPIO_PIN_6  | GPIO_PIN_7  | GPIO_PIN_8  | GPIO_PIN_9  | GPIO_PIN_10 | GPIO_PIN_11 | \
									GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15
#define	GPIOC_PIN						GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_2  | GPIO_PIN_3  | GPIO_PIN_4  | GPIO_PIN_5  | \
									GPIO_PIN_6  | GPIO_PIN_7  | GPIO_PIN_8  | GPIO_PIN_9  | GPIO_PIN_10 | GPIO_PIN_11 | \
									GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15
#define	GPIOD_PIN						GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_2  | GPIO_PIN_3  | GPIO_PIN_4  | GPIO_PIN_5  | \
									GPIO_PIN_6  | GPIO_PIN_7  | GPIO_PIN_8  | GPIO_PIN_9  | GPIO_PIN_10 | GPIO_PIN_11 | \
									GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15
#define	GPIOE_PIN						GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_2  | GPIO_PIN_3  | GPIO_PIN_4  | GPIO_PIN_5  | \
									GPIO_PIN_6  | GPIO_PIN_7  | GPIO_PIN_8  | GPIO_PIN_9  | GPIO_PIN_10 | GPIO_PIN_11 | \
									GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15
#define	GPIOH_PIN						GPIO_PIN_0  | GPIO_PIN_1

#define MODEL_POWER_GPIOx				GPIOA
#define MODEL_POWER_PIN					GPIO_PIN_8
#define MODEL_POWER_RCC_GPIO_CLK_ENABLE()	__HAL_RCC_GPIOA_CLK_ENABLE()
#define MODEL_POWER_RCC_GPIO_CLK_DISABLE()	__HAL_RCC_GPIOA_CLK_DISABLE()

#define RADAR_POWER_GPIOx				GPIOB
#define RADAR_POWER_PIN					GPIO_PIN_3
#define RADAR_POWER_RCC_GPIO_CLK_ENABLE()	__HAL_RCC_GPIOB_CLK_ENABLE()
#define RADAR_POWER_RCC_GPIO_CLK_DISABLE()	__HAL_RCC_GPIOB_CLK_DISABLE()

#define NBIOT_POWER_GPIOx				GPIOA
#define NBIOT_POWER_PIN					GPIO_PIN_0
#define NBIOT_POWER_RCC_GPIO_CLK_ENABLE()	__HAL_RCC_GPIOA_CLK_ENABLE()
#define NBIOT_POWER_RCC_GPIO_CLK_DISABLE()	__HAL_RCC_GPIOA_CLK_DISABLE()

#define VBAT_POWER_GPIOx					GPIOA
#define VBAT_POWER_PIN					GPIO_PIN_12
#define VBAT_POWER_RCC_GPIO_CLK_ENABLE()	__HAL_RCC_GPIOA_CLK_ENABLE()
#define VBAT_POWER_RCC_GPIO_CLK_DISABLE()	__HAL_RCC_GPIOA_CLK_DISABLE()

#define MODELPOWER(n)	(n?HAL_GPIO_WritePin(MODEL_POWER_GPIOx, MODEL_POWER_PIN, GPIO_PIN_RESET):HAL_GPIO_WritePin(MODEL_POWER_GPIOx, MODEL_POWER_PIN, GPIO_PIN_SET))
#define RADARPOWER(n)	(n?HAL_GPIO_WritePin(RADAR_POWER_GPIOx, RADAR_POWER_PIN, GPIO_PIN_SET):HAL_GPIO_WritePin(RADAR_POWER_GPIOx, RADAR_POWER_PIN, GPIO_PIN_RESET))
#define NBIOTPOWER(n)	(n?HAL_GPIO_WritePin(NBIOT_POWER_GPIOx, NBIOT_POWER_PIN, GPIO_PIN_SET):HAL_GPIO_WritePin(NBIOT_POWER_GPIOx, NBIOT_POWER_PIN, GPIO_PIN_RESET))
#define VBATPOWER(n)	(n?HAL_GPIO_WritePin(VBAT_POWER_GPIOx, VBAT_POWER_PIN, GPIO_PIN_RESET):HAL_GPIO_WritePin(VBAT_POWER_GPIOx, VBAT_POWER_PIN, GPIO_PIN_SET))

/* Reset Flag Status */
typedef enum
{
	RCC_RESET_FLAG_NONE					= 0x00,							/*!< None Reset Flag				*/
	RCC_RESET_FLAG_IWDGRST				= 0x01,							/*!< Independent Watchdog Reset Flag	*/
	RCC_RESET_FLAG_SFTRST				= 0x02,							/*!< Software Reset Flag				*/
	RCC_RESET_FLAG_PORRST				= 0x03,							/*!< POR/PDR Reset Flag				*/
	RCC_RESET_FLAG_PINRST				= 0x04,							/*!< PIN Reset Flag					*/
	RCC_RESET_FLAG_LPWRRST				= 0x05,							/*!< Low-Power Reset Flag			*/
	RCC_RESET_FLAG_OBLRST				= 0x06,							/*!< Options Bytes Loading Reset Flag	*/
	RCC_RESET_FLAG_WWDGRST				= 0x07							/*!< Window Watchdog Reset Flag		*/
}RCC_RESET_FLAG_TypeDef;

RCC_RESET_FLAG_TypeDef RCC_ResetFlag_GetStatus(void);							//获取复位标志位

void LowPowerCtrlIO_Init(void);											//低功耗控制IO初始化
void LowPowerEnterStop(void);												//进入低功耗stop模式
void ModulePowerReset_Init(void);											//模块复位电源控制
void PowerCtrlIO_Init(void);												//电源控制IO初始化

void ModelError_Init(void);												//模块异常初始化
void RadarError_Init(void);												//雷达异常初始化
void NbiotError_Init(void);												//NBIOT异常初始化

#endif
