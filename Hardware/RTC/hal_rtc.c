/**
  *********************************************************************************************************
  * @file    hal_rtc.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2017-10-24
  * @brief   HAL RTC Device
  *********************************************************************************************************
  * @attention
  *			
  *
  *
  *********************************************************************************************************
  */

#include "hal_rtc.h"
#include "string.h"

RTC_HandleTypeDef RTC_Handler;											//RTC���

/**********************************************************************************************************
 @Function			u8 RTC_Init(void)
 @Description			RTC��ʼ��
 @Input				void
 @Return				0 : ��ʼ���ɹ�
					2 : �����ʼ��ģʽʧ��
**********************************************************************************************************/
u8 RTC_Init(void)
{
#if RTCBulidTime
	struct tm datetime;
#endif
	
	RTC_Handler.Instance = RTC;
	RTC_Handler.Init.HourFormat = RTC_HOURFORMAT_24;							//RTC����Ϊ24Сʱ��ʽ
	RTC_Handler.Init.AsynchPrediv = 0X7F;									//RTC�첽��Ƶϵ��(1~0X7F)
	RTC_Handler.Init.SynchPrediv = 0XFF;									//RTCͬ����Ƶϵ��(0~7FFF)
	RTC_Handler.Init.OutPut = RTC_OUTPUT_DISABLE;
	RTC_Handler.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	RTC_Handler.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	if (HAL_RTC_Init(&RTC_Handler) != HAL_OK) return 2;
	
	if (HAL_RTCEx_BKUPRead(&RTC_Handler, RTC_BKP_DR0) != 0X5050)				//�Ƿ��һ������
	{
#if RTCBulidTime
		datetime = RTC_ConvUnixToCalendar(RTC_BulidTimeToStamp());				//��ȡϵͳ����ʱ��
		RTC_Set_Date(datetime.tm_year, datetime.tm_mon, datetime.tm_mday);		//��������
		RTC_Set_Time(datetime.tm_hour, datetime.tm_min, datetime.tm_sec);		//����ʱ��
#else
		RTC_Set_Date(18, 01, 01);										//��������
		RTC_Set_Time(12, 00, 00);										//����ʱ��
#endif
		HAL_RTCEx_BKUPWrite(&RTC_Handler, RTC_BKP_DR0, 0X5050);				//����Ѿ���ʼ������
	}
	
	return 0;
}

/**********************************************************************************************************
 @Function			HAL_StatusTypeDef RTC_Set_Time(u8 hour, u8 min, u8 sec)
 @Description			RTCʱ������
 @Input				hour, min, sec : Сʱ, ����, ����
 @Return				SUCEE(1) : �ɹ�
					ERROR(0) : �����ʼ��ģʽʧ��
**********************************************************************************************************/
HAL_StatusTypeDef RTC_Set_Time(u8 hour, u8 min, u8 sec)
{
	RTC_TimeTypeDef RTC_TimeStructure;
	
	RTC_TimeStructure.Hours = hour;
	RTC_TimeStructure.Minutes = min;
	RTC_TimeStructure.Seconds = sec;
	RTC_TimeStructure.TimeFormat = (hour < 12)?RTC_HOURFORMAT12_AM:RTC_HOURFORMAT12_PM;
	RTC_TimeStructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	RTC_TimeStructure.StoreOperation = RTC_STOREOPERATION_RESET;
	
	return HAL_RTC_SetTime(&RTC_Handler, &RTC_TimeStructure, RTC_FORMAT_BIN);
}

/**********************************************************************************************************
 @Function			HAL_StatusTypeDef RTC_Set_Date(u8 year, u8 month, u8 date)
 @Description			RTC��������
 @Input				year, month, date 	: ��(0~99), ��(1~12), ��(0~31)
 @Return				SUCEE(1) : �ɹ�
					ERROR(0) : �����ʼ��ģʽʧ��
**********************************************************************************************************/
HAL_StatusTypeDef RTC_Set_Date(u8 year, u8 month, u8 date)
{
	u8 week;
	RTC_DateTypeDef RTC_DateStructure;
	
	RTC_DateStructure.Date = date;
	RTC_DateStructure.Month = month;
	RTC_DateStructure.Year = year;
	if (month == 1 || month == 2) {
		month += 12;
		year--;
	}
	week = (date+2*month+3*(month+1)/5+year+year/4-year/100+year/400+1)%7;
	RTC_DateStructure.WeekDay = (week == 0) ? 7 : week;
	
	return HAL_RTC_SetDate(&RTC_Handler, &RTC_DateStructure, RTC_FORMAT_BIN);
}

