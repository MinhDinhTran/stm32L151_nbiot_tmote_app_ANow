#ifndef __HAL_RADAR_H
#define   __HAL_RADAR_H

#include "sys.h"
#include "radar_api.h"

#define	RADAR_CACHE_SIZE		32										//�״����ݰ������С
#define	RADAR_DATA_PACK		12										//�״����ݰ�����

/* �״����ݽ��ն��� */
typedef struct
{
	u8 RADARData[RADAR_DATA_PACK][RADAR_CACHE_SIZE];							//��������
	u8 front;															//��ͷ
	u8 rear;															//��β
}RADAR_DataPack;
extern RADAR_DataPack radarDataPack;										//�״����ݽ��ն���

/* �״�������ȡ�ṹ�� */
typedef struct
{
	u8  NotargetNum;													//Notarget����
	u8  CoverNum;														//COVER����
	u8  DismagNum;														//Dismag����
	u32 DisVal;														//Ŀ��߶�ֵ
	u32 MagVal;														//Ŀ�����ֵ
	u32 Diff;															//�仯ֵ
}RADAR_DataStruct;
extern RADAR_DataStruct radarDataStruct;									//�״�������ȡ�ṹ��

void Radar_DataPackInit(void);											//��ʼ���״���ն���
u8   Radar_DataPackisFull(void);											//���DataPack�����Ƿ�����
u8   Radar_DataPackisEmpty(void);											//���DataPack�����Ƿ��ѿ�
void Radar_DataPackEnqueue(u8 *buf, u16 length);								//�״�����д�����
u8   Radar_DataPackDequeue(u8 *buf);										//�״����ݶ�������(����ͷƫ��1)
void Radar_DataPackToDataStruct(void);										//���״�������ȡ���״�ṹ��

#endif
