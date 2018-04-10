/**
  *********************************************************************************************************
  * @file    inspectqmc5883l.c
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

#include "inspectqmc5883l.h"
#include "platform_config.h"
#include "platform_map.h"
#include "stdlib.h"

/**********************************************************************************************************
 @Function			Inspect_Qmc5883l_StatusTypeDef Inspect_Qmc5883l_Pass_Detect(int16_t x_mag, int16_t y_mag, int16_t z_mag)
 @Description			Inspect_Qmc5883l_Pass_Detect		: 车辆检测(6~7MS)
 @Input				x_mag						: X轴磁场值
					y_mag						: Y轴磁场值
					z_mag						: Z轴磁场值
 @Return				Inspect_Qmc5883l_StatusTypeDef	: 检测状态
**********************************************************************************************************/
Inspect_Qmc5883l_StatusTypeDef Inspect_Qmc5883l_Pass_Detect(int16_t x_mag, int16_t y_mag, int16_t z_mag)
{
	Inspect_Qmc5883l_StatusTypeDef retStatus = INSPECT_CAR_NONE;
	s32 magSumS32 = 0;
	u16 magSumU16 = 0;
	u16 diff = 0;
	u16 threshhold = InspectQmc5883lHandler.Configuration.carin_threshhold;
	
	InspectQmc5883lHandler.MagValCalculate.x_now = x_mag;
	InspectQmc5883lHandler.MagValCalculate.y_now = y_mag;
	InspectQmc5883lHandler.MagValCalculate.z_now = z_mag;
	
	/* 车辆判断, 任意一个基准值如果有不为0的话, 就进行计算 */
	if ((InspectQmc5883lHandler.MagValCalculate.x_back != 0) || (InspectQmc5883lHandler.MagValCalculate.y_back != 0) || (InspectQmc5883lHandler.MagValCalculate.z_back != 0))
	{
		/* 已经有基准值 */
		/* 获取地磁平方和 */
		magSumS32 = (((int32_t)(InspectQmc5883lHandler.MagValCalculate.x_now - InspectQmc5883lHandler.MagValCalculate.x_back)) * 
				   ((int32_t)(InspectQmc5883lHandler.MagValCalculate.x_now - InspectQmc5883lHandler.MagValCalculate.x_back)) + 
				   ((int32_t)(InspectQmc5883lHandler.MagValCalculate.y_now - InspectQmc5883lHandler.MagValCalculate.y_back)) * 
				   ((int32_t)(InspectQmc5883lHandler.MagValCalculate.y_now - InspectQmc5883lHandler.MagValCalculate.y_back)) + 
				   ((int32_t)(InspectQmc5883lHandler.MagValCalculate.z_now - InspectQmc5883lHandler.MagValCalculate.z_back)) * 
				   ((int32_t)(InspectQmc5883lHandler.MagValCalculate.z_now - InspectQmc5883lHandler.MagValCalculate.z_back))) >> 6;
		
		if (magSumS32 > 0xFFFF) {
			magSumU16 = 0xFFFF;
		}
		else {
			magSumU16 = magSumS32;
		}
		
		/* 指针归0 */
		if (InspectQmc5883lHandler.dataIndex >= INSPECT_QMC_PROCESS_NUM) {
			InspectQmc5883lHandler.dataIndex = 0;
		}
		
		if ((InspectQmc5883lHandler.minPos >= INSPECT_QMC_PROCESS_NUM) || (InspectQmc5883lHandler.maxPos >= INSPECT_QMC_PROCESS_NUM)) {
			InspectQmc5883lHandler.minPos = InspectQmc5883lHandler.dataIndex;
			InspectQmc5883lHandler.maxPos = InspectQmc5883lHandler.dataIndex;
		}
		
		/* 限制过快增长 */
		if (InspectQmc5883lHandler.bInStatus) {
			/* 车辆进入状态 */
			if (magSumU16 > (InspectQmc5883lHandler.lastCenter + threshhold)) {
				magSumU16 = InspectQmc5883lHandler.lastCenter + threshhold;
			}
		}
		else {
			/* 车辆离开状态 */
			if (magSumU16 > (InspectQmc5883lHandler.lastCenter + 3 * threshhold / 2)) {
				magSumU16 = InspectQmc5883lHandler.lastCenter + 3 * threshhold / 2;
			}
		}
		
		/* 当前值比原来的最大值小, 原来最大值的位置就是当前要替换掉的位置
		 * 当前值比原来的最小值大, 原来最小值的位置就是当前要替换掉的位置
		 * 最大值的位置等于最小值的位置
		 */
		if (((InspectQmc5883lHandler.maxPos == InspectQmc5883lHandler.dataIndex) && (magSumU16 < InspectQmc5883lHandler.Data[InspectQmc5883lHandler.maxPos].sum)) ||
		    ((InspectQmc5883lHandler.minPos == InspectQmc5883lHandler.dataIndex) && (magSumU16 > InspectQmc5883lHandler.Data[InspectQmc5883lHandler.minPos].sum)) ||
		    (InspectQmc5883lHandler.maxPos == InspectQmc5883lHandler.minPos)) {
			/* 重新计算最大值和最小值的位置 */
			InspectQmc5883lHandler.Data[InspectQmc5883lHandler.dataIndex].sum = magSumU16;
			
			for (unsigned char i = 0; i < INSPECT_QMC_PROCESS_NUM; i++) {
				if (InspectQmc5883lHandler.Data[i].sum < InspectQmc5883lHandler.Data[InspectQmc5883lHandler.minPos].sum) {
					InspectQmc5883lHandler.minPos = i;
				}
				else if (InspectQmc5883lHandler.Data[i].sum > InspectQmc5883lHandler.Data[InspectQmc5883lHandler.maxPos].sum) {
					InspectQmc5883lHandler.maxPos = i;
				}
			}
		}
		else {
			/* 当前的值比原来的最大值还要大, 则最大值的位置更新为当前值的位置 */
			if (magSumU16 > InspectQmc5883lHandler.Data[InspectQmc5883lHandler.maxPos].sum) {
				InspectQmc5883lHandler.maxPos = InspectQmc5883lHandler.dataIndex;
			}
			/* 当前的值比原来的最小值还要小, 则最小值的位置更新为当前值的位置 */
			else if (magSumU16 < InspectQmc5883lHandler.Data[InspectQmc5883lHandler.minPos].sum) {
				InspectQmc5883lHandler.minPos = InspectQmc5883lHandler.dataIndex;
			}
			InspectQmc5883lHandler.Data[InspectQmc5883lHandler.dataIndex].sum = magSumU16;
		}
		
		if (magSumU16 < threshhold) {
			InspectQmc5883lHandler.Data[InspectQmc5883lHandler.dataIndex].flag = 0;
			diff = 0;
			InspectQmc5883lHandler.lastCenter = InspectQmc5883lHandler.Data[InspectQmc5883lHandler.minPos].sum;
		}
		else {
			/* K-Means算法, 重新定义质心 */
			/* 限制增长过快 */
			InspectQmc5883lHandler.ClusterGroup[0].center = InspectQmc5883lHandler.Data[InspectQmc5883lHandler.minPos].sum + 
												 ((InspectQmc5883lHandler.Data[InspectQmc5883lHandler.maxPos].sum - 
												   InspectQmc5883lHandler.Data[InspectQmc5883lHandler.minPos].sum) >> 2);
			
			InspectQmc5883lHandler.ClusterGroup[1].center = InspectQmc5883lHandler.Data[InspectQmc5883lHandler.maxPos].sum - 
												 ((InspectQmc5883lHandler.Data[InspectQmc5883lHandler.maxPos].sum - 
												   InspectQmc5883lHandler.Data[InspectQmc5883lHandler.minPos].sum) >> 2);
			
			/* 最小值和最大值之差大于阈值才有必要分类 */
			if ((InspectQmc5883lHandler.Data[InspectQmc5883lHandler.maxPos].sum - InspectQmc5883lHandler.Data[InspectQmc5883lHandler.minPos].sum) >= INSPECT_QMC_DISTANCE_HIGH) {
				for (unsigned char i = 1; i < 50; i++) {
					if (Inspect_Qmc5883l_KmeansAllocate() == 1) {
						break;
					}
				}
			}
			
			/* 特殊情况调整状态 */
			diff = abs(InspectQmc5883lHandler.ClusterGroup[0].center - InspectQmc5883lHandler.ClusterGroup[1].center);
			/* 如果没达到调整状态的变化值则保持同前面一致 */
			if (diff < threshhold) {
				if (InspectQmc5883lHandler.bInStatus) {
					InspectQmc5883lHandler.Data[InspectQmc5883lHandler.dataIndex].flag = 1;
				}
				else {
					InspectQmc5883lHandler.Data[InspectQmc5883lHandler.dataIndex].flag = 0;
				}
			}
			/* 如果分类后0类大于1类,作为异常处理 */
			else {
				/* 如果分类后0类大于1类,更新状态 */
				if (InspectQmc5883lHandler.ClusterGroup[0].center > InspectQmc5883lHandler.ClusterGroup[1].center) {
					for (unsigned char i = 0; i < INSPECT_QMC_PROCESS_NUM; i++) {
						if (InspectQmc5883lHandler.Data[i].flag == 0) {
							InspectQmc5883lHandler.Data[i].flag = 1;
						}
						else {
							InspectQmc5883lHandler.Data[i].flag = 0;
						}
					}
					InspectQmc5883lHandler.ClusterGroup[0].center = InspectQmc5883lHandler.ClusterGroup[1].center;
				}
			}
			InspectQmc5883lHandler.lastCenter = InspectQmc5883lHandler.ClusterGroup[0].center;
		}
		
		/* 状态机算法 */
		retStatus = Inspect_Qmc5883l_CheckStatus(InspectQmc5883lHandler.dataIndex);
		InspectQmc5883lHandler.dataIndex++;
		if (InspectQmc5883lHandler.dataIndex == INSPECT_QMC_PROCESS_NUM) {
			InspectQmc5883lHandler.dataIndex = 0;
		}
	}
	else {
		/* 还未有基准值 */
		InspectQmc5883lHandler.MagValCalculate.x_back = InspectQmc5883lHandler.MagValCalculate.x_now;
		InspectQmc5883lHandler.MagValCalculate.y_back = InspectQmc5883lHandler.MagValCalculate.y_now;
		InspectQmc5883lHandler.MagValCalculate.z_back = InspectQmc5883lHandler.MagValCalculate.z_now;
	}
	
	/* 背景值计算 */
	if (Inspect_Qmc5883l_BackgroundCalculate()) {
		/* 更新计算数组 */
		for (unsigned char i = 0; i < INSPECT_QMC_PROCESS_NUM; i++) {
			InspectQmc5883lHandler.Data[i].sum = 0;
		}
		InspectQmc5883lHandler.minPos = InspectQmc5883lHandler.maxPos = InspectQmc5883lHandler.dataIndex;
		InspectQmc5883lHandler.lastCenter = 0;
	}
	
	return retStatus;
}

