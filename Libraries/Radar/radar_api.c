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
 @Description			�״��ʼ��
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radar_Init(void)
{
	Radar_DMA_Init();
	Radar_ADC_Init();
	Radar_DAC_Init();
	
	Radar_TIM2_Init(10-1, 32-1);																	//10us�ж�һ��(32MHz)
	__HAL_TIM_DISABLE(&RADAR_TIM2_Handler);															//�ȹر�TIM2,�ȴ�ʹ���״�ʱ����
	
	Radar_DataPackInit();																		//��ʼ���״���ն���
	
	FLASH_EEPROM_ReadBuffer(EEPROM_BASE_ADDR1, (u8 *)fre_magBG, sizeof(fre_magBG));							//��ȡEEPROM����ֵ
	
	tradar_background_set(fre_magBG, (sizeof(fre_magBG))/2);											//д�뱳������
}

/**********************************************************************************************************
 @Function			u8 Radar_CheckData(u32 datain_t[], u16 in_num)
 @Description			�״�������
 @Input				datain_t		: sample data
					in_num		: the data number
 @Return				TRADAR_ERROR	: �쳣
					TRADAR_OK 	: ��ȷ
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
 @Description			�״��ʼ������ֵ
 @Input				void
 @Return				TRADAR_ERROR	: �쳣
					TRADAR_OK 	: ��ȷ
**********************************************************************************************************/
u8 Radar_InitBackGround(void)
{
	RADAR_ENTER_CRITICAL_SECTION();																//�״�������ʼ������
	
	RADERPOWER(ON);																			//�����״��Դ
	Delay_MS(10);
	
	__HAL_TIM_ENABLE(&RADAR_TIM2_Handler);															//�����״ﶨʱ��
	
	bgTimes = 31;
	
	while (1) {
		while (flag_main_go != 0) {
			if (Radar_CheckData(sample_array0, SAMPLE_NUM) == TRADAR_OK) {								//�״�����У��
				if (bgTimes > 1) {
					if (flag_main_go == 1) {									
						tradar_background_transform(sample_array0,SAMPLE_NUM,fre_magBG,(sizeof(fre_magBG))/2);
					}
					else {
						tradar_background_transform(sample_array1,SAMPLE_NUM,fre_magBG,(sizeof(fre_magBG))/2);
					}
					
					bgTimes--;
					if (bgTimes == 1) {															//��ʼ���������
						FLASH_EEPROM_WriteBuffer(EEPROM_BASE_ADDR1, (u8 *)fre_magBG, sizeof(fre_magBG));	//����ֵд��EEPROM
						tradar_background_set(fre_magBG,(sizeof(fre_magBG))/2);						//���ñ���ֵ
						flag_main_go = 0;
						
						__HAL_TIM_DISABLE(&RADAR_TIM2_Handler);										//�״﹤�������رն�ʱ��
						RADERPOWER(OFF);														//�ر��״��Դ
						
						RADAR_EXIT_CRITICAL_SECTION();
						
						return TRADAR_OK;
					}
				}
			}
			else {
				__HAL_TIM_DISABLE(&RADAR_TIM2_Handler);												//�״﹤�������رն�ʱ��
				RADERPOWER(OFF);																//�ر��״��Դ
				
				RADAR_EXIT_CRITICAL_SECTION();
				
				return TRADAR_ERROR;
			}
			
			flag_main_go = 0;
		}
	}
}

