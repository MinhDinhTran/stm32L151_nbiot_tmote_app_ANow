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
	
	InspectQmc5883lHandler.Qmc5883lFile							= INSPECT_QMC_ERROR_NONE;
	InspectQmc5883lHandler.CarStatus								= INSPECT_CAR_NONE;
	
	/* 调用方法 */
	InspectQmc5883lHandler.PassDetect								= Inspect_Qmc5883l_Pass_Detect;		//车辆检测
	InspectQmc5883lHandler.CheckStatus								= Inspect_Qmc5883l_CheckStatus;		//检测状态
	InspectQmc5883lHandler.KmeansAllocate							= Inspect_Qmc5883l_KmeansAllocate;		//质心分配
	InspectQmc5883lHandler.BackgroundCalculate						= Inspect_Qmc5883l_BackgroundCalculate;	//背景计算
}

/**********************************************************************************************************
 @Function			void Inspect_Qmc5883l_ISR(void)
 @Description			Inspect_Qmc5883l_ISR		: Qmc5883L中断处理函数
 @Input				void
 @Return				void
**********************************************************************************************************/
void Inspect_Qmc5883l_ISR(void)
{
	InspectQmc5883lHandler.DataReady = INSPECT_QMC_DATAREADY;											//标记QMC数据准备完毕
}

/********************************************** END OF FLEE **********************************************/
