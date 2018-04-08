#ifndef __INSPECT_CONFIG_H
#define   __INSPECT_CONFIG_H

#include "sys.h"

#define INSPECT_QMC_PROCESS_NUM			100												//处理数据量
#define INSPECT_QMC_DISTANCE_HIGH			20												//启动分类算法上限值
#define INSPECT_QMC_CNT_1_MIN				3  												//检测为有车状态的最少次数

enum
{
	INSPECT_QMC_DATAUNREADY				= 0x00,
	INSPECT_QMC_DATAREADY				= 0x01
};

enum
{
	INSPECT_QMC_ERROR_NONE				= 0x00,
	INSPECT_QMC_ERROR_IS				= 0x01
};

typedef enum
{
	INSPECT_CAR_NONE       				= 0x00,											//空状态
	INSPECT_CAR_COME    				= 0x01,											//车辆驶入
	INSPECT_CAR_GO						= 0x02											//车辆驶出
}Inspect_Qmc5883l_StatusTypeDef;

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

/* Inspect Qmc5883L MagVal Calculate */
typedef struct
{
	int16_t							x_now;											//X轴当前磁场值
	int16_t							y_now;											//Y轴当前磁场值
	int16_t							z_now;											//Z轴当前磁场值
	
	int16_t							x_back;											//X轴当前背景磁场值
	int16_t							y_back;											//Y轴当前背景磁场值
	int16_t							z_back;											//Z轴当前背景磁场值
	
	int32_t							x_sum;											//X轴地磁值累加
	int32_t							y_sum;											//Y轴地磁值累加
	int32_t							z_sum;											//Z轴地磁值累加
	int16_t							sum_num;											//已经累加地磁值的数量
	
	int32_t							x_back_sum;										//X轴稳定的背景值
	int32_t							y_back_sum;										//Y轴稳定的背景值
	int32_t							z_back_sum;										//Z轴稳定的背景值
	int16_t							sum_back_num;										//稳定的背景值累加数量
}Inspect_Qmc5883l_MagValCalculateTypeDef;

/* Inspect Qmc5883L Config */
typedef struct
{
	unsigned char						mag_measure_freq;									//地磁扫描频率 0->10, 1->50, 2->100, 3->200
	unsigned char						carin_threshhold;									//车辆进入参数
	unsigned char						carout_threshhold;									//车辆离开参数
	unsigned char						recalibration_num:4;								//微小变化重计算次数
	unsigned char						recalibration_overtime:4;							//激烈变化重计算时间
	unsigned short						recalibration_overtime_cnt;							//背景再计算累加器
	unsigned short						background_recalibration_seconds;						//记录上次背景校准之后时间
}Inspect_Qmc5883l_ConfigTypeDef;

/* Inspect Qmc5883L Parameter */
typedef struct
{
	unsigned short						carNumber;										//车辆次数统计
	unsigned short						diff_mag;											//变化值
	int16_t							back_x_mag;										//x轴背景值
	int16_t							back_y_mag;										//Y轴背景值
	int16_t							back_z_mag;										//Z轴背景值
}Inspect_Qmc5883l_ParameterTypeDef;

/* Inspect Qmc5883L */
typedef struct
{
	Inspect_Qmc5883l_DataTypeDef			Data[INSPECT_QMC_PROCESS_NUM];						//处理数据
	Inspect_Qmc5883l_ClusterGroupTypeDef	ClusterGroup[2];									//集群数据
	Inspect_Qmc5883l_ConfigTypeDef		Configuration;										//配置信息
	Inspect_Qmc5883l_MagValCalculateTypeDef	MagValCalculate;									//地磁数据计算值
	Inspect_Qmc5883l_ParameterTypeDef		Parameter;										//检测获取数据
	unsigned char						bInStatus;										//车辆状态值 1 : CarIn 0 : CarOut
	unsigned short						lastCenter;										//上次质心值
	unsigned char						minPos;											//数组最小值位置
	unsigned char						maxPos;											//数组最大值位置
	unsigned char						dataIndex;										//当前最新得到值的位置
	unsigned short						detectCnt;										//地磁扫描次数
	unsigned short						detect_nS0;										//连续状态0个数
	unsigned short						detect_nS1;										//连续状态1个数
	
	unsigned char						DataReady;										//数据准备标志位
	unsigned char						Qmc5883lFail;										//QMC5883L异常标志位
	Inspect_Qmc5883l_StatusTypeDef		CarStatus;										//检测状态
	
	Inspect_Qmc5883l_StatusTypeDef		(*PassDetect)(int16_t x_mag, int16_t y_mag, int16_t z_mag);	//车辆检测
	Inspect_Qmc5883l_StatusTypeDef		(*CheckStatus)(unsigned char index);					//检测状态
	unsigned char						(*KmeansAllocate)(void);								//质心分配
	unsigned char						(*BackgroundCalculate)(void);							//背景计算
}Inspect_Qmc5883lTypeDef;

extern Inspect_Qmc5883lTypeDef			InspectQmc5883lHandler;								//Inspect QMC5883L Handler

void Inspect_Qmc5883l_Init(void);															//Qmc5883L车辆检测初始化
void Inspect_Qmc5883l_SensitivityConfig(unsigned char Sensitivity);								//Qmc5883L灵敏度配置
void Inspect_Qmc5883l_MagFreqConfig(unsigned char Freq);										//Qmc5883L地磁扫描频率配置
void Inspect_Qmc5883l_ISR(void);															//Qmc5883L中断处理函数

#endif