/**********************************************************************************************************
 @Function			time_t RTC_TimeToStamp(u8 year, u8 month, u8 date, u8 hour, u8 min, u8 sec)
 @Description			ʱ��ת��ΪUNIXʱ���
 @Input				year, month, date 	: ��(0~99), ��(1~12), ��(0~31)
					hour, min, sec 	: Сʱ, ����, ����
 @Return				time_t			: ʱ���
**********************************************************************************************************/
time_t RTC_TimeToStamp(u8 year, u8 month, u8 date, u8 hour, u8 min, u8 sec)
{
	struct tm time;
	
	time.tm_year = 100 + year;											//1900�굽��ǰ��ݲ�
	time.tm_mon = month - 1;												//0~11 ��ʾ 1~12��
	time.tm_mday = date;
	time.tm_hour = hour;
	time.tm_min = min;
	time.tm_sec = sec;
	
	return mktime(&time);
}

/**********************************************************************************************************
 @Function			time_t RTC_BulidTimeToStamp(void)
 @Description			����ʱ��ת��ΪUNIXʱ���
 @Input				void
 @Return				time_t			: ʱ���
**********************************************************************************************************/
time_t RTC_BulidTimeToStamp(void)
{
	int year, month, date, hour, min, sec;
	char bulidData[12];
	char bulidTime[9];
	struct tm time;
	
	memset(bulidData, 0x0, sizeof(bulidData));
	memset(bulidTime, 0x0, sizeof(bulidTime));
	sprintf(bulidData, "%s", __DATE__);
	sprintf(bulidTime, "%s", __TIME__);
	
	if (!memcmp(&bulidData[0], "Jan", 3)) {
		month = 1;
	}
	else if (!memcmp(&bulidData[0], "Feb", 3)) {
		month = 2;
	}
	else if (!memcmp(&bulidData[0], "Mar", 3)) {
		month = 3;
	}
	else if (!memcmp(&bulidData[0], "Apr", 3)) {
		month = 4;
	}
	else if (!memcmp(&bulidData[0], "May", 3)) {
		month = 5;
	}
	else if (!memcmp(&bulidData[0], "Jun", 3)) {
		month = 6;
	}
	else if (!memcmp(&bulidData[0], "Jul", 3)) {
		month = 7;
	}
	else if (!memcmp(&bulidData[0], "Aug", 3)) {
		month = 8;
	}
	else if (!memcmp(&bulidData[0], "Sep", 3)) {
		month = 9;
	}
	else if (!memcmp(&bulidData[0], "Oct", 3)) {
		month = 10;
	}
	else if (!memcmp(&bulidData[0], "Nov", 3)) {
		month = 11;
	}
	else if (!memcmp(&bulidData[0], "Dec", 3)) {
		month = 12;
	}
	
	sscanf((const char *)&bulidData[4], "%d %d", &date, &year);
	sscanf((const char *)&bulidTime[0], "%d:%d:%d", &hour, &min, &sec);
	
	time.tm_year = year - 1900;
	time.tm_mon = month - 1;
	time.tm_mday = date;
	time.tm_hour = hour;
	time.tm_min = min;
	time.tm_sec = sec;
	
	return mktime(&time);
}

/**********************************************************************************************************
 @Function			struct tm RTC_ConvUnixToCalendar(time_t t)
 @Description			ת��UNIXʱ���Ϊ����ʱ��
 @Input				UNIXʱ���
 @Return				struct tm
**********************************************************************************************************/
struct tm RTC_ConvUnixToCalendar(time_t t)
{
	struct tm *t_time;
	
	t_time = localtime(&t);
	t_time->tm_year -= 100;												//1900���׼ת����2000���׼
	t_time->tm_mon += 1;												//0~11��׼ת����1~12��׼
	
	return *t_time;
}

/**********************************************************************************************************
 @Function			time_t RTC_GetUnixTimeToStamp(void)
 @Description			��ȡ��ǰUNIXʱ���
 @Input				void
 @Return				time_t			: ʱ���
**********************************************************************************************************/
time_t RTC_GetUnixTimeToStamp(void)
{
	RTC_TimeTypeDef RTC_Timestructure;
	RTC_DateTypeDef RTC_Datestructure;
	
	HAL_RTC_GetTime(&RTC_Handler, &RTC_Timestructure, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&RTC_Handler, &RTC_Datestructure, RTC_FORMAT_BIN);
	
	return RTC_TimeToStamp(RTC_Datestructure.Year, RTC_Datestructure.Month, RTC_Datestructure.Date, RTC_Timestructure.Hours, RTC_Timestructure.Minutes, RTC_Timestructure.Seconds) - 8 * 3600;
}

/********************************************** END OF FLEE **********************************************/
