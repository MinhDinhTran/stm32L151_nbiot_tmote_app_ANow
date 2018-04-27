/**
  *********************************************************************************************************
  * @file    rollingover.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2018-04-26
  * @brief   
  *********************************************************************************************************
  * @attention
  *
  *
  *
  *********************************************************************************************************
  */

#include "rollingover.h"
#include "platform_config.h"
#include "platform_map.h"
#include "hal_switch.h"
#include "hal_beep.h"
#include "delay.h"
#include "usart.h"
#include "main.h"

unsigned int TimeBeforeIdle	= 0;											//TimeBeforeIdle

/**********************************************************************************************************
 @Function			void RollingOverPollExecution(void)
 @Description			RollingOverPollExecution
 @Input				void
 @Return				void
 @attention			翻转检测处理
**********************************************************************************************************/
void RollingOverPollExecution(void)
{
	/* 水银开关工作模式选择 */
	if (Mercury_Read() != 0) {
		/* 断开 */
		RollingOverMercuryBreak();
	}
	else {
		/* 闭合 */
		RollingOverMercuryClose();
	}
}

/**********************************************************************************************************
 @Function			bool RollingOverCheckToActive(void)
 @Description			RollingOverCheckToActive
 @Input				void
 @Return				bool
 @attention			翻转检测
**********************************************************************************************************/
bool RollingOverCheckToActive(void)
{
	static int32_t rollingover_time[5] = {-100, -100, -100, -100, -100};
	
	if (TCFG_EEPROM_GetActiveDevice()) {
		DeviceActivedMode = true;
		rollingover_time[0] = rollingover_time[1];
		rollingover_time[1] = rollingover_time[2];
		rollingover_time[2] = rollingover_time[3];
		rollingover_time[3] = rollingover_time[4];
		rollingover_time[4] = Stm32_GetSecondTick();
		if (((rollingover_time[4] - rollingover_time[0]) < 24) && (rollingover_time[4] - rollingover_time[0]) > 8) {
			rollingover_time[0] = 0;
			rollingover_time[1] = 0;
			rollingover_time[2] = 0;
			rollingover_time[3] = 0;
			rollingover_time[4] = 0;
		}
	}
	else {
		rollingover_time[0] = rollingover_time[1];
		rollingover_time[1] = rollingover_time[2];
		rollingover_time[2] = rollingover_time[3];
		rollingover_time[3] = rollingover_time[4];
		rollingover_time[4] = Stm32_GetSecondTick();
		if (((rollingover_time[4] - rollingover_time[0]) < 12) && (rollingover_time[4] - rollingover_time[0]) > 8) {
			TCFG_EEPROM_SetActiveDevice(1);
			rollingover_time[0] = 0;
			rollingover_time[1] = 0;
			rollingover_time[2] = 0;
			rollingover_time[3] = 0;
			rollingover_time[4] = 0;
		}
		else {
			DeviceActivedMode = false;
		}
	}
	
	return DeviceActivedMode;
}

/**********************************************************************************************************
 @Function			void RollingOverMercuryBreak(void)
 @Description			RollingOverMercuryBreak
 @Input				void
 @Return				void
 @attention			水银开关断开工作
**********************************************************************************************************/
void RollingOverMercuryBreak(void)
{
	if (DeviceIdleMode == true) {														//上次为倒置
		Delay_MS(100);
		if (Mercury_Read() != 0) {
			if (RollingOverCheckToActive() == true) {
				/* 进入正置工作 (Idle : false | Active : true) */
				MainRollingEnteredUpWork();												//已进入正放工作状态
			}
			else {
				/* 未进入正置工作 (Idle : false | Active : false) */
				MainRollingEnteringUpWork();												//将进入正放工作状态
			}
			DeviceIdleMode = false;
		}
	}
	else {																		//上次为正置
		if (DeviceActivedMode == true) {
			/* 正置工作 (Idle : false | Active : true) */
			MainRollingUpwardsActived();													//正放工作
		}
		else {
			/* 正置休眠 (Idle : false | Active : false) */
			MainRollingUpwardsSleep();													//正放休眠
		}
		
		TimeBeforeIdle = Stm32_GetSecondTick();
	}
}

/**********************************************************************************************************
 @Function			void RollingOverMercuryClose(void)
 @Description			RollingOverMercuryClose
 @Input				void
 @Return				void
 @attention			水银开关闭合休眠
**********************************************************************************************************/
void RollingOverMercuryClose(void)
{
	if (DeviceIdleMode == false) {
		BEEP_CtrlRepeat_Extend(1, 100, 0);
	}
	
	if (DeviceActivedMode == true) {
		/* 倒置工作 */
		if (DeviceIdleMode == false) {
			MainRollingEnteredDownWork();													//已进入倒放工作状态
			DeviceIdleMode = true;
		}
		
		/* 15Min 后进入休眠 */
		if ((TimeBeforeIdle + ROLLINGOVER_ENTER_SLEEP_TIME) < Stm32_GetSecondTick()) {
			DeviceActivedMode = false;
			TCFG_EEPROM_SetActiveDevice(0);
			MainRollingEnteredDownSleep();												//已进入倒放休眠状态
		}
		/* 15Sec 内处理 */
		else if ((TimeBeforeIdle + ROLLINGOVER_WAIT_SLEEP_TIME) > Stm32_GetSecondTick()) {
			MainRollingEnteredDownSleepKeepActived();										//将进入倒放休眠状态前保持工作
		}
		/* 15Sec ~ 15Min */
		else {
			MainRollingEnteringDownSleep();												//将进入倒放休眠
		}
	}
	else {
		/* 倒置休眠 */
		MainRollingDownSleep();															//倒放休眠
	}
	
	DeviceIdleMode = true;
}

/********************************************** END OF FLEE **********************************************/
