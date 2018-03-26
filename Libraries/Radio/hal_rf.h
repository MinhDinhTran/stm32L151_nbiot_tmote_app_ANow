#ifndef __HAL_RF_H
#define   __HAL_RF_H

#include "sys.h"

/* Definition for SPIx clock resources */
#define SPIx							SPI1
#define SPIx_RCC_CLK_ENABLE()				__HAL_RCC_SPI1_CLK_ENABLE()
#define SPIx_RCC_CLK_DISABLE()			__HAL_RCC_SPI1_CLK_DISABLE()

#define SPIx_FORCE_RESET()				__HAL_RCC_SPI1_FORCE_RESET()
#define SPIx_RELEASE_RESET()				__HAL_RCC_SPI1_RELEASE_RESET()

/* Definition for SPIx GPIO resources */
#define SPIx_NSS_GPIO_CLK_ENABLE()			__HAL_RCC_GPIOA_CLK_ENABLE()
#define SPIx_NSS_GPIO_CLK_DISABLE()		__HAL_RCC_GPIOA_CLK_DISABLE()
#define SPIx_SCK_GPIO_CLK_ENABLE()			__HAL_RCC_GPIOA_CLK_ENABLE()
#define SPIx_SCK_GPIO_CLK_DISABLE()		__HAL_RCC_GPIOA_CLK_DISABLE()
#define SPIx_MOSI_GPIO_CLK_ENABLE()		__HAL_RCC_GPIOA_CLK_ENABLE()
#define SPIx_MOSI_GPIO_CLK_DISABLE()		__HAL_RCC_GPIOA_CLK_DISABLE()
#define SPIx_MISO_GPIO_CLK_ENABLE()		__HAL_RCC_GPIOA_CLK_ENABLE()
#define SPIx_MISO_GPIO_CLK_DISABLE()		__HAL_RCC_GPIOA_CLK_DISABLE()

/* Definition for SPIx Pins */
#define SPIx_NSS_GPIO_PORT				GPIOA
#define SPIx_NSS_PIN					GPIO_PIN_15
#define SPIx_SCK_GPIO_PORT				GPIOA
#define SPIx_SCK_PIN					GPIO_PIN_5
#define SPIx_SCK_AF						GPIO_AF5_SPI1
#define SPIx_MOSI_GPIO_PORT				GPIOA
#define SPIx_MOSI_PIN					GPIO_PIN_7
#define SPIx_MOSI_AF					GPIO_AF5_SPI1
#define SPIx_MISO_GPIO_PORT				GPIOA
#define SPIx_MISO_PIN					GPIO_PIN_6
#define SPIx_MISO_AF					GPIO_AF5_SPI1

/* Definition for RF GPIO resources */
#define RF_SDN_GPIO_CLK_ENABLE()			__HAL_RCC_GPIOB_CLK_ENABLE()
#define RF_SDN_GPIO_CLK_DISABLE()			__HAL_RCC_GPIOB_CLK_DISABLE()
#define RF_nIRQ_GPIO_CLK_ENABLE()			__HAL_RCC_GPIOB_CLK_ENABLE()
#define RF_nIRQ_GPIO_CLK_DISABLE()			__HAL_RCC_GPIOB_CLK_DISABLE()

/* Definition for RF Pins */
#define RF_SDN_GPIO_PORT					GPIOB
#define RF_SDN_PIN						GPIO_PIN_4
#define RF_nIRQ_GPIO_PORT				GPIOB
#define RF_nIRQ_PIN						GPIO_PIN_10

/* Definition for SPIx's NVIC */
#define RF_IRQn						EXTI15_10_IRQn

extern SPI_HandleTypeDef SPI_Handler;										//SPI句柄



void Radio_Rf_Interface_Init(void);										//Initialize the mcu's spi interface
















#endif
