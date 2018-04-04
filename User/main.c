/**
  *********************************************************************************************************
  * @file    main.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2017-10-23
  * @brief   
  *********************************************************************************************************
  * @attention
  *
  *
  *
  *********************************************************************************************************
  */

#include "main.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "stm32l1xx_config.h"
#include "platform_config.h"
#include "platform_map.h"
#include "hal_rtc.h"
#include "hal_iwdg.h"
#include "hal_beep.h"
#include "hal_switch.h"
#include "hal_vbat.h"
#include "hal_vptat.h"
#include "hal_temperature.h"
#include "hal_eeprom.h"
#include "hal_qmc5883l.h"
#include "hal_radar.h"
#include "net_coap_app.h"
#include "net_mqttsn_app.h"
#include "radio_hal_rf.h"
#include "radio_rf_app.h"
#include "tmesh_xtea.h"
#include "inspectconfig.h"


/**********************************************************************************************************
 @Function			int main(void)
 @Description			Main
 @Input				void
 @Return				int
**********************************************************************************************************/
int main(void)
{
	HAL_Init();																//HAL库初始化
	
#ifndef SYSTEMCLOCK
	#error No Define SYSTEMCLOCK!
#else
#if (SYSTEMCLOCK == SYSTEMCLOCKMSI)
	Stm32_MSIClock_Init(RCC_MSIRANGE_6);											//设置时钟MSI->4.194MHz
	Delay_Init(4194);															//延时初始化4.194MHz系统时钟
#elif (SYSTEMCLOCK == SYSTEMCLOCKHSI)
	Stm32_Clock_Init(RCC_PLLMUL_6, RCC_PLLDIV_3);									//设置时钟HSI->32MHz
	Delay_Init(32000);															//延时初始化32MHz系统时钟
#else
	#error SYSTEMCLOCK Define Error
#endif
#endif
	
	IWDG_Init(IWDG_PRESCALER_256, 0x0FFF);											//看门狗初始化,分频系数256,重载值0x0FFF,溢出时间28s
	RTC_Init();																//RTC初始化
	
	Uart1_Init(9600);															//串口1初始化
	Uart2_Init(9600);															//串口2初始化
	
#ifdef MVB_SUBSN
	TCFG_EEPROM_Set_MAC_SN(MVB_SUBSN);												//写入MACSN
	TCFG_EEPROM_SetVender(MVB_BRAND);												//写入Verder
	TCFG_EEPROM_WriteConfigData();												//写入系统配置信息
#endif
	if (TCFG_EEPROM_CheckNewSNorBrand() == true) {									//检测新设备号或厂牌
		TCFG_EEPROM_WriteConfigData();											//写入系统配置信息
	}
	TCFG_EEPROM_SystemInfo_Init();												//系统运行信息初始化
	
	LowPowerCtrlIO_Init();														//低功耗控制IO初始化
	ModulePowerReset_Init();														//模块电源复位
	PowerCtrlIO_Init();															//电源控制IO初始化
	
	tmesh_securityInit();														//XTEA加密初始化
	Radio_Rf_Init();															//SI4438初始化
	Radio_Trf_Xmit_Heartbeat();													//SI4438发送心跳包
	
	Inspect_Qmc5883l_Init();														//QMC5883L检测算法初始化
	QMC5883L_Init();															//QMC5883L初始化
	
	TCFG_EEPROM_SetBootCount(0);													//运行正常BootCount清0
	BEEP_CtrlRepeat_Extend(20, 50, 25);											//蜂鸣器
	Radio_Trf_Printf("Device Start MVBKK Reboot : %d ^_^", TCFG_SystemData.DeviceBootCount);	//启动信息
	
	QMC5883L_Mode_Selection(QMC_MODE_CONTINOUS);										//QMC5883L数据读取开启
	
	
	
	
	while (1) {
		
		
		
		MainMajorCycle();
		
		Delay_MS(1000);
	}
}

/**********************************************************************************************************
 @Function			void MainMajorCycle(void)
 @Description			MainMajorCycle
 @Input				void
 @Return				void
**********************************************************************************************************/
void MainMajorCycle(void)
{
	Radio_Trf_App_Task();
	
	IWDG_Feed();
	
	
	
	
	
	
}

/********************************************** END OF FLEE **********************************************/
