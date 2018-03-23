/**
  *********************************************************************************************************
  * @file    radar_api.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2017-10-31
  * @brief   
  *********************************************************************************************************
  * @attention
  *
  * 
  *
  *********************************************************************************************************
  */

#include "radar_api.h"
#include "radar_adc.h"
#include "radar_dac.h"
#include "radar_dma.h"
#include "radar_timer.h"
#include "hal_radar.h"
#include "hal_eeprom.h"
#include "stm32_dsp.h"
#include "stm32l1xx_config.h"
#include "platform_config.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "string.h"
#include <math.h>

#define SAMPLE_NUM	TRADAR_SAMPLE_NUM

#define	VDDA2V8														//VDDA = 2.8V 0~2633 -> 0~1.8V
//#define	VDDA3V3														//VDDA = 3.3V 0~2234 -> 0~1.8V
//#define	RADAR_VPTAT

#ifdef	VDDA3V3
#ifdef	RADAR_VPTAT
#define	RADER_LOW			650
#define	RADER_RANGE		7
#else
#define	RADER_LOW			50
#define	RADER_RANGE		6
#endif
#else
#define	RADER_LOW			50
#define	RADER_RANGE		8
#endif

u16 fre_magBG[TRADAR_BACKGROUND_NUM] = {10};

u32 sample_array0[SAMPLE_NUM] = {0};
u32 sample_array1[SAMPLE_NUM] = {0};

int n_array = 0;
int flag_in_array = 0;
int flag_main_go = 0;

u8  bgTimes = 1;
tradar_targetinfo_s targetinfo;

/**********************************************************************************************************
 @Function			void Rader_Init(void)
 @Description			雷达初始化
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radar_Init(void)
{
	Radar_DMA_Init();
	Radar_ADC_Init();
	Radar_DAC_Init();
	
	Radar_TIM2_Init(10-1, 32-1);																	//10us中断一次(32MHz)
	__HAL_TIM_DISABLE(&RADAR_TIM2_Handler);															//先关闭TIM2,等待使用雷达时开启
	
	Radar_DataPackInit();																		//初始化雷达接收队列
	
	FLASH_EEPROM_ReadBuffer(EEPROM_BASE_ADDR1, (u8 *)fre_magBG, sizeof(fre_magBG));							//读取EEPROM背景值
	
	tradar_background_set(fre_magBG, (sizeof(fre_magBG))/2);											//写入背景参数
}

/**********************************************************************************************************
 @Function			u8 Radar_CheckData(u32 datain_t[], u16 in_num)
 @Description			雷达检测数据
 @Input				datain_t		: sample data
					in_num		: the data number
 @Return				TRADAR_ERROR	: 异常
					TRADAR_OK 	: 正确
**********************************************************************************************************/
u8 Radar_CheckData(u32 datain_t[], u16 in_num)
{
	u16 i, count = 0;
	
	for (i = 0; i < in_num; i++) {
#ifdef VDDA2V8
		if ((datain_t[i]<2390) || (datain_t[i]>2480)) {
			count++;
		}
#else
		if ((datain_t[i]<2390) || (datain_t[i]>2470)) {
			count++;
		}
#endif
	}
	
	if (count < 10) {
		return TRADAR_ERROR;
	}
	else {
		return TRADAR_OK;
	}
}

/**********************************************************************************************************
 @Function			u8 Radar_InitBackGround(void)
 @Description			雷达初始化背景值
 @Input				void
 @Return				TRADAR_ERROR	: 异常
					TRADAR_OK 	: 正确
**********************************************************************************************************/
u8 Radar_InitBackGround(void)
{
	RADAR_ENTER_CRITICAL_SECTION();																//雷达启动初始化配置
	
	RADERPOWER(ON);																			//开启雷达电源
	Delay_MS(10);
	
	__HAL_TIM_ENABLE(&RADAR_TIM2_Handler);															//启动雷达定时器
	
	bgTimes = 31;
	
	while (1) {
		while (flag_main_go != 0) {
			if (Radar_CheckData(sample_array0, SAMPLE_NUM) == TRADAR_OK) {								//雷达数据校验
				if (bgTimes > 1) {
					if (flag_main_go == 1) {									
						tradar_background_transform(sample_array0,SAMPLE_NUM,fre_magBG,(sizeof(fre_magBG))/2);
					}
					else {
						tradar_background_transform(sample_array1,SAMPLE_NUM,fre_magBG,(sizeof(fre_magBG))/2);
					}
					
					bgTimes--;
					if (bgTimes == 1) {															//初始化背景完成
						FLASH_EEPROM_WriteBuffer(EEPROM_BASE_ADDR1, (u8 *)fre_magBG, sizeof(fre_magBG));	//背景值写入EEPROM
						tradar_background_set(fre_magBG,(sizeof(fre_magBG))/2);						//设置背景值
						flag_main_go = 0;
						
						__HAL_TIM_DISABLE(&RADAR_TIM2_Handler);										//雷达工作结束关闭定时器
						RADERPOWER(OFF);														//关闭雷达电源
						
						RADAR_EXIT_CRITICAL_SECTION();
						
						return TRADAR_OK;
					}
				}
			}
			else {
				__HAL_TIM_DISABLE(&RADAR_TIM2_Handler);												//雷达工作结束关闭定时器
				RADERPOWER(OFF);																//关闭雷达电源
				
				RADAR_EXIT_CRITICAL_SECTION();
				
				return TRADAR_ERROR;
			}
			
			flag_main_go = 0;
		}
	}
}

