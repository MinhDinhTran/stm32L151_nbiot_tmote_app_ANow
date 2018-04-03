#ifndef __INSPECT_CONFIG_H
#define   __INSPECT_CONFIG_H

#include "sys.h"

#define INSPECT_QMC_PROCESS_NUM			100												//����������

/* Inspect Qmc5883L Data */
typedef struct
{
	unsigned short						sum : 15;											//���ݺ�
	unsigned char						flag : 1;											//��־λ
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
	Inspect_Qmc5883l_DataTypeDef			Data[INSPECT_QMC_PROCESS_NUM];						//��������
	Inspect_Qmc5883l_ClusterGroupTypeDef	ClusterGroup[2];									//��Ⱥ����
	
	
	
	
	
}Inspect_Qmc5883lTypeDef;

























#endif
