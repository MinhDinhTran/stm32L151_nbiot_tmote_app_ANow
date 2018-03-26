/**
  *********************************************************************************************************
  * @file    hal_rf.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2018-03-26
  * @brief   
  *********************************************************************************************************
  * @attention
  *					PB4	:		SDN
  *					PB10	:		nIRQ
  *					PA15	:		SPI1_NSS
  *					PA5	:		SPI1_SCK
  *					PA7	:		SPI1_MOSI
  *					PA6	:		SPI1_MISO
  *********************************************************************************************************
  */

#include "hal_rf.h"
#include "platform_config.h"

SPI_HandleTypeDef SPI_Handler;											//SPI句柄

/**********************************************************************************************************
 @Function			void Radio_Rf_Interface_Init(void)
 @Description			Radio_Rf_Interface_Init		: initialize the mcu's spi interface
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radio_Rf_Interface_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	SPI_Handler.Instance				= SPIx;
	
#ifndef SYSTEMCLOCK
	#error No Define SYSTEMCLOCK!
#else
#if (SYSTEMCLOCK == 4194000)
	SPI_Handler.Init.BaudRatePrescaler		= SPI_BAUDRATEPRESCALER_2;
#elif (SYSTEMCLOCK == 32000000)
	SPI_Handler.Init.BaudRatePrescaler		= SPI_BAUDRATEPRESCALER_8;
#else
	#error SYSTEMCLOCK Define Error
#endif
#endif
	
	SPI_Handler.Init.Direction			= SPI_DIRECTION_2LINES;
	SPI_Handler.Init.CLKPhase			= SPI_PHASE_1EDGE;
	SPI_Handler.Init.CLKPolarity			= SPI_POLARITY_LOW;
	SPI_Handler.Init.CRCCalculation		= SPI_CRCCALCULATION_DISABLE;
	SPI_Handler.Init.CRCPolynomial		= 7;
	SPI_Handler.Init.DataSize			= SPI_DATASIZE_8BIT;
	SPI_Handler.Init.FirstBit			= SPI_FIRSTBIT_MSB;
	SPI_Handler.Init.NSS				= SPI_NSS_SOFT;
	SPI_Handler.Init.TIMode				= SPI_TIMODE_DISABLE;
	SPI_Handler.Init.Mode				= SPI_MODE_MASTER;
	
	if (HAL_SPI_Init(&SPI_Handler) != HAL_OK) {
		/* Initialization Error */
	}
	
	RF_SDN_GPIO_CLK_ENABLE();
	RF_nIRQ_GPIO_CLK_ENABLE();
	
	GPIO_Initure.Pin					= RF_SDN_PIN;
	GPIO_Initure.Mode					= GPIO_MODE_OUTPUT_PP;
	GPIO_Initure.Speed					= GPIO_SPEED_HIGH;
	HAL_GPIO_Init(RF_SDN_GPIO_PORT, &GPIO_Initure);
	
	GPIO_Initure.Pin					= RF_nIRQ_PIN;
	GPIO_Initure.Mode					= GPIO_MODE_IT_FALLING;
	GPIO_Initure.Pull					= GPIO_PULLUP;
	GPIO_Initure.Speed					= GPIO_SPEED_HIGH;
	HAL_GPIO_Init(RF_nIRQ_GPIO_PORT, &GPIO_Initure);
	
	/* Set EXTI lines 15 to 10 Interrupt to the lowest priority */
	HAL_NVIC_SetPriority(RF_IRQn, 2, 0);
	HAL_NVIC_DisableIRQ(RF_IRQn);
}









































/********************************************** END OF FLEE **********************************************/
