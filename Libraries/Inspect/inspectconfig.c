/**
  *********************************************************************************************************
  * @file    inspectconfig.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2018-04-02
  * @brief   
  *********************************************************************************************************
  * @attention
  *
  *
  *
  *********************************************************************************************************
  */

#include "inspectconfig.h"
#include "inspectqmc5883l.h"
#include "inspectradar.h"
#include "hal_qmc5883l.h"
#include "hal_rtc.h"
#include "hal_beep.h"
#include "net_coap_app.h"
#include "net_mqttsn_app.h"
#include "radio_hal_rf.h"
#include "radio_rf_app.h"
#include "platform_config.h"
#include "platform_map.h"

Inspect_Qmc5883lTypeDef		InspectQmc5883lHandler;							//Inspect QMC5883L Handler

/**********************************************************************************************************
 @Function			void Inspect_Qmc5883l_Init(void)
 @Description			Inspect_Qmc5883l_Init		: Qmc5883L车辆检测初始化
 @Input				void
 @Return				void
**********************************************************************************************************/
void Inspect_Qmc5883l_Init(void)
{
	/* 配置参数 */
	InspectQmc5883lHandler.Configuration.mag_measure_freq				= TCFG_SystemData.MagFreq;			//地磁扫描频率
	InspectQmc5883lHandler.Configuration.carin_threshhold				= TCFG_SystemData.CarinThreshhold;		//车辆进入参数
	InspectQmc5883lHandler.Configuration.carout_threshhold				= TCFG_SystemData.CaroutThreshhold;	//车辆离开参数
	InspectQmc5883lHandler.Configuration.recalibration_num				= TCFG_SystemData.RecalibrationNum;	//微小变化重计算次数
	InspectQmc5883lHandler.Configuration.recalibration_overtime			= TCFG_SystemData.RecalibrationOvertime;//激烈变化重计算时间(min)
	InspectQmc5883lHandler.Parameter.carNumber						= TCFG_SystemData.CarNumber;			//检测车辆数
	
	if (InspectQmc5883lHandler.Configuration.mag_measure_freq == 0) {									//10Hz
		InspectQmc5883lHandler.Configuration.recalibration_overtime_cnt	= 600 * InspectQmc5883lHandler.Configuration.recalibration_overtime;
	}
	else if (InspectQmc5883lHandler.Configuration.mag_measure_freq == 1) {								//50Hz
		InspectQmc5883lHandler.Configuration.recalibration_overtime_cnt	= 3000 * InspectQmc5883lHandler.Configuration.recalibration_overtime;
	}
	else if (InspectQmc5883lHandler.Configuration.mag_measure_freq == 2) {								//100Hz
		InspectQmc5883lHandler.Configuration.recalibration_overtime_cnt	= 6000 * InspectQmc5883lHandler.Configuration.recalibration_overtime;
	}
	else if (InspectQmc5883lHandler.Configuration.mag_measure_freq == 3) {								//200Hz
		InspectQmc5883lHandler.Configuration.recalibration_overtime_cnt	= 12000 * InspectQmc5883lHandler.Configuration.recalibration_overtime;
	}
	else {
		InspectQmc5883lHandler.Configuration.recalibration_overtime_cnt	= 3000 * InspectQmc5883lHandler.Configuration.recalibration_overtime;
	}
	InspectQmc5883lHandler.Configuration.background_recalibration_seconds	= 0;								//记录上次背景校准之后时间
	
	InspectQmc5883lHandler.DataReady								= INSPECT_QMC_DATAUNREADY;
	InspectQmc5883lHandler.Qmc5883lFail							= INSPECT_QMC_ERROR_NONE;
	InspectQmc5883lHandler.CarStatus								= INSPECT_CAR_NONE;
	
	/* 调用方法 */
	InspectQmc5883lHandler.PassDetect								= Inspect_Qmc5883l_Pass_Detect;		//车辆检测
	InspectQmc5883lHandler.CheckStatus								= Inspect_Qmc5883l_CheckStatus;		//检测状态
	InspectQmc5883lHandler.KmeansAllocate							= Inspect_Qmc5883l_KmeansAllocate;		//质心分配
	InspectQmc5883lHandler.BackgroundCalculate						= Inspect_Qmc5883l_BackgroundCalculate;	//背景计算
}

