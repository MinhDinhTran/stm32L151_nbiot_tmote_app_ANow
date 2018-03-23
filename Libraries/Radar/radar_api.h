#ifndef __RADAR_API_H
#define   __RADAR_API_H

#include "sys.h"
#include "tmesh_radar_algorithm.h"

#define RADAR_ENTER_CRITICAL_SECTION()		Radar_EnterCriticalSection();
#define RADAR_EXIT_CRITICAL_SECTION()		Radar_ExitCriticalSection();

void Radar_Init(void);													//�״��ʼ��
u8   Radar_InitBackGround(void);											//�״��ʼ������ֵ
u8   Radar_GetDataPack(u8 dataNum);										//��ȡ�״�����
void Radar_RunTest(void);												//�״����в���
void Radar_Get_Sample(void);												//�״����
void Radar_Get_Sample_Time(void);											//�״��������

u8   Radar_CheckData(u32 datain_t[], u16 in_num);								//�״�������

void Radar_EnterCriticalSection(void);										//�״�������ʼ������Ϊ32MHz HSI
void Radar_ExitCriticalSection(void);										//�״��˳�����ʼ������Ϊԭϵͳʱ��MSI

#endif
