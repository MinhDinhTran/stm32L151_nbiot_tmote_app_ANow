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

RADAR_DataPack		radarDataPack;											//雷达数据接收队列
RADAR_DataStruct	radarDataStruct;										//雷达数据提取结构体

/**********************************************************************************************************
 @Function			void Radar_DataPackInit(void)
 @Description			初始化雷达接收队列
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
 @Description			检查DataPack队列是否已满
 @Input				void
 @Return				0  : 没满
					1  : 已满
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
 @Description			检查DataPack队列是否已空
 @Input				void
 @Return				0  : 队列有数据
					1  : 队列空数据
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
 @Description			雷达数据写入队列
 @Input				buf 		 : 数据
					length     : 数据长度
 @Return				void
**********************************************************************************************************/
void Radar_DataPackEnqueue(u8 *buf, u16 length)
{
	u8 i = 0;
	
	if (Radar_DataPackisFull()) {											//队列已满
		radarDataPack.rear = (radarDataPack.rear + 1) % RADAR_DATA_PACK;			//队尾偏移1
		memset((u8 *)radarDataPack.RADARData[radarDataPack.rear], 0x0, sizeof(radarDataPack.RADARData[radarDataPack.rear]));
		for (i = 0; i < length; i++) {
			radarDataPack.RADARData[radarDataPack.rear][i] = buf[i];			//写入队列数据
		}
		radarDataPack.front = (radarDataPack.front + 1) % RADAR_DATA_PACK;		//队头偏移1
	}
	else {															//队列未满
		radarDataPack.rear = (radarDataPack.rear + 1) % RADAR_DATA_PACK;			//队尾偏移1
		memset((u8 *)radarDataPack.RADARData[radarDataPack.rear], 0x0, sizeof(radarDataPack.RADARData[radarDataPack.rear]));
		for (i = 0; i < length; i++) {
			radarDataPack.RADARData[radarDataPack.rear][i] = buf[i];			//写入队列数据
		}
	}
}

/**********************************************************************************************************
 @Function			u8 Radar_DataPackDequeue(u8 *buf)
 @Description			雷达数据读出队列(队列头偏移1)
 @Input				&buf 	 : 数据地址
 @Return				0 	 	 : 读取成功
					1  		 : 队列已空
**********************************************************************************************************/
u8 Radar_DataPackDequeue(u8 *buf)
{
	if (Radar_DataPackisEmpty()) {										//队列已空
		return 1;
	}
	else {															//队列未空
		radarDataPack.front = (radarDataPack.front + 1) % RADAR_DATA_PACK;		//队头偏移1
		strcpy((char *)buf, (const char *)radarDataPack.RADARData[radarDataPack.front]);
		return 0;
	}
}

/**********************************************************************************************************
 @Function			void Radar_DataPackToDataStruct(void)
 @Description			将雷达数据提取到雷达结构体
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