/**********************************************************************************************************
 @Function			unsigned char Inspect_Qmc5883l_KmeansAllocate(void)
 @Description			Inspect_Qmc5883l_KmeansAllocate	: 质心分配
 @Input				void
 @Return				1							: 完成
					0							: 未完成
**********************************************************************************************************/
unsigned char Inspect_Qmc5883l_KmeansAllocate(void)
{
	unsigned char flag;
	unsigned int i, D1, D2;
	unsigned char j, k;
	unsigned long tempCenter;
	
	for (i = 0; i < 2; i++) {
		InspectQmc5883lHandler.ClusterGroup[i].groupSize = 0;
	}
	
	/* 按距离分配到各聚类域 */
	for (i = 0; i < INSPECT_QMC_PROCESS_NUM; i++) {
		D1 = abs(InspectQmc5883lHandler.ClusterGroup[0].center - InspectQmc5883lHandler.Data[i].sum);
		D2 = abs(InspectQmc5883lHandler.ClusterGroup[1].center - InspectQmc5883lHandler.Data[i].sum);
		
		k = 0;
		if (D1 > D2) {
			k = 1;
		}
		
		InspectQmc5883lHandler.Data[i].flag = k;
		InspectQmc5883lHandler.ClusterGroup[k].groupSize++;
	}
	
	flag = 1;
	/* 计算新的聚类中心 */
	for (j = 0; j < 2; j++) {
		tempCenter = 0;
		
		for (i = 0; i < INSPECT_QMC_PROCESS_NUM; i++) {
			if (InspectQmc5883lHandler.Data[i].flag == j) {
				tempCenter += InspectQmc5883lHandler.Data[i].sum;
			}
		}
		
		tempCenter /= InspectQmc5883lHandler.ClusterGroup[j].groupSize;
		
		if (tempCenter != InspectQmc5883lHandler.ClusterGroup[j].center) {
			InspectQmc5883lHandler.ClusterGroup[j].center = tempCenter;
			flag = 0;
		}
	}
	
	return flag;
}

