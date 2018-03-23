#ifndef __HAL_RADAR_H
#define   __HAL_RADAR_H

#include "sys.h"
#include "radar_api.h"

#define	RADAR_CACHE_SIZE		32										//雷达数据包缓存大小
#define	RADAR_DATA_PACK		12										//雷达数据包个数

/* 雷达数据接收队列 */
typedef struct
{
	u8 RADARData[RADAR_DATA_PACK][RADAR_CACHE_SIZE];							//队中数据
	u8 front;															//队头
	u8 rear;															//队尾
}RADAR_DataPack;
extern RADAR_DataPack radarDataPack;										//雷达数据接收队列

/* 雷达数据提取结构体 */
typedef struct
{
	u8  NotargetNum;													//Notarget包数
	u8  CoverNum;														//COVER包数
	u8  DismagNum;														//Dismag包数
	u32 DisVal;														//目标高度值
	u32 MagVal;														//目标幅度值
	u32 Diff;															//变化值
}RADAR_DataStruct;
extern RADAR_DataStruct radarDataStruct;									//雷达数据提取结构体

void Radar_DataPackInit(void);											//初始化雷达接收队列
u8   Radar_DataPackisFull(void);											//检查DataPack队列是否已满
u8   Radar_DataPackisEmpty(void);											//检查DataPack队列是否已空
void Radar_DataPackEnqueue(u8 *buf, u16 length);								//雷达数据写入队列
u8   Radar_DataPackDequeue(u8 *buf);										//雷达数据读出队列(队列头偏移1)
void Radar_DataPackToDataStruct(void);										//将雷达数据提取到雷达结构体

#endif
