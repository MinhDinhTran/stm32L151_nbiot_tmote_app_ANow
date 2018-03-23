#ifndef __HAL_RTC_H
#define   __HAL_RTC_H

#include "sys.h"
#include <time.h>

#define RTCBulidTime		0											//RTC����ʱ��д��ʹ��

extern RTC_HandleTypeDef	RTC_Handler;										//RTC���

u8 RTC_Init(void);														//RTC��ʼ��

HAL_StatusTypeDef RTC_Set_Date(u8 year, u8 month, u8 date);						//RTC��������
HAL_StatusTypeDef RTC_Set_Time(u8 hour, u8 min, u8 sec);						//RTCʱ������

time_t RTC_TimeToStamp(u8 year, u8 month, u8 date, u8 hour, u8 min, u8 sec);		//ʱ��ת��ΪUNIXʱ���
time_t RTC_BulidTimeToStamp(void);											//����ʱ��ת��ΪUNIXʱ���
time_t RTC_GetUnixTimeToStamp(void);										//��ȡ��ǰUNIXʱ���
struct tm RTC_ConvUnixToCalendar(time_t t);									//ת��UNIXʱ���Ϊ����ʱ��

#endif
