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

#include "hal_beep.h"

Inspect_Qmc5883lTypeDef		InspectQmc5883lHandler;							//Inspect QMC5883L Handler

/**********************************************************************************************************
 @Function			void Inspect_Qmc5883l_Init(void)
 @Description			Inspect_Qmc5883l_Init		: Qmc5883L车辆检测初始化
 @Input				void
 @Return				void
**********************************************************************************************************/
void Inspect_Qmc5883l_Init(void)
{
	InspectQmc5883lHandler.Configuration.mag_measure_freq				= 0;								//地磁扫描频率
	InspectQmc5883lHandler.Configuration.carin_threshhold				= 80;							//车辆进入参数
	InspectQmc5883lHandler.Configuration.carout_threshhold				= 13;							//车辆离开参数
	InspectQmc5883lHandler.Configuration.recalibration_num				= 10;							//微小变化重计算次数
	InspectQmc5883lHandler.Configuration.recalibration_overtime			= 1;								//激烈变化重计算时间(min)
	InspectQmc5883lHandler.Configuration.recalibration_overtime_cnt		= 3000;							//背景再计算累加器
	InspectQmc5883lHandler.Configuration.background_recalibration_seconds	= 0;								//记录上次背景校准之后时间
	
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
	//Debug
	
	QMC5883L_ReadData_Simplify();
	
	Inspect_Qmc5883l_StatusTypeDef retqmc = INSPECT_CAR_NONE;
	
	retqmc = InspectQmc5883lHandler.PassDetect(Qmc5883lData.X_Now, Qmc5883lData.Y_Now, Qmc5883lData.Z_Now);
	if (retqmc == INSPECT_CAR_COME) {
		BEEP_CtrlRepeat(2, 25);
	}
	else if (retqmc == INSPECT_CAR_GO) {
		BEEP_CtrlRepeat(1, 25);
	}
	
	
	
	
	
}


















/********************************************** END OF FLEE **********************************************/