/**********************************************************************************************************
 @Function			void Inspect_Qmc5883l_SensitivityConfig(unsigned char Sensitivity)
 @Description			Inspect_Qmc5883l_SensitivityConfig	: Qmc5883L灵敏度配置
 @Input				Sensitivity
 @Return				void
**********************************************************************************************************/
void Inspect_Qmc5883l_SensitivityConfig(unsigned char Sensitivity)
{
	if (Sensitivity == SENSE_HIGHEST) {
		TCFG_SystemData.CarinThreshhold							= 120;
		TCFG_SystemData.CaroutThreshhold							= 12;
		TCFG_EEPROM_SetCarinThreshhold(TCFG_SystemData.CarinThreshhold);
		TCFG_EEPROM_SetCaroutThreshhold(TCFG_SystemData.CaroutThreshhold);
		InspectQmc5883lHandler.Configuration.carin_threshhold			= TCFG_SystemData.CarinThreshhold;		//车辆进入参数
		InspectQmc5883lHandler.Configuration.carout_threshhold			= TCFG_SystemData.CaroutThreshhold;	//车辆离开参数
	}
	else if (Sensitivity == SENSE_HIGH) {
		TCFG_SystemData.CarinThreshhold							= 150;
		TCFG_SystemData.CaroutThreshhold							= 15;
		TCFG_EEPROM_SetCarinThreshhold(TCFG_SystemData.CarinThreshhold);
		TCFG_EEPROM_SetCaroutThreshhold(TCFG_SystemData.CaroutThreshhold);
		InspectQmc5883lHandler.Configuration.carin_threshhold			= TCFG_SystemData.CarinThreshhold;		//车辆进入参数
		InspectQmc5883lHandler.Configuration.carout_threshhold			= TCFG_SystemData.CaroutThreshhold;	//车辆离开参数
	}
	else if (Sensitivity == SENSE_MIDDLE) {
		TCFG_SystemData.CarinThreshhold							= 180;
		TCFG_SystemData.CaroutThreshhold							= 18;
		TCFG_EEPROM_SetCarinThreshhold(TCFG_SystemData.CarinThreshhold);
		TCFG_EEPROM_SetCaroutThreshhold(TCFG_SystemData.CaroutThreshhold);
		InspectQmc5883lHandler.Configuration.carin_threshhold			= TCFG_SystemData.CarinThreshhold;		//车辆进入参数
		InspectQmc5883lHandler.Configuration.carout_threshhold			= TCFG_SystemData.CaroutThreshhold;	//车辆离开参数
	}
	else if (Sensitivity == SENSE_LOW) {
		TCFG_SystemData.CarinThreshhold							= 210;
		TCFG_SystemData.CaroutThreshhold							= 21;
		TCFG_EEPROM_SetCarinThreshhold(TCFG_SystemData.CarinThreshhold);
		TCFG_EEPROM_SetCaroutThreshhold(TCFG_SystemData.CaroutThreshhold);
		InspectQmc5883lHandler.Configuration.carin_threshhold			= TCFG_SystemData.CarinThreshhold;		//车辆进入参数
		InspectQmc5883lHandler.Configuration.carout_threshhold			= TCFG_SystemData.CaroutThreshhold;	//车辆离开参数
	}
	else if (Sensitivity == SENSE_LOWEST) {
		TCFG_SystemData.CarinThreshhold							= 240;
		TCFG_SystemData.CaroutThreshhold							= 24;
		TCFG_EEPROM_SetCarinThreshhold(TCFG_SystemData.CarinThreshhold);
		TCFG_EEPROM_SetCaroutThreshhold(TCFG_SystemData.CaroutThreshhold);
		InspectQmc5883lHandler.Configuration.carin_threshhold			= TCFG_SystemData.CarinThreshhold;		//车辆进入参数
		InspectQmc5883lHandler.Configuration.carout_threshhold			= TCFG_SystemData.CaroutThreshhold;	//车辆离开参数
	}
	else {
		TCFG_SystemData.CarinThreshhold							= 80;
		TCFG_SystemData.CaroutThreshhold							= 13;
		TCFG_EEPROM_SetCarinThreshhold(TCFG_SystemData.CarinThreshhold);
		TCFG_EEPROM_SetCaroutThreshhold(TCFG_SystemData.CaroutThreshhold);
		InspectQmc5883lHandler.Configuration.carin_threshhold			= TCFG_SystemData.CarinThreshhold;		//车辆进入参数
		InspectQmc5883lHandler.Configuration.carout_threshhold			= TCFG_SystemData.CaroutThreshhold;	//车辆离开参数
	}
}