/**********************************************************************************************************
 @Function			Inspect_Qmc5883l_StatusTypeDef Inspect_Qmc5883l_CheckStatus(unsigned char index)
 @Description			Inspect_Qmc5883l_CheckStatus		: 检查状态
 @Input				index
 @Return				Inspect_Qmc5883l_StatusTypeDef	: 检测状态
**********************************************************************************************************/
Inspect_Qmc5883l_StatusTypeDef Inspect_Qmc5883l_CheckStatus(unsigned char index)
{
	Inspect_Qmc5883l_StatusTypeDef status = INSPECT_CAR_NONE;
	
	/* 进入状态1 */
	if (InspectQmc5883lHandler.Data[index].flag == 1)
	{
		InspectQmc5883lHandler.detect_nS0 = 0;
		InspectQmc5883lHandler.detect_nS1++;
		/* 如果超过最小有车状态时间则判断为有车进入 */
		if (InspectQmc5883lHandler.detect_nS1 == INSPECT_QMC_CNT_1_MIN) {
			/* 车辆数加1 */
			InspectQmc5883lHandler.Parameter.carNumber++;
			TCFG_SystemData.CarNumber = InspectQmc5883lHandler.Parameter.carNumber;
			TCFG_EEPROM_SetCarNumber(TCFG_SystemData.CarNumber);
			
			InspectQmc5883lHandler.Parameter.diff_mag = 0;
			status = INSPECT_CAR_COME;
			InspectQmc5883lHandler.bInStatus = 1;
		}
		if (InspectQmc5883lHandler.Parameter.diff_mag < InspectQmc5883lHandler.Data[index].sum) {
			InspectQmc5883lHandler.Parameter.diff_mag = InspectQmc5883lHandler.Data[index].sum;
		}
	}
	/* 进入状态0 */
	else
	{
		/* 有车辆经过后, 进入状态0, 则计数 */
		if (InspectQmc5883lHandler.detect_nS1 >= INSPECT_QMC_CNT_1_MIN) {
			InspectQmc5883lHandler.detect_nS0++;
			/* 车辆进入后超过无车状态阈值, 清0, 判断为车辆离开 */
			if (InspectQmc5883lHandler.detect_nS0 > InspectQmc5883lHandler.Configuration.carout_threshhold) {
				status = INSPECT_CAR_GO;
				InspectQmc5883lHandler.bInStatus = 0;
				InspectQmc5883lHandler.detect_nS0 = 0;
				InspectQmc5883lHandler.detect_nS1 = 0;
			}
		}
		else {
			InspectQmc5883lHandler.detect_nS1 = 0;
		}
	}
	
	return status;
}