/**********************************************************************************************************
 @Function			u8 Radar_GetDataPack(u8 dataNum)
 @Description			获取雷达数据
 @Input				dataNum : 雷达数据条数
 @Return				TRADAR_ERROR	: 异常
					TRADAR_OK 	: 正确
**********************************************************************************************************/
u8 Radar_GetDataPack(u8 dataNum)
{
	u8 buf[RADAR_CACHE_SIZE];
	
	Radar_DataPackInit();																		//初始化雷达接收队列
	
	RADAR_ENTER_CRITICAL_SECTION();																//雷达启动初始化配置
	
	RADERPOWER(ON);																			//开启雷达电源
	Delay_MS(10);
	
	__HAL_TIM_ENABLE(&RADAR_TIM2_Handler);															//启动雷达定时器
	
	while (dataNum) {
		while (flag_main_go != 0) {
			if (Radar_CheckData(sample_array0, SAMPLE_NUM) == TRADAR_OK) {								//雷达数据校验
				if (bgTimes > 1) {
					if (flag_main_go == 1)
					{
						tradar_background_transform(sample_array0, SAMPLE_NUM, fre_magBG, (sizeof(fre_magBG))/2);
					}
					else
					{
						tradar_background_transform(sample_array1, SAMPLE_NUM, fre_magBG, (sizeof(fre_magBG))/2);
					}
					
					bgTimes--;
					if (bgTimes == 1) {															//初始化背景完成
						FLASH_EEPROM_WriteBuffer(EEPROM_BASE_ADDR1, (u8 *)fre_magBG, sizeof(fre_magBG));
						tradar_background_set(fre_magBG, (sizeof(fre_magBG))/2);
						flag_main_go = 0;
						
						__HAL_TIM_DISABLE(&RADAR_TIM2_Handler);										//雷达工作结束关闭定时器
						RADERPOWER(OFF);														//关闭雷达电源
						
						RADAR_EXIT_CRITICAL_SECTION();
						
						return TRADAR_ERROR;
					}
				}
				else if (flag_main_go == 1)
				{
					tradar_target_detect(sample_array0, SAMPLE_NUM, &targetinfo);
				}
				else
				{
					tradar_target_detect(sample_array1, SAMPLE_NUM, &targetinfo);
				}
			}
			else {
				__HAL_TIM_DISABLE(&RADAR_TIM2_Handler);												//雷达工作结束关闭定时器
				RADERPOWER(OFF);																//关闭雷达电源
				
				RADAR_EXIT_CRITICAL_SECTION();
				
				return TRADAR_ERROR;
			}
			
			if (targetinfo.status == TRADAR_BE_COVERED) {
				memset(buf, 0, RADAR_CACHE_SIZE);
				sprintf((char *)buf, "COVER !!!diff=%d", targetinfo.strenth_total_diff);
				Radar_DataPackEnqueue((u8 *)buf, strlen((char *)buf));									//雷达数据写入队列
				dataNum--;
			}
			else if (targetinfo.status == TRADAR_NO_TARGET) {
				memset(buf, 0, RADAR_CACHE_SIZE);
				sprintf((char *)buf, "No target ...diff=%d", targetinfo.strenth_total_diff);
				Radar_DataPackEnqueue((u8 *)buf, strlen((char *)buf));									//雷达数据写入队列
				dataNum--;
			}
			else if (targetinfo.status == TRADAR_HAS_TARGET) {
				memset(buf, 0, RADAR_CACHE_SIZE);
				sprintf((char *)buf, "dis=%d,mag=%d,diff=%d", targetinfo.distance_cm, targetinfo.signal_strength, targetinfo.strenth_total_diff);
				Radar_DataPackEnqueue((u8 *)buf, strlen((char *)buf));									//雷达数据写入队列
				dataNum--;
			}
			
			flag_main_go = 0;
		}
	}
	
	__HAL_TIM_DISABLE(&RADAR_TIM2_Handler);															//雷达工作结束关闭定时器
	RADERPOWER(OFF);																			//关闭雷达电源
	
	RADAR_EXIT_CRITICAL_SECTION();
	
	return TRADAR_OK;
}

