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
	HAL_Init();																		//HAL库初始化
	
#ifndef SYSTEMCLOCK
	#error No Define SYSTEMCLOCK!
#else
#if (SYSTEMCLOCK == SYSTEMCLOCKMSI)
	Stm32_MSIClock_Init(RCC_MSIRANGE_6);													//设置时钟MSI->4.194MHz
	Delay_Init(4194);																	//延时初始化4.194MHz系统时钟
#elif (SYSTEMCLOCK == SYSTEMCLOCKHSI)
	Stm32_Clock_Init(RCC_PLLMUL_6, RCC_PLLDIV_3);											//设置时钟HSI->32MHz
	Delay_Init(32000);																	//延时初始化32MHz系统时钟
#else
	#error SYSTEMCLOCK Define Error
#endif
#endif
	
	IWDG_Init(IWDG_PRESCALER_256, 0x0FFF);													//看门狗初始化,溢出时间28s
	RTC_Init();																		//RTC初始化
	
#ifdef MVB_SUBSN
	TCFG_EEPROM_Set_MAC_SN(MVB_SUBSN);														//写入MACSN
	TCFG_EEPROM_SetVender(MVB_BRAND);														//写入Verder
	TCFG_EEPROM_WriteConfigData();														//写入系统配置信息
#endif
	if (TCFG_EEPROM_CheckNewSNorBrand() == true) {											//检测新设备号或厂牌
		TCFG_EEPROM_WriteConfigData();													//写入系统配置信息
	}
	TCFG_EEPROM_SystemInfo_Init();														//系统运行信息初始化
	
	BEEP_CtrlRepeat_Extend(5, 50, 25);														//蜂鸣器
	
	LowPowerCtrlIO_Init();																//低功耗控制IO初始化
	ModulePowerReset_Init();																//模块电源复位
	PowerCtrlIO_Init();																	//电源控制IO初始化
	
	Uart1_Init(9600);																	//串口1初始化
	Uart2_Init(9600);																	//串口2初始化
	
	tmesh_securityInit();																//XTEA加密初始化
	Radio_Rf_Init();																	//SI4438初始化
	Radio_Trf_Xmit_Heartbeat();															//SI4438发送心跳包
	
	Inspect_Qmc5883l_Init();																//QMC5883L检测算法初始化
	QMC5883L_Init();																	//QMC5883L初始化
	
	NBIOT_Transport_Init(&NbiotATCmdHandler);												//NBIOT数据传输接口初始化
	NBIOT_Client_Init(&NbiotClientHandler, &NbiotATCmdHandler);									//NBIOT客户端初始化
	
	MQTTSN_Transport_Init(&MqttSNSocketNetHandler, &NbiotClientHandler, 4000, "106.14.142.169", 1884);	//MqttSN数据传输接口初始化
	MQTTSN_Client_Init(&MqttSNClientHandler, &MqttSNSocketNetHandler);							//MQTTSN客户端初始化
	
	CoapLongStructure.HeadPacket.DeviceSN = TCFG_EEPROM_Get_MAC_SN();
	CoapLongStructure.HeadPacket.DataLen = 0x00;
	CoapLongStructure.HeadPacket.ProtocolType = 0x00;
	CoapLongStructure.HeadPacket.Reserved1 = 0x00;
	CoapLongStructure.HeadPacket.ProtocolVersion = 0x00;
	CoapLongStructure.HeadPacket.Reserved2 = 0x00;
	CoapLongStructure.HeadPacket.PacketType = 0x05;
	CoapLongStructure.HeadPacket.PacketNumber = 0x00;
	CoapLongStructure.MsgPacket.DestSN = 0x00;
	CoapLongStructure.MsgPacket.Version = 0x01;
	CoapLongStructure.MsgPacket.Type = 0x3A;
	CoapLongStructure.RadarDistance = 0;
	CoapLongStructure.RadarStrength = 0;
	CoapLongStructure.RadarCoverCount = 0;
	CoapLongStructure.RadarDiff = 0;
	
	MqttSNStatusExtendStructure.DeviceSN = TCFG_EEPROM_Get_MAC_SN();
	MqttSNStatusExtendStructure.Distance = 0;
	MqttSNStatusExtendStructure.Strength = 0;
	MqttSNStatusExtendStructure.CoverCount = 0;
	MqttSNStatusExtendStructure.RadarDiff = 0;
	
	BEEP_CtrlRepeat_Extend(20, 50, 25);													//蜂鸣器
	Radio_Trf_Printf("Device Start MVBKK Reboot : %d ^_^", TCFG_SystemData.DeviceBootCount);			//启动信息
	
	QMC5883L_Rates_Selection_Freq(InspectQmc5883lHandler.Configuration.mag_measure_freq);			//QMC5883L扫描频率设置
	QMC5883L_Mode_Selection(QMC_MODE_CONTINOUS);												//QMC5883L数据读取开启
	
	while (1) {
		
		IWDG_Feed();																	//喂狗
		
		Radio_Trf_App_Task();															//小无线处理
		
		if (InspectQmc5883lHandler.DataReady == INSPECT_QMC_DATAREADY) {							//QMC5883L有数据待读取
			QMC5883L_ReadData_Simplify();													//QMC5883L读取数据
			InspectQmc5883lHandler.CarStatus = InspectQmc5883lHandler.PassDetect(Qmc5883lData.X_Now, Qmc5883lData.Y_Now, Qmc5883lData.Z_Now);
			if (InspectQmc5883lHandler.CarStatus == INSPECT_CAR_COME) {							//车检入
				Radio_Trf_Do_Heartbeat();												//小无线发送心跳包
				
#if NETPROTOCAL == NETCOAP
				/* COAP DATA ENQUEUE */
				CoapLongStructure.MagneticX = InspectQmc5883lHandler.MagValCalculate.x_now;
				CoapLongStructure.MagneticY = InspectQmc5883lHandler.MagValCalculate.y_now;
				CoapLongStructure.MagneticZ = InspectQmc5883lHandler.MagValCalculate.z_now;
				CoapLongStructure.MagneticDiff = InspectQmc5883lHandler.Parameter.diff_mag;
				CoapLongStructure.SpotStatus = InspectQmc5883lHandler.bInStatus;
				CoapLongStructure.SpotCount = InspectQmc5883lHandler.Parameter.carNumber * 2 - 1;
				CoapLongStructure.DateTime = RTC_GetUnixTimeToStamp();
				NET_Coap_Message_SendDataEnqueue((unsigned char *)&CoapLongStructure, sizeof(CoapLongStructure));
#elif NETPROTOCAL == NETMQTTSN
				/* MQTTSN DATA ENQUEUE */
				MqttSNStatusExtendStructure.MagX = InspectQmc5883lHandler.MagValCalculate.x_now;
				MqttSNStatusExtendStructure.MagY = InspectQmc5883lHandler.MagValCalculate.y_now;
				MqttSNStatusExtendStructure.MagZ = InspectQmc5883lHandler.MagValCalculate.z_now;
				MqttSNStatusExtendStructure.MagDiff = InspectQmc5883lHandler.Parameter.diff_mag;
				MqttSNStatusExtendStructure.Status = InspectQmc5883lHandler.bInStatus;
				MqttSNStatusExtendStructure.Count = InspectQmc5883lHandler.Parameter.carNumber * 2 - 1;
				MqttSNStatusExtendStructure.DateTime = RTC_GetUnixTimeToStamp();
				NET_MqttSN_Message_StatusExtendEnqueue(MqttSNStatusExtendStructure);
#endif
				
				BEEP_CtrlRepeat_Extend(1, 10, 0);
			}
			else if (InspectQmc5883lHandler.CarStatus == INSPECT_CAR_GO) {						//车检出
				Radio_Trf_Do_Heartbeat();												//小无线发送心跳包
				
#if NETPROTOCAL == NETCOAP
				/* COAP DATA ENQUEUE */
				CoapLongStructure.MagneticX = InspectQmc5883lHandler.MagValCalculate.x_now;
				CoapLongStructure.MagneticY = InspectQmc5883lHandler.MagValCalculate.y_now;
				CoapLongStructure.MagneticZ = InspectQmc5883lHandler.MagValCalculate.z_now;
				CoapLongStructure.MagneticDiff = InspectQmc5883lHandler.Parameter.diff_mag;
				CoapLongStructure.SpotStatus = InspectQmc5883lHandler.bInStatus;
				CoapLongStructure.SpotCount = InspectQmc5883lHandler.Parameter.carNumber * 2;
				CoapLongStructure.DateTime = RTC_GetUnixTimeToStamp();
				NET_Coap_Message_SendDataEnqueue((unsigned char *)&CoapLongStructure, sizeof(CoapLongStructure));
#elif NETPROTOCAL == NETMQTTSN
				/* MQTTSN DATA ENQUEUE */
				MqttSNStatusExtendStructure.MagX = InspectQmc5883lHandler.MagValCalculate.x_now;
				MqttSNStatusExtendStructure.MagY = InspectQmc5883lHandler.MagValCalculate.y_now;
				MqttSNStatusExtendStructure.MagZ = InspectQmc5883lHandler.MagValCalculate.z_now;
				MqttSNStatusExtendStructure.MagDiff = InspectQmc5883lHandler.Parameter.diff_mag;
				MqttSNStatusExtendStructure.Status = InspectQmc5883lHandler.bInStatus;
				MqttSNStatusExtendStructure.Count = InspectQmc5883lHandler.Parameter.carNumber * 2;
				MqttSNStatusExtendStructure.DateTime = RTC_GetUnixTimeToStamp();
				NET_MqttSN_Message_StatusExtendEnqueue(MqttSNStatusExtendStructure);
#endif
				
				BEEP_CtrlRepeat_Extend(2, 10, 10);
			}
		}
		
		/* 软件秒处理 */
		if (SecondTickTimes != Stm32_GetSecondTick()) {
			SecondTickTimes = Stm32_GetSecondTick();
			if ((SecondTickTimes >= 60) && (SecondTickTimes <= 65)) {
				TCFG_EEPROM_SetBootCount(0);												//运行正常BootCount清0
			}
			InspectQmc5883lHandler.Configuration.background_recalibration_seconds++;				//QMC5883L检测背景学习时间累加
			
#if NETPROTOCAL == NETCOAP
			NET_COAP_APP_PollExecution(&NbiotClientHandler);									//COAP PollExecution
#elif NETPROTOCAL == NETMQTTSN
			NET_MQTTSN_APP_PollExecution(&MqttSNClientHandler);								//MQTTSN PollExecution
#endif
		}
		
		/* QMC5883L异常由WAKE UP唤醒MCU */
		if (InspectQmc5883lHandler.Qmc5883lFile == INSPECT_QMC_ERROR_IS) {						//QMC5883L异常状态
			InspectQmc5883lHandler.Qmc5883lFile = INSPECT_QMC_ERROR_NONE;
			
			
			
			
		}
		
		
		
		
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
	while (1) {
		IWDG_Feed();																	//喂狗
		
		Radio_Trf_App_Task();															//小无线处理
		
		if (InspectQmc5883lHandler.DataReady == INSPECT_QMC_DATAREADY) {							//QMC5883L有数据待读取
			QMC5883L_ReadData_Simplify();													//QMC5883L读取数据
			InspectQmc5883lHandler.CarStatus = InspectQmc5883lHandler.PassDetect(Qmc5883lData.X_Now, Qmc5883lData.Y_Now, Qmc5883lData.Z_Now);
			if (InspectQmc5883lHandler.CarStatus == INSPECT_CAR_COME) {							//车检入
				Radio_Trf_Do_Heartbeat();												//小无线发送心跳包
				
#if NETPROTOCAL == NETCOAP
				/* COAP DATA ENQUEUE */
				CoapLongStructure.MagneticX = InspectQmc5883lHandler.MagValCalculate.x_now;
				CoapLongStructure.MagneticY = InspectQmc5883lHandler.MagValCalculate.y_now;
				CoapLongStructure.MagneticZ = InspectQmc5883lHandler.MagValCalculate.z_now;
				CoapLongStructure.MagneticDiff = InspectQmc5883lHandler.Parameter.diff_mag;
				CoapLongStructure.SpotStatus = InspectQmc5883lHandler.bInStatus;
				CoapLongStructure.SpotCount = InspectQmc5883lHandler.Parameter.carNumber * 2 - 1;
				CoapLongStructure.DateTime = RTC_GetUnixTimeToStamp();
				NET_Coap_Message_SendDataEnqueue((unsigned char *)&CoapLongStructure, sizeof(CoapLongStructure));
#elif NETPROTOCAL == NETMQTTSN
				/* MQTTSN DATA ENQUEUE */
				MqttSNStatusExtendStructure.MagX = InspectQmc5883lHandler.MagValCalculate.x_now;
				MqttSNStatusExtendStructure.MagY = InspectQmc5883lHandler.MagValCalculate.y_now;
				MqttSNStatusExtendStructure.MagZ = InspectQmc5883lHandler.MagValCalculate.z_now;
				MqttSNStatusExtendStructure.MagDiff = InspectQmc5883lHandler.Parameter.diff_mag;
				MqttSNStatusExtendStructure.Status = InspectQmc5883lHandler.bInStatus;
				MqttSNStatusExtendStructure.Count = InspectQmc5883lHandler.Parameter.carNumber * 2 - 1;
				MqttSNStatusExtendStructure.DateTime = RTC_GetUnixTimeToStamp();
				NET_MqttSN_Message_StatusExtendEnqueue(MqttSNStatusExtendStructure);
#endif
				
				BEEP_CtrlRepeat_Extend(1, 10, 0);
			}
			else if (InspectQmc5883lHandler.CarStatus == INSPECT_CAR_GO) {						//车检出
				Radio_Trf_Do_Heartbeat();												//小无线发送心跳包
				
#if NETPROTOCAL == NETCOAP
				/* COAP DATA ENQUEUE */
				CoapLongStructure.MagneticX = InspectQmc5883lHandler.MagValCalculate.x_now;
				CoapLongStructure.MagneticY = InspectQmc5883lHandler.MagValCalculate.y_now;
				CoapLongStructure.MagneticZ = InspectQmc5883lHandler.MagValCalculate.z_now;
				CoapLongStructure.MagneticDiff = InspectQmc5883lHandler.Parameter.diff_mag;
				CoapLongStructure.SpotStatus = InspectQmc5883lHandler.bInStatus;
				CoapLongStructure.SpotCount = InspectQmc5883lHandler.Parameter.carNumber * 2;
				CoapLongStructure.DateTime = RTC_GetUnixTimeToStamp();
				NET_Coap_Message_SendDataEnqueue((unsigned char *)&CoapLongStructure, sizeof(CoapLongStructure));
#elif NETPROTOCAL == NETMQTTSN
				/* MQTTSN DATA ENQUEUE */
				MqttSNStatusExtendStructure.MagX = InspectQmc5883lHandler.MagValCalculate.x_now;
				MqttSNStatusExtendStructure.MagY = InspectQmc5883lHandler.MagValCalculate.y_now;
				MqttSNStatusExtendStructure.MagZ = InspectQmc5883lHandler.MagValCalculate.z_now;
				MqttSNStatusExtendStructure.MagDiff = InspectQmc5883lHandler.Parameter.diff_mag;
				MqttSNStatusExtendStructure.Status = InspectQmc5883lHandler.bInStatus;
				MqttSNStatusExtendStructure.Count = InspectQmc5883lHandler.Parameter.carNumber * 2;
				MqttSNStatusExtendStructure.DateTime = RTC_GetUnixTimeToStamp();
				NET_MqttSN_Message_StatusExtendEnqueue(MqttSNStatusExtendStructure);
#endif
				
				BEEP_CtrlRepeat_Extend(2, 10, 10);
			}
		}
		
		/* 软件秒处理 */
		if (SecondTickTimes != Stm32_GetSecondTick()) {
			SecondTickTimes = Stm32_GetSecondTick();
			if ((SecondTickTimes >= 60) && (SecondTickTimes <= 65)) {
				TCFG_EEPROM_SetBootCount(0);												//运行正常BootCount清0
			}
			InspectQmc5883lHandler.Configuration.background_recalibration_seconds++;				//QMC5883L检测背景学习时间累加
			break;
		}
	}
}

/********************************************** END OF FLEE **********************************************/
