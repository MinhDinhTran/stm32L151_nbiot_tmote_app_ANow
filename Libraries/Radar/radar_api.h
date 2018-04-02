#ifndef __RADAR_API_H
#define   __RADAR_API_H

#include "sys.h"
#include "tmesh_radar_algorithm.h"

#define RADAR_ENTER_CRITICAL_SECTION()		Radar_EnterCriticalSection();
#define RADAR_EXIT_CRITICAL_SECTION()		Radar_ExitCriticalSection();

void Radar_Init(void);													//雷达初始化
u8   Radar_InitBackGround(void);											//雷达初始化背景值
u8   Radar_GetDataPack(u8 dataNum);										//获取雷达数据
void Radar_RunTest(void);												//雷达运行测试
void Radar_Get_Sample(void);												//雷达采样
void Radar_Get_Sample_Time(void);											//雷达采样节拍

u8   Radar_CheckData(u32 datain_t[], u16 in_num);								//雷达检测数据

void Radar_EnterCriticalSection(void);										//雷达启动初始化配置为32MHz HSI
void Radar_ExitCriticalSection(void);										//雷达退出反初始化配置为原系统时钟MSI

#endif