/**********************************************************************************************************
 @Function			void Inspect_Qmc5883l_MagFreqConfig(unsigned char Freq)
 @Description			Inspect_Qmc5883l_MagFreqConfig	: Qmc5883L地磁扫描频率配置
 @Input				Freq
 @Return				void
**********************************************************************************************************/
void Inspect_Qmc5883l_MagFreqConfig(unsigned char Freq)
{
	if (Freq == 0) {
		TCFG_SystemData.MagFreq = 0;
		TCFG_EEPROM_SetMagFreq(TCFG_SystemData.MagFreq);
		InspectQmc5883lHandler.Configuration.mag_measure_freq			= TCFG_SystemData.MagFreq;			//地磁扫描频率
	}
	else if (Freq == 1) {
		TCFG_SystemData.MagFreq = 1;
		TCFG_EEPROM_SetMagFreq(TCFG_SystemData.MagFreq);
		InspectQmc5883lHandler.Configuration.mag_measure_freq			= TCFG_SystemData.MagFreq;			//地磁扫描频率
	}
	else if (Freq == 2) {
		TCFG_SystemData.MagFreq = 2;
		TCFG_EEPROM_SetMagFreq(TCFG_SystemData.MagFreq);
		InspectQmc5883lHandler.Configuration.mag_measure_freq			= TCFG_SystemData.MagFreq;			//地磁扫描频率
	}
	else if (Freq == 3) {
		TCFG_SystemData.MagFreq = 3;
		TCFG_EEPROM_SetMagFreq(TCFG_SystemData.MagFreq);
		InspectQmc5883lHandler.Configuration.mag_measure_freq			= TCFG_SystemData.MagFreq;			//地磁扫描频率
	}
	else {
		TCFG_SystemData.MagFreq = 1;
		TCFG_EEPROM_SetMagFreq(TCFG_SystemData.MagFreq);
		InspectQmc5883lHandler.Configuration.mag_measure_freq			= TCFG_SystemData.MagFreq;			//地磁扫描频率
	}
	
	if (InspectQmc5883lHandler.Configuration.mag_measure_freq == 0) {									//10Hz
		InspectQmc5883lHandler.Configuration.recalibration_overtime_cnt	= 600 * InspectQmc5883lHandler.Configuration.recalibration_overtime;
	}
	else if (InspectQmc5883lHandler.Configuration.mag_measure_freq == 1) {								//50Hz
		InspectQmc5883lHandler.Configuration.recalibration_overtime_cnt	= 3000 * InspectQmc5883lHandler.Configuration.recalibration_overtime;
	}
	else if (InspectQmc5883lHandler.Configuration.mag_measure_freq == 2) {								//100Hz
		InspectQmc5883lHandler.Configuration.recalibration_overtime_cnt	= 6000 * InspectQmc5883lHandler.Configuration.recalibration_overtime;
	}
	else if (InspectQmc5883lHandler.Configuration.mag_measure_freq == 3) {								//200Hz
		InspectQmc5883lHandler.Configuration.recalibration_overtime_cnt	= 12000 * InspectQmc5883lHandler.Configuration.recalibration_overtime;
	}
	else {
		InspectQmc5883lHandler.Configuration.recalibration_overtime_cnt	= 3000 * InspectQmc5883lHandler.Configuration.recalibration_overtime;
	}
	
	InspectQmc5883lHandler.DataReady								= INSPECT_QMC_DATAUNREADY;			//QMC5883L待读取标志位清空
	
	QMC5883L_Init();																			//QMC5883L初始化
	QMC5883L_Rates_Selection_Freq(InspectQmc5883lHandler.Configuration.mag_measure_freq);					//QMC5883L扫描频率设置
	QMC5883L_Mode_Selection(QMC_MODE_CONTINOUS);														//QMC5883L数据读取开启
}