/**********************************************************************************************************
 @Function			u8 Radar_GetDataPack(u8 dataNum)
 @Description			��ȡ�״�����
 @Input				dataNum : �״���������
 @Return				TRADAR_ERROR	: �쳣
					TRADAR_OK 	: ��ȷ
**********************************************************************************************************/
u8 Radar_GetDataPack(u8 dataNum)
{
	u8 buf[RADAR_CACHE_SIZE];
	
	Radar_DataPackInit();																		//��ʼ���״���ն���
	
	RADAR_ENTER_CRITICAL_SECTION();																//�״�������ʼ������
	
	RADERPOWER(ON);																			//�����״��Դ
	Delay_MS(10);
	
	__HAL_TIM_ENABLE(&RADAR_TIM2_Handler);															//�����״ﶨʱ��
	
	while (dataNum) {
		while (flag_main_go != 0) {
			if (Radar_CheckData(sample_array0, SAMPLE_NUM) == TRADAR_OK) {								//�״�����У��
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
					if (bgTimes == 1) {															//��ʼ���������
						FLASH_EEPROM_WriteBuffer(EEPROM_BASE_ADDR1, (u8 *)fre_magBG, sizeof(fre_magBG));
						tradar_background_set(fre_magBG, (sizeof(fre_magBG))/2);
						flag_main_go = 0;
						
						__HAL_TIM_DISABLE(&RADAR_TIM2_Handler);										//�״﹤�������رն�ʱ��
						RADERPOWER(OFF);														//�ر��״��Դ
						
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
				__HAL_TIM_DISABLE(&RADAR_TIM2_Handler);												//�״﹤�������رն�ʱ��
				RADERPOWER(OFF);																//�ر��״��Դ
				
				RADAR_EXIT_CRITICAL_SECTION();
				
				return TRADAR_ERROR;
			}
			
			if (targetinfo.status == TRADAR_BE_COVERED) {
				memset(buf, 0, RADAR_CACHE_SIZE);
				sprintf((char *)buf, "COVER !!!diff=%d", targetinfo.strenth_total_diff);
				Radar_DataPackEnqueue((u8 *)buf, strlen((char *)buf));									//�״�����д�����
				dataNum--;
			}
			else if (targetinfo.status == TRADAR_NO_TARGET) {
				memset(buf, 0, RADAR_CACHE_SIZE);
				sprintf((char *)buf, "No target ...diff=%d", targetinfo.strenth_total_diff);
				Radar_DataPackEnqueue((u8 *)buf, strlen((char *)buf));									//�״�����д�����
				dataNum--;
			}
			else if (targetinfo.status == TRADAR_HAS_TARGET) {
				memset(buf, 0, RADAR_CACHE_SIZE);
				sprintf((char *)buf, "dis=%d,mag=%d,diff=%d", targetinfo.distance_cm, targetinfo.signal_strength, targetinfo.strenth_total_diff);
				Radar_DataPackEnqueue((u8 *)buf, strlen((char *)buf));									//�״�����д�����
				dataNum--;
			}
			
			flag_main_go = 0;
		}
	}
	
	__HAL_TIM_DISABLE(&RADAR_TIM2_Handler);															//�״﹤�������رն�ʱ��
	RADERPOWER(OFF);																			//�ر��״��Դ
	
	RADAR_EXIT_CRITICAL_SECTION();
	
	return TRADAR_OK;
}

/**********************************************************************************************************
 @Function			void Radar_RunTest(void)
 @Description			�״����в���
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radar_RunTest(void)
{
	RADAR_ENTER_CRITICAL_SECTION();																//�״�������ʼ������
	
	Uart1_Init(115200);
	Uart2_Init(115200);
	
	RADERPOWER(ON);																			//�����״��Դ
	Delay_MS(10);
	
	__HAL_TIM_ENABLE(&RADAR_TIM2_Handler);															//�����״ﶨʱ��
	
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
				if (bgTimes == 1) {																//��ʼ���������
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
 @Description			�״����
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
		
		/* ����DACͨ��ֵ */
		HAL_DAC_SetValue(&RADAR_DAC_Handler, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (SAMPLE_NUM - n_array) * RADER_RANGE + RADER_LOW);
		/* ����DAC */
		HAL_DAC_Start(&RADAR_DAC_Handler, DAC_CHANNEL_1);
		
		n_array++;
	}
	else {
		sample_array1[n_array] = RADAR_ADC_ConvertedValue;
		
		/* ����DACͨ��ֵ */
		HAL_DAC_SetValue(&RADAR_DAC_Handler, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (SAMPLE_NUM - n_array) * RADER_RANGE + RADER_LOW);
		
		/* ����DAC */
		HAL_DAC_Start(&RADAR_DAC_Handler, DAC_CHANNEL_1);
		
		n_array++;
	}
}

/**********************************************************************************************************
 @Function			void Radar_Get_Sample_Time(void)
 @Description			�״��������
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
 @Description			�״�������ʼ������Ϊ32MHz HSI
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radar_EnterCriticalSection(void)
{
#ifndef SYSTEMCLOCK
	#error No Define SYSTEMCLOCK!
#else
#if (SYSTEMCLOCK == 4194000)
	Stm32_Clock_Init(RCC_PLLMUL_6, RCC_PLLDIV_3);													//����ʱ��32Mhz
	Delay_Init(32000);																			//��ʼ����ʱ
#endif
#endif
	
	n_array = 0;																				//���״̬
	flag_in_array = 0;
	flag_main_go = 0;
	
	Radar_DMA_Init();
	Radar_ADC_Init();
	Radar_DAC_Init();
}

/**********************************************************************************************************
 @Function			void Radar_EnterCriticalSection(void)
 @Description			�״��˳�����ʼ������Ϊԭϵͳʱ��MSI
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radar_ExitCriticalSection(void)
{
#ifndef SYSTEMCLOCK
	#error No Define SYSTEMCLOCK!
#else
#if (SYSTEMCLOCK == 4194000)
	Stm32_MSIClock_Init(RCC_MSIRANGE_6);															//����ʱ��MSI->4.194MHz
	Delay_Init(4194);																			//��ʼ����ʱ
#endif
#endif
	
	n_array = 0;																				//���״̬
	flag_in_array = 0;
	flag_main_go = 0;
	
	Radar_ADC_DeInit();
	Radar_DMA_DeInit();
	Radar_DAC_DeInit();
}

/********************************************** END OF FLEE **********************************************/