/**********************************************************************************************************
 @Function			void Radar_RunTest(void)
 @Description			雷达运行测试
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radar_RunTest(void)
{
	RADAR_ENTER_CRITICAL_SECTION();																//雷达启动初始化配置
	
	Uart1_Init(115200);
	Uart2_Init(115200);
	
	RADERPOWER(ON);																			//开启雷达电源
	Delay_MS(10);
	
	__HAL_TIM_ENABLE(&RADAR_TIM2_Handler);															//启动雷达定时器
	
	while (1) {
		while (flag_main_go != 0) {
			if (bgTimes > 1) {
				if (flag_main_go == 1)
				{
					tradar_background_transform(sample_array0, SAMPLE_NUM, fre_magBG, (sizeof(fre_magBG))/2);
				}
				else
				{
					tradar_background_transform(sample_array1, SAMPLE_NUM, fre_magBG, (sizeof(fre_magBG))/2);
				}
				
				bgTimes--;
				if (bgTimes == 1) {																//初始化背景完成
					FLASH_EEPROM_WriteBuffer(EEPROM_BASE_ADDR1, (u8 *)fre_magBG, sizeof(fre_magBG));
					tradar_background_set(fre_magBG, (sizeof(fre_magBG))/2);
				}
			}
			else if (flag_main_go == 1)
			{
				tradar_target_detect(sample_array0, SAMPLE_NUM, &targetinfo);
			}
			else
			{
				tradar_target_detect(sample_array1, SAMPLE_NUM, &targetinfo);
			}
			if (targetinfo.status == TRADAR_BE_COVERED) {
				printf("COVER !!!diff=%d\n", targetinfo.strenth_total_diff);
			}
			else if (targetinfo.status == TRADAR_NO_TARGET) {
				printf("No target ...diff=%d\n", targetinfo.strenth_total_diff);
			}
			else if (targetinfo.status == TRADAR_HAS_TARGET) {
				printf("dis=%d.0 cm., mag=%d, diff=%d\n", targetinfo.distance_cm, targetinfo.signal_strength, targetinfo.strenth_total_diff);
			}
			flag_main_go = 0;
		}
	}
}

/**********************************************************************************************************
 @Function			void Radar_Get_Sample(void)
 @Description			雷达采样
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radar_Get_Sample(void)
{
	if (n_array >= SAMPLE_NUM) {
		if (flag_main_go != 0) {
			return;
		}
		n_array = 0;
		flag_main_go = 1 + flag_in_array;
		flag_in_array = 1 - flag_in_array;		
	}
	
	if (flag_in_array == 0) {
		
		sample_array0[n_array] = RADAR_ADC_ConvertedValue;
		
		/* 设置DAC通道值 */
		HAL_DAC_SetValue(&RADAR_DAC_Handler, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (SAMPLE_NUM - n_array) * RADER_RANGE + RADER_LOW);
		/* 启动DAC */
		HAL_DAC_Start(&RADAR_DAC_Handler, DAC_CHANNEL_1);
		
		n_array++;
	}
	else {
		sample_array1[n_array] = RADAR_ADC_ConvertedValue;
		
		/* 设置DAC通道值 */
		HAL_DAC_SetValue(&RADAR_DAC_Handler, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (SAMPLE_NUM - n_array) * RADER_RANGE + RADER_LOW);
		
		/* 启动DAC */
		HAL_DAC_Start(&RADAR_DAC_Handler, DAC_CHANNEL_1);
		
		n_array++;
	}
}

/**********************************************************************************************************
 @Function			void Radar_Get_Sample_Time(void)
 @Description			雷达采样节拍
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radar_Get_Sample_Time(void)
{
	static int xx = 0;
	
	if (xx < 1) {
		xx = 10;
		Radar_Get_Sample();
	}
	else {
		xx--;
	}
}

/**********************************************************************************************************
 @Function			void Radar_EnterCriticalSection(void)
 @Description			雷达启动初始化配置为32MHz HSI
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radar_EnterCriticalSection(void)
{
#ifndef SYSTEMCLOCK
	#error No Define SYSTEMCLOCK!
#else
#if (SYSTEMCLOCK == 4194000)
	Stm32_Clock_Init(RCC_PLLMUL_6, RCC_PLLDIV_3);													//设置时钟32Mhz
	Delay_Init(32000);																			//初始化延时
#endif
#endif
	
	n_array = 0;																				//清空状态
	flag_in_array = 0;
	flag_main_go = 0;
	
	Radar_DMA_Init();
	Radar_ADC_Init();
	Radar_DAC_Init();
}

/**********************************************************************************************************
 @Function			void Radar_EnterCriticalSection(void)
 @Description			雷达退出反初始化配置为原系统时钟MSI
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radar_ExitCriticalSection(void)
{
#ifndef SYSTEMCLOCK
	#error No Define SYSTEMCLOCK!
#else
#if (SYSTEMCLOCK == 4194000)
	Stm32_MSIClock_Init(RCC_MSIRANGE_6);															//设置时钟MSI->4.194MHz
	Delay_Init(4194);																			//初始化延时
#endif
#endif
	
	n_array = 0;																				//清空状态
	flag_in_array = 0;
	flag_main_go = 0;
	
	Radar_ADC_DeInit();
	Radar_DMA_DeInit();
	Radar_DAC_DeInit();
}

/********************************************** END OF FLEE **********************************************/