/**********************************************************************************************************
 @Function			void Inspect_Qmc5883l_CarinThreshholdConfig(unsigned char CarinThreshhold)
 @Description			Inspect_Qmc5883l_CarinThreshholdConfig	: Qmc5883L车辆进入参数配置
 @Input				CarinThreshhold
 @Return				void
**********************************************************************************************************/
void Inspect_Qmc5883l_CarinThreshholdConfig(unsigned char CarinThreshhold)
{
	TCFG_SystemData.CarinThreshhold = CarinThreshhold;
	TCFG_EEPROM_SetCarinThreshhold(TCFG_SystemData.CarinThreshhold);
	InspectQmc5883lHandler.Configuration.carin_threshhold				= TCFG_SystemData.CarinThreshhold;		//车辆进入参数
}

/**********************************************************************************************************
 @Function			void Inspect_Qmc5883l_CaroutThreshholdConfig(unsigned char CaroutThreshhold)
 @Description			Inspect_Qmc5883l_CaroutThreshholdConfig	: Qmc5883L车辆离开参数配置
 @Input				CaroutThreshhold
 @Return				void
**********************************************************************************************************/
void Inspect_Qmc5883l_CaroutThreshholdConfig(unsigned char CaroutThreshhold)
{
	TCFG_SystemData.CaroutThreshhold = CaroutThreshhold;
	TCFG_EEPROM_SetCaroutThreshhold(TCFG_SystemData.CaroutThreshhold);
	InspectQmc5883lHandler.Configuration.carout_threshhold				= TCFG_SystemData.CaroutThreshhold;	//车辆离开参数
}

/**********************************************************************************************************
 @Function			void Inspect_Qmc5883l_RecalibrationNumConfig(unsigned char RecalibrationNum)
 @Description			Inspect_Qmc5883l_RecalibrationNumConfig	: Qmc5883L微小变化重计算次数配置
 @Input				RecalibrationNum
 @Return				void
**********************************************************************************************************/
void Inspect_Qmc5883l_RecalibrationNumConfig(unsigned char RecalibrationNum)
{
	if (RecalibrationNum > 15) {
		RecalibrationNum = 15;
	}
	
	TCFG_SystemData.RecalibrationNum = RecalibrationNum;
	TCFG_EEPROM_SetRecalibrationNum(TCFG_SystemData.RecalibrationNum);
	InspectQmc5883lHandler.Configuration.recalibration_num				= TCFG_SystemData.RecalibrationNum;	//微小变化重计算次数
}

/**********************************************************************************************************
 @Function			void Inspect_Qmc5883l_RecalibrationNumConfig(unsigned char RecalibrationNum)
 @Description			Inspect_Qmc5883l_RecalibrationNumConfig	: Qmc5883L激烈变化重计算时间(min)配置
 @Input				RecalibrationOvertime
 @Return				void
**********************************************************************************************************/
void Inspect_Qmc5883l_RecalibrationOvertimeConfig(unsigned char RecalibrationOvertime)
{
	if (RecalibrationOvertime > 15) {
		RecalibrationOvertime = 15;
	}
	
	TCFG_SystemData.RecalibrationOvertime = RecalibrationOvertime;
	TCFG_EEPROM_SetRecalibrationOverTime(TCFG_SystemData.RecalibrationOvertime);
	InspectQmc5883lHandler.Configuration.recalibration_overtime			= TCFG_SystemData.RecalibrationOvertime;//激烈变化重计算时间(min)
}

/**********************************************************************************************************
 @Function			void Inspect_Qmc5883l_ISR(void)
 @Description			Inspect_Qmc5883l_ISR		: Qmc5883L中断处理函数
 @Input				void
 @Return				void
**********************************************************************************************************/
void Inspect_Qmc5883l_ISR(void)
{
	InspectQmc5883lHandler.DataReady 								= INSPECT_QMC_DATAREADY;				//标记QMC数据准备完毕
}

/********************************************** END OF FLEE **********************************************/