/**********************************************************************************************************
 @Function			unsigned char Inspect_Qmc5883l_BackgroundCalculate(void)
 @Description			Inspect_Qmc5883l_BackgroundCalculate	: 背景值计算
 @Input				void
 @Return				0								: 正常
					1								: 异常
**********************************************************************************************************/
unsigned char Inspect_Qmc5883l_BackgroundCalculate(void)
{
	unsigned char ret = 0;
	
	/*
	 * 如果无车, 则计算基准值
	 * 以上一次的基准值为基础, 稳定的作为新的基准值更新
	 * 考虑到特殊情况是, 环境巨变, 则如果稳定时间小于三为之一的计算之间, 按照最新的均值计算
	 * 后面是否考虑保存所有数组, 然后计算稳定的均值
	 */
	if (!InspectQmc5883lHandler.bInStatus)
	{
		/* 车辆离开 */
		if (InspectQmc5883lHandler.MagValCalculate.sum_num < InspectQmc5883lHandler.Configuration.recalibration_overtime_cnt) {
			/* N分钟采样次数 */
			if ((abs(InspectQmc5883lHandler.MagValCalculate.x_back - InspectQmc5883lHandler.MagValCalculate.x_now) < InspectQmc5883lHandler.Configuration.recalibration_num) &&
			    (abs(InspectQmc5883lHandler.MagValCalculate.y_back - InspectQmc5883lHandler.MagValCalculate.y_now) < InspectQmc5883lHandler.Configuration.recalibration_num) &&
			    (abs(InspectQmc5883lHandler.MagValCalculate.z_back - InspectQmc5883lHandler.MagValCalculate.z_now) < InspectQmc5883lHandler.Configuration.recalibration_num)) {
				InspectQmc5883lHandler.MagValCalculate.sum_back_num++;
				InspectQmc5883lHandler.MagValCalculate.x_back_sum += InspectQmc5883lHandler.MagValCalculate.x_now;
				InspectQmc5883lHandler.MagValCalculate.y_back_sum += InspectQmc5883lHandler.MagValCalculate.y_now;
				InspectQmc5883lHandler.MagValCalculate.z_back_sum += InspectQmc5883lHandler.MagValCalculate.z_now;
			}
			
			InspectQmc5883lHandler.MagValCalculate.sum_num++;
			InspectQmc5883lHandler.MagValCalculate.x_sum += InspectQmc5883lHandler.MagValCalculate.x_now;
			InspectQmc5883lHandler.MagValCalculate.y_sum += InspectQmc5883lHandler.MagValCalculate.y_now;
			InspectQmc5883lHandler.MagValCalculate.z_sum += InspectQmc5883lHandler.MagValCalculate.z_now;
		}
		else {
			/* N分钟计算一次 */
			/* 三分之一的累计采样时间 */
			if (InspectQmc5883lHandler.MagValCalculate.sum_back_num < (InspectQmc5883lHandler.Configuration.recalibration_overtime_cnt / 3)) {
				InspectQmc5883lHandler.MagValCalculate.x_back = InspectQmc5883lHandler.MagValCalculate.x_sum / InspectQmc5883lHandler.MagValCalculate.sum_num;
				InspectQmc5883lHandler.MagValCalculate.y_back = InspectQmc5883lHandler.MagValCalculate.y_sum / InspectQmc5883lHandler.MagValCalculate.sum_num;
				InspectQmc5883lHandler.MagValCalculate.z_back = InspectQmc5883lHandler.MagValCalculate.z_sum / InspectQmc5883lHandler.MagValCalculate.sum_num;
			}
			else {
				InspectQmc5883lHandler.MagValCalculate.x_back = InspectQmc5883lHandler.MagValCalculate.x_back_sum / InspectQmc5883lHandler.MagValCalculate.sum_back_num;
				InspectQmc5883lHandler.MagValCalculate.y_back = InspectQmc5883lHandler.MagValCalculate.y_back_sum / InspectQmc5883lHandler.MagValCalculate.sum_back_num;
				InspectQmc5883lHandler.MagValCalculate.z_back = InspectQmc5883lHandler.MagValCalculate.z_back_sum / InspectQmc5883lHandler.MagValCalculate.sum_back_num;
			}
			
			InspectQmc5883lHandler.MagValCalculate.sum_num = 0;
			InspectQmc5883lHandler.MagValCalculate.x_sum = 0;
			InspectQmc5883lHandler.MagValCalculate.y_sum = 0;
			InspectQmc5883lHandler.MagValCalculate.z_sum = 0;
			
			InspectQmc5883lHandler.MagValCalculate.sum_back_num = 0;
			InspectQmc5883lHandler.MagValCalculate.x_back_sum = 0;
			InspectQmc5883lHandler.MagValCalculate.y_back_sum = 0;
			InspectQmc5883lHandler.MagValCalculate.z_back_sum = 0;
			
			/* 更新计算数组 */
			for (unsigned char i = 0; i < INSPECT_QMC_PROCESS_NUM; i++) {
				InspectQmc5883lHandler.Data[i].sum = 0;
			}
			InspectQmc5883lHandler.maxPos = InspectQmc5883lHandler.minPos = InspectQmc5883lHandler.dataIndex;
			InspectQmc5883lHandler.lastCenter = 0;
		}
		
		InspectQmc5883lHandler.Configuration.background_recalibration_seconds = 0;
	}
	else
	{
		/* 车辆进入 */
		/* 如果连续有车 N 分钟, 则认为出错, 认定车辆离开 */
		if (InspectQmc5883lHandler.Configuration.background_recalibration_seconds >= (InspectQmc5883lHandler.Configuration.recalibration_overtime * 60)) {
			ret = 1;
			
			/* 以当前值作为基准值 */
			InspectQmc5883lHandler.MagValCalculate.x_back = InspectQmc5883lHandler.MagValCalculate.x_now;
			InspectQmc5883lHandler.MagValCalculate.y_back = InspectQmc5883lHandler.MagValCalculate.y_now;
			InspectQmc5883lHandler.MagValCalculate.z_back = InspectQmc5883lHandler.MagValCalculate.z_now;
			
			InspectQmc5883lHandler.MagValCalculate.sum_num = 0;
			InspectQmc5883lHandler.MagValCalculate.x_sum = 0;
			InspectQmc5883lHandler.MagValCalculate.y_sum = 0;
			InspectQmc5883lHandler.MagValCalculate.z_sum = 0;
			
			InspectQmc5883lHandler.MagValCalculate.sum_back_num = 0;
			InspectQmc5883lHandler.MagValCalculate.x_back_sum = 0;
			InspectQmc5883lHandler.MagValCalculate.y_back_sum = 0;
			InspectQmc5883lHandler.MagValCalculate.z_back_sum = 0;
		}
	}
	
	return ret;
}

/**********************************************************************************************************
 @Function			unsigned char Inspect_Qmc5883l_BackgroundCalculate(void)
 @Description			Inspect_Qmc5883l_BackgroundCalculate	: 背景值重新计算
 @Input				void
 @Return				0								: 正常
					1								: 异常
**********************************************************************************************************/
unsigned char Inspect_Qmc5883l_BackgroundDoCalculate(void)
{
	unsigned char ret = 1;
	
	InspectQmc5883lHandler.Configuration.background_recalibration_seconds += InspectQmc5883lHandler.Configuration.recalibration_overtime * 60;
	
	/* 背景值计算 */
	if (Inspect_Qmc5883l_BackgroundCalculate()) {
		/* 更新计算数组 */
		for (unsigned char i = 0; i < INSPECT_QMC_PROCESS_NUM; i++) {
			InspectQmc5883lHandler.Data[i].sum = 0;
		}
		InspectQmc5883lHandler.minPos = InspectQmc5883lHandler.maxPos = InspectQmc5883lHandler.dataIndex;
		InspectQmc5883lHandler.lastCenter = 0;
		
		ret = 0;
	}
	
	return ret;
}

/********************************************** END OF FLEE **********************************************/
