#ifndef __INSPECT_CONFIG_H
#define   __INSPECT_CONFIG_H

#include "sys.h"

#define INSPECT_QMC_PROCESS_NUM			100												//处理数据量

/* Inspect Qmc5883L Data */
typedef struct
{
	unsigned short						sum : 15;											//数据和
	unsigned char						flag : 1;											//标志位
}Inspect_Qmc5883l_DataTypeDef;

/* Inspect Qmc5883L Cluster Group */
typedef struct
{
	unsigned short						center;
	unsigned char						groupSize;
}Inspect_Qmc5883l_ClusterGroupTypeDef;

/* Inspect Qmc5883L */
typedef struct
{
	Inspect_Qmc5883l_DataTypeDef			Data[INSPECT_QMC_PROCESS_NUM];						//处理数据
	Inspect_Qmc5883l_ClusterGroupTypeDef	ClusterGroup[2];									//集群数据
	
	
	
	
	
}Inspect_Qmc5883lTypeDef;

























#endif
