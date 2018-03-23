/**
  *********************************************************************************************************
  * @file    hal_radar.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2017-10-26
  * @brief   RADAR Device
  *********************************************************************************************************
  * @attention
  *			
  *
  *
  *********************************************************************************************************
  */

#include "hal_radar.h"
#include "string.h"

RADAR_DataPack		radarDataPack;											//�״����ݽ��ն���
RADAR_DataStruct	radarDataStruct;										//�״�������ȡ�ṹ��

/**********************************************************************************************************
 @Function			void Radar_DataPackInit(void)
 @Description			��ʼ���״���ն���
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radar_DataPackInit(void)
{
	radarDataPack.front = 0;
	radarDataPack.rear = 0;
	memset((u8 *)radarDataPack.RADARData, 0x0, sizeof(radarDataPack.RADARData));
}

/**********************************************************************************************************
 @Function			u8 Radar_DataPackisFull(void)
 @Description			���DataPack�����Ƿ�����
 @Input				void
 @Return				0  : û��
					1  : ����
**********************************************************************************************************/
u8 Radar_DataPackisFull(void)
{
	if ((radarDataPack.rear + 1) % RADAR_DATA_PACK == radarDataPack.front) {
		return 1;
	}
	else {
		return 0;
	}
}

/**********************************************************************************************************
 @Function			u8 Radar_DataPackisEmpty(void)
 @Description			���DataPack�����Ƿ��ѿ�
 @Input				void
 @Return				0  : ����������
					1  : ���п�����
**********************************************************************************************************/
u8 Radar_DataPackisEmpty(void)
{
	if (radarDataPack.front == radarDataPack.rear) {
		return 1;
	}
	else {
		return 0;
	}
}

/**********************************************************************************************************
 @Function			void Radar_DataPackEnqueue(u8 *buf, u16 length)
 @Description			�״�����д�����
 @Input				buf 		 : ����
					length     : ���ݳ���
 @Return				void
**********************************************************************************************************/
void Radar_DataPackEnqueue(u8 *buf, u16 length)
{
	u8 i = 0;
	
	if (Radar_DataPackisFull()) {											//��������
		radarDataPack.rear = (radarDataPack.rear + 1) % RADAR_DATA_PACK;			//��βƫ��1
		memset((u8 *)radarDataPack.RADARData[radarDataPack.rear], 0x0, sizeof(radarDataPack.RADARData[radarDataPack.rear]));
		for (i = 0; i < length; i++) {
			radarDataPack.RADARData[radarDataPack.rear][i] = buf[i];			//д���������
		}
		radarDataPack.front = (radarDataPack.front + 1) % RADAR_DATA_PACK;		//��ͷƫ��1
	}
	else {															//����δ��
		radarDataPack.rear = (radarDataPack.rear + 1) % RADAR_DATA_PACK;			//��βƫ��1
		memset((u8 *)radarDataPack.RADARData[radarDataPack.rear], 0x0, sizeof(radarDataPack.RADARData[radarDataPack.rear]));
		for (i = 0; i < length; i++) {
			radarDataPack.RADARData[radarDataPack.rear][i] = buf[i];			//д���������
		}
	}
}

/**********************************************************************************************************
 @Function			u8 Radar_DataPackDequeue(u8 *buf)
 @Description			�״����ݶ�������(����ͷƫ��1)
 @Input				&buf 	 : ���ݵ�ַ
 @Return				0 	 	 : ��ȡ�ɹ�
					1  		 : �����ѿ�
**********************************************************************************************************/
u8 Radar_DataPackDequeue(u8 *buf)
{
	if (Radar_DataPackisEmpty()) {										//�����ѿ�
		return 1;
	}
	else {															//����δ��
		radarDataPack.front = (radarDataPack.front + 1) % RADAR_DATA_PACK;		//��ͷƫ��1
		strcpy((char *)buf, (const char *)radarDataPack.RADARData[radarDataPack.front]);
		return 0;
	}
}

/**********************************************************************************************************
 @Function			void Radar_DataPackToDataStruct(void)
 @Description			���״�������ȡ���״�ṹ��
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radar_DataPackToDataStruct(void)
{
	int dis = 0;
	int mag = 0;
	int diff = 0;
	u8 buf[RADAR_CACHE_SIZE];
	
	memset((u8 *)buf, 0x0, sizeof(buf));
	memset((u8 *)&radarDataStruct, 0x0, sizeof(radarDataStruct));
	while (Radar_DataPackDequeue(buf) != 1) {
		if (strncmp((const char *)buf, (const char *)"No target ...", 13) == 0) {
			radarDataStruct.NotargetNum += 1;
			sscanf((const char *)buf, (const char *)"No target ...diff=%d", &diff);
			radarDataStruct.Diff += diff;
		}
		else if (strncmp((const char *)buf, (const char *)"COVER !!!", 9) == 0) {
			radarDataStruct.CoverNum += 1;
			sscanf((const char *)buf, (const char *)"COVER !!!diff=%d", &diff);
			radarDataStruct.Diff += diff;
		}
		else if (strncmp((const char *)buf, (const char *)"dis=", 4) == 0) {
			radarDataStruct.DismagNum += 1;
			sscanf((const char *)buf, (const char *)"dis=%d,mag=%d,diff=%d", &dis, &mag, &diff);
			radarDataStruct.DisVal += dis;
			radarDataStruct.MagVal += mag;
			radarDataStruct.Diff += diff;
		}
		memset((u8 *)buf, 0x0, sizeof(buf));
	}
	
	if (radarDataStruct.DismagNum != 0) {
		radarDataStruct.DisVal /= radarDataStruct.DismagNum;
		radarDataStruct.MagVal /= radarDataStruct.DismagNum;
	}
	if ((radarDataStruct.NotargetNum != 0) || (radarDataStruct.CoverNum != 0) || (radarDataStruct.DismagNum != 0)) {
		radarDataStruct.Diff /= (radarDataStruct.DismagNum + radarDataStruct.NotargetNum + radarDataStruct.CoverNum);
	}
}

/********************************************** END OF FLEE **********************************************/
