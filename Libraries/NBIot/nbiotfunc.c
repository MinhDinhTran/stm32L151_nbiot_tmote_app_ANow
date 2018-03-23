/**
  *********************************************************************************************************
  * @file    nbiotfunc.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2018-03-03
  * @brief   
  *********************************************************************************************************
  * @attention
  *
  *
  *
  *********************************************************************************************************
  */

#include "nbiotfunc.h"
#include "stm32l1xx_config.h"
#include "delay.h"
#include "usart.h"
#include "hal_rtc.h"

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Neul_NBxx_HardwarePoweroff(NBIOT_ClientsTypeDef* pClient)
 @Description			NBIOT_Neul_NBxx_HardwareReboot			: Ӳ���ϵ�
 @Input				pClient								: NBIOT�ͻ���ʵ��
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_HardwarePoweroff(NBIOT_ClientsTypeDef* pClient)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	GPIO_InitTypeDef GPIO_Initure;
	
	GPIO_Initure.Pin = NBIOT_RESET_PIN;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Initure.Pull = GPIO_NOPULL;
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(NBIOT_RESET_GPIOx, &GPIO_Initure);
	
	NBIOTPOWER(OFF);
	HAL_GPIO_WritePin(NBIOT_RESET_GPIOx, NBIOT_RESET_PIN, GPIO_PIN_RESET);
	
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Neul_NBxx_HardwareReboot(NBIOT_ClientsTypeDef* pClient, u32 rebootTimeoutMS)
 @Description			NBIOT_Neul_NBxx_HardwareReboot			: Ӳ������NBģ��
 @Input				pClient								: NBIOT�ͻ���ʵ��
					rebootTimeoutMS						: �����ȴ���ʱʱ��
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_HardwareReboot(NBIOT_ClientsTypeDef* pClient, u32 rebootTimeoutMS)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	Stm32_CalculagraphTypeDef ATCmd_timer_Ms;
	GPIO_InitTypeDef GPIO_Initure;
	
	GPIO_Initure.Pin = NBIOT_RESET_PIN;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Initure.Pull = GPIO_NOPULL;
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(NBIOT_RESET_GPIOx, &GPIO_Initure);
	
	NBIOTPOWER(OFF);
	Delay_MS(2000);
	NBIOTPOWER(ON);
	Delay_MS(300);
	
	HAL_GPIO_WritePin(NBIOT_RESET_GPIOx, NBIOT_RESET_PIN, GPIO_PIN_SET);
	Delay_MS(200);
	HAL_GPIO_WritePin(NBIOT_RESET_GPIOx, NBIOT_RESET_PIN, GPIO_PIN_RESET);
	
	Stm32_Calculagraph_CountdownMS(&ATCmd_timer_Ms, rebootTimeoutMS);
	pClient->ATCmdStack->CmdWaitTime = ATCmd_timer_Ms;
	
	pClient->ATCmdStack->ATack = "OK";
	pClient->ATCmdStack->ATNack = "ERROR";
	NBStatus = pClient->ATCmdStack->Read(pClient->ATCmdStack);
	
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Neul_NBxx_SoftwareReboot(NBIOT_ClientsTypeDef* pClient, u32 rebootTimeoutMS)
 @Description			NBIOT_Neul_NBxx_SoftwareReboot			: �������NBģ��
 @Input				pClient								: NBIOT�ͻ���ʵ��
					rebootTimeoutMS						: �����ȴ���ʱʱ��
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_SoftwareReboot(NBIOT_ClientsTypeDef* pClient, u32 rebootTimeoutMS)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	Stm32_CalculagraphTypeDef ATCmd_timer_Ms;
	
	Stm32_Calculagraph_CountdownMS(&ATCmd_timer_Ms, rebootTimeoutMS);
	pClient->ATCmdStack->CmdWaitTime = ATCmd_timer_Ms;
	
	sprintf((char *)pClient->ATCmdStack->ATSendbuf, "AT+NRB\r");
	pClient->ATCmdStack->ATSendlen = strlen("AT+NRB\r");
	pClient->ATCmdStack->ATack = "OK";
	pClient->ATCmdStack->ATNack = "ERROR";
	NBStatus = pClient->ATCmdStack->Write(pClient->ATCmdStack);
	
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadManufacturer(NBIOT_ClientsTypeDef* pClient)
 @Description			NBIOT_Neul_NBxx_CheckReadManufacturer		: ��������̱�ʶ
 @Input				pClient								: NBIOT�ͻ���ʵ��
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadManufacturer(NBIOT_ClientsTypeDef* pClient)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	Stm32_CalculagraphTypeDef ATCmd_timer_Ms;
	
	Stm32_Calculagraph_CountdownMS(&ATCmd_timer_Ms, pClient->Command_Timeout_Msec);
	pClient->ATCmdStack->CmdWaitTime = ATCmd_timer_Ms;
	
	sprintf((char *)pClient->ATCmdStack->ATSendbuf, "AT+CGMI\r");
	pClient->ATCmdStack->ATSendlen = strlen("AT+CGMI\r");
	pClient->ATCmdStack->ATack = "OK";
	pClient->ATCmdStack->ATNack = "ERROR";
	if ((NBStatus = pClient->ATCmdStack->Write(pClient->ATCmdStack)) == NBIOT_OK) {
		memset((void *)pClient->Parameter.manufacturer, 0x0, sizeof(pClient->Parameter.manufacturer));
		if (sscanf((const char*)pClient->ATCmdStack->ATRecvbuf, "%s\r\n", pClient->Parameter.manufacturer) <= 0) {
			NBStatus = NBIOT_ERROR;
		}
	}
	
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadManufacturerModel(NBIOT_ClientsTypeDef* pClient)
 @Description			NBIOT_Neul_NBxx_CheckReadManufacturerModel	: ����������ͺ�
 @Input				pClient								: NBIOT�ͻ���ʵ��
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadManufacturerModel(NBIOT_ClientsTypeDef* pClient)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	Stm32_CalculagraphTypeDef ATCmd_timer_Ms;
	
	Stm32_Calculagraph_CountdownMS(&ATCmd_timer_Ms, pClient->Command_Timeout_Msec);
	pClient->ATCmdStack->CmdWaitTime = ATCmd_timer_Ms;
	
	sprintf((char *)pClient->ATCmdStack->ATSendbuf, "AT+CGMM\r");
	pClient->ATCmdStack->ATSendlen = strlen("AT+CGMM\r");
	pClient->ATCmdStack->ATack = "OK";
	pClient->ATCmdStack->ATNack = "ERROR";
	if ((NBStatus = pClient->ATCmdStack->Write(pClient->ATCmdStack)) == NBIOT_OK) {
		memset((void *)pClient->Parameter.manufacturermode, 0x0, sizeof(pClient->Parameter.manufacturermode));
		if (sscanf((const char*)pClient->ATCmdStack->ATRecvbuf, "%s\r\n", pClient->Parameter.manufacturermode) <= 0) {
			NBStatus = NBIOT_ERROR;
		}
	}
	
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadIMEI(NBIOT_ClientsTypeDef* pClient)
 @Description			NBIOT_Neul_NBxx_CheckReadIMEI				: ���IMEI��
 @Input				pClient								: NBIOT�ͻ���ʵ��
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadIMEI(NBIOT_ClientsTypeDef* pClient)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	Stm32_CalculagraphTypeDef ATCmd_timer_Ms;
	
	Stm32_Calculagraph_CountdownMS(&ATCmd_timer_Ms, pClient->Command_Timeout_Msec);
	pClient->ATCmdStack->CmdWaitTime = ATCmd_timer_Ms;
	
	sprintf((char *)pClient->ATCmdStack->ATSendbuf, "AT+CGSN=1\r");
	pClient->ATCmdStack->ATSendlen = strlen("AT+CGSN=1\r");
	pClient->ATCmdStack->ATack = "OK";
	pClient->ATCmdStack->ATNack = "ERROR";
	if ((NBStatus = pClient->ATCmdStack->Write(pClient->ATCmdStack)) == NBIOT_OK) {
		memset((void *)pClient->Parameter.imei, 0x0, sizeof(pClient->Parameter.imei));
		if (sscanf((const char*)pClient->ATCmdStack->ATRecvbuf, "%*[^+]+CGSN:%[^\r]", pClient->Parameter.imei) <= 0) {
			NBStatus = NBIOT_ERROR;
		}
	}
	
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadIMEISV(NBIOT_ClientsTypeDef* pClient)
 @Description			NBIOT_Neul_NBxx_CheckReadIMEISV			: ���IMEISV��
 @Input				pClient								: NBIOT�ͻ���ʵ��
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadIMEISV(NBIOT_ClientsTypeDef* pClient)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	Stm32_CalculagraphTypeDef ATCmd_timer_Ms;
	
	Stm32_Calculagraph_CountdownMS(&ATCmd_timer_Ms, pClient->Command_Timeout_Msec);
	pClient->ATCmdStack->CmdWaitTime = ATCmd_timer_Ms;
	
	sprintf((char *)pClient->ATCmdStack->ATSendbuf, "AT+CGSN=2\r");
	pClient->ATCmdStack->ATSendlen = strlen("AT+CGSN=2\r");
	pClient->ATCmdStack->ATack = "OK";
	pClient->ATCmdStack->ATNack = "ERROR";
	if ((NBStatus = pClient->ATCmdStack->Write(pClient->ATCmdStack)) == NBIOT_OK) {
		memset((void *)pClient->Parameter.imeisv, 0x0, sizeof(pClient->Parameter.imeisv));
		if (sscanf((const char*)pClient->ATCmdStack->ATRecvbuf, "%*[^+]+CGSN:%[^\r]", pClient->Parameter.imeisv) <= 0) {
			NBStatus = NBIOT_ERROR;
		}
	}
	
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadRSSI(NBIOT_ClientsTypeDef* pClient)
 @Description			NBIOT_Neul_NBxx_CheckReadRSSI				: ���RSSI��
 @Input				pClient								: NBIOT�ͻ���ʵ��
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadRSSI(NBIOT_ClientsTypeDef* pClient)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	Stm32_CalculagraphTypeDef ATCmd_timer_Ms;
	
	Stm32_Calculagraph_CountdownMS(&ATCmd_timer_Ms, pClient->Command_Timeout_Msec);
	pClient->ATCmdStack->CmdWaitTime = ATCmd_timer_Ms;
	
	sprintf((char *)pClient->ATCmdStack->ATSendbuf, "AT+CSQ\r");
	pClient->ATCmdStack->ATSendlen = strlen("AT+CSQ\r");
	pClient->ATCmdStack->ATack = "OK";
	pClient->ATCmdStack->ATNack = "ERROR";
	if ((NBStatus = pClient->ATCmdStack->Write(pClient->ATCmdStack)) == NBIOT_OK) {
		if (sscanf((const char*)pClient->ATCmdStack->ATRecvbuf, "%*[^+]+CSQ:%d,%*d\r\n", &pClient->Parameter.rssi) <= 0) {
			NBStatus = NBIOT_ERROR;
		}
	}
	
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadStatisticsCELL(NBIOT_ClientsTypeDef* pClient)
 @Description			NBIOT_Neul_NBxx_CheckReadStatisticsCELL		: �����վ���Ӳ���
 @Input				pClient								: NBIOT�ͻ���ʵ��
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadStatisticsCELL(NBIOT_ClientsTypeDef* pClient)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	Stm32_CalculagraphTypeDef ATCmd_timer_Ms;
	
	Stm32_Calculagraph_CountdownMS(&ATCmd_timer_Ms, pClient->Command_Timeout_Msec);
	pClient->ATCmdStack->CmdWaitTime = ATCmd_timer_Ms;
	
	sprintf((char *)pClient->ATCmdStack->ATSendbuf, "AT+NUESTATS=CELL\r");
	pClient->ATCmdStack->ATSendlen = strlen("AT+NUESTATS=CELL\r");
	pClient->ATCmdStack->ATack = "OK";
	pClient->ATCmdStack->ATNack = "ERROR";
	if ((NBStatus = pClient->ATCmdStack->Write(pClient->ATCmdStack)) == NBIOT_OK) {
		if ( sscanf((const char*)pClient->ATCmdStack->ATRecvbuf, 
			"\r\nNUESTATS:CELL,%d,%d,%d,%d,%d,%d,%d\r", 
			&pClient->Parameter.statisticsCELL.earfcn, 
			&pClient->Parameter.statisticsCELL.cellID, 
			&pClient->Parameter.statisticsCELL.cell, 
			&pClient->Parameter.statisticsCELL.rsrp, 
			&pClient->Parameter.statisticsCELL.rsrq, 
			&pClient->Parameter.statisticsCELL.cellrssi, 
			&pClient->Parameter.statisticsCELL.snr) <= 0) {
			NBStatus = NBIOT_ERROR;
		}
	}
	
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadICCID(NBIOT_ClientsTypeDef* pClient)
 @Description			NBIOT_Neul_NBxx_CheckReadICCID			: ���ICCID����
 @Input				pClient								: NBIOT�ͻ���ʵ��
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadICCID(NBIOT_ClientsTypeDef* pClient)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	Stm32_CalculagraphTypeDef ATCmd_timer_Ms;
	
	Stm32_Calculagraph_CountdownMS(&ATCmd_timer_Ms, pClient->Command_Timeout_Msec);
	pClient->ATCmdStack->CmdWaitTime = ATCmd_timer_Ms;
	
	sprintf((char *)pClient->ATCmdStack->ATSendbuf, "AT+NCCID\r");
	pClient->ATCmdStack->ATSendlen = strlen("AT+NCCID\r");
	pClient->ATCmdStack->ATack = "OK";
	pClient->ATCmdStack->ATNack = "ERROR";
	if ((NBStatus = pClient->ATCmdStack->Write(pClient->ATCmdStack)) == NBIOT_OK) {
		memset((void *)pClient->Parameter.iccid, 0x0, sizeof(pClient->Parameter.iccid));
		if (sscanf((const char*)pClient->ATCmdStack->ATRecvbuf, "%*[^+]+NCCID:%[^\r]", pClient->Parameter.iccid) <= 0) {
			NBStatus = NBIOT_ERROR;
		}
	}
	
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadIMSI(NBIOT_ClientsTypeDef* pClient)
 @Description			NBIOT_Neul_NBxx_CheckReadIMSI				: ���IMSI��
 @Input				pClient								: NBIOT�ͻ���ʵ��
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadIMSI(NBIOT_ClientsTypeDef* pClient)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	Stm32_CalculagraphTypeDef ATCmd_timer_Ms;
	
	Stm32_Calculagraph_CountdownMS(&ATCmd_timer_Ms, pClient->Command_Timeout_Msec);
	pClient->ATCmdStack->CmdWaitTime = ATCmd_timer_Ms;
	
	sprintf((char *)pClient->ATCmdStack->ATSendbuf, "AT+CIMI\r");
	pClient->ATCmdStack->ATSendlen = strlen("AT+CIMI\r");
	pClient->ATCmdStack->ATack = "OK";
	pClient->ATCmdStack->ATNack = "ERROR";
	if ((NBStatus = pClient->ATCmdStack->Write(pClient->ATCmdStack)) == NBIOT_OK) {
		memset((void *)pClient->Parameter.imsi, 0x0, sizeof(pClient->Parameter.imsi));
		if (sscanf((const char*)pClient->ATCmdStack->ATRecvbuf, "\r\n%[^\r]", pClient->Parameter.imsi) <= 0) {
			NBStatus = NBIOT_ERROR;
		}
	}
	
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadCGPADDR(NBIOT_ClientsTypeDef* pClient)
 @Description			NBIOT_Neul_NBxx_CheckReadCGPADDR			: ���CGPADDR��ַ
 @Input				pClient								: NBIOT�ͻ���ʵ��
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadCGPADDR(NBIOT_ClientsTypeDef* pClient)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	Stm32_CalculagraphTypeDef ATCmd_timer_Ms;
	
	Stm32_Calculagraph_CountdownMS(&ATCmd_timer_Ms, pClient->Command_Timeout_Msec);
	pClient->ATCmdStack->CmdWaitTime = ATCmd_timer_Ms;
	
	sprintf((char *)pClient->ATCmdStack->ATSendbuf, "AT+CGPADDR\r");
	pClient->ATCmdStack->ATSendlen = strlen("AT+CGPADDR\r");
	pClient->ATCmdStack->ATack = "OK";
	pClient->ATCmdStack->ATNack = "ERROR";
	if ((NBStatus = pClient->ATCmdStack->Write(pClient->ATCmdStack)) == NBIOT_OK) {
		if (strstr((const char*)pClient->ATCmdStack->ATRecvbuf, "+CGPADDR:0,")) {
			memset((void *)pClient->Parameter.cgpaddr, 0x0, sizeof(pClient->Parameter.cgpaddr));
			if (sscanf((const char*)pClient->ATCmdStack->ATRecvbuf, "%*[^+]+CGPADDR:0,%[^\r]", pClient->Parameter.cgpaddr) <= 0) {
				NBStatus = NBIOT_ERROR;
			}
			else {
				NBStatus = NBIOT_OK;
			}
		}
		else {
			NBStatus = NBIOT_ERROR;
		}
	}
	
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadCGDCONT(NBIOT_ClientsTypeDef* pClient)
 @Description			NBIOT_Neul_NBxx_CheckReadCGDCONT			: ���CGDCONT��ַ
 @Input				pClient								: NBIOT�ͻ���ʵ��
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadCGDCONT(NBIOT_ClientsTypeDef* pClient)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	Stm32_CalculagraphTypeDef ATCmd_timer_Ms;
	
	Stm32_Calculagraph_CountdownMS(&ATCmd_timer_Ms, pClient->Command_Timeout_Msec);
	pClient->ATCmdStack->CmdWaitTime = ATCmd_timer_Ms;
	
	sprintf((char *)pClient->ATCmdStack->ATSendbuf, "AT+CGDCONT?\r");
	pClient->ATCmdStack->ATSendlen = strlen("AT+CGDCONT?\r");
	pClient->ATCmdStack->ATack = "OK";
	pClient->ATCmdStack->ATNack = "ERROR";
	if ((NBStatus = pClient->ATCmdStack->Write(pClient->ATCmdStack)) == NBIOT_OK) {
		if (strstr((const char*)pClient->ATCmdStack->ATRecvbuf, "+CGDCONT:0,\"IP\",\"")) {
			memset((void *)pClient->Parameter.cgdcont, 0x0, sizeof(pClient->Parameter.cgdcont));
			if (sscanf((const char*)pClient->ATCmdStack->ATRecvbuf, "\r\n+CGDCONT:0,\"IP\",\"%[^\"]", pClient->Parameter.cgdcont) <= 0) {
				NBStatus = NBIOT_ERROR;
			}
			else {
				NBStatus = NBIOT_OK;
			}
		}
		else {
			NBStatus = NBIOT_ERROR;
		}
	}
	
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadDateTime(NBIOT_ClientsTypeDef* pClient)
 @Description			NBIOT_Neul_NBxx_CheckReadDateTime			: ���DateTime��վʱ��
 @Input				pClient								: NBIOT�ͻ���ʵ��
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadDateTime(NBIOT_ClientsTypeDef* pClient)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	Stm32_CalculagraphTypeDef ATCmd_timer_Ms;
	struct tm Datetime;
	int zoneTime;
	
	Stm32_Calculagraph_CountdownMS(&ATCmd_timer_Ms, pClient->Command_Timeout_Msec);
	pClient->ATCmdStack->CmdWaitTime = ATCmd_timer_Ms;
	
	sprintf((char *)pClient->ATCmdStack->ATSendbuf, "AT+CCLK?\r");
	pClient->ATCmdStack->ATSendlen = strlen("AT+CCLK?\r");
	pClient->ATCmdStack->ATack = "OK";
	pClient->ATCmdStack->ATNack = "ERROR";
	if ((NBStatus = pClient->ATCmdStack->Write(pClient->ATCmdStack)) == NBIOT_OK) {
		if ( sscanf((const char*)pClient->ATCmdStack->ATRecvbuf, 
			"\r\n+CCLK:%d/%d/%d,%d:%d:%d+%d\r", 
			&pClient->Parameter.dataTime.year, 
			&pClient->Parameter.dataTime.month, 
			&pClient->Parameter.dataTime.day, 
			&pClient->Parameter.dataTime.hour, 
			&pClient->Parameter.dataTime.min, 
			&pClient->Parameter.dataTime.sec, 
			&zoneTime) <= 0) {
			pClient->Parameter.dataTime.year		= 18;
			pClient->Parameter.dataTime.month		= 01;
			pClient->Parameter.dataTime.day		= 01;
			pClient->Parameter.dataTime.hour		= 00;
			pClient->Parameter.dataTime.min		= 00;
			pClient->Parameter.dataTime.sec		= 00;
			NBStatus = NBIOT_ERROR;
		}
		else {
			pClient->Parameter.dataTime.localstamp = RTC_TimeToStamp(pClient->Parameter.dataTime.year, 
														  pClient->Parameter.dataTime.month, 
														  pClient->Parameter.dataTime.day, 
														  pClient->Parameter.dataTime.hour, 
														  pClient->Parameter.dataTime.min, 
														  pClient->Parameter.dataTime.sec) + (zoneTime * 15 * 60);
			Datetime = RTC_ConvUnixToCalendar(pClient->Parameter.dataTime.localstamp);
			pClient->Parameter.dataTime.year		= Datetime.tm_year;
			pClient->Parameter.dataTime.month		= Datetime.tm_mon;
			pClient->Parameter.dataTime.day		= Datetime.tm_mday;
			pClient->Parameter.dataTime.hour		= Datetime.tm_hour;
			pClient->Parameter.dataTime.min		= Datetime.tm_min;
			pClient->Parameter.dataTime.sec		= Datetime.tm_sec;
			NBStatus = NBIOT_OK;
		}
	}
	
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Neul_NBxx_SetAttachOrDetach(NBIOT_ClientsTypeDef* pClient, NBIOT_NetstateTypeDef attdet)
 @Description			NBIOT_Neul_NBxx_SetAttachOrDetach			: �����ն���������
 @Input				pClient								: NBIOT�ͻ���ʵ��
					attdet								: Detach / Attach
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_SetAttachOrDetach(NBIOT_ClientsTypeDef* pClient, NBIOT_NetstateTypeDef attdet)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	Stm32_CalculagraphTypeDef ATCmd_timer_Ms;
	
	Stm32_Calculagraph_CountdownMS(&ATCmd_timer_Ms, pClient->Command_Timeout_Msec);
	pClient->ATCmdStack->CmdWaitTime = ATCmd_timer_Ms;
	
	if (attdet == Detach) {												//����
		sprintf((char *)pClient->ATCmdStack->ATSendbuf, "AT+CGATT=0\r");
		pClient->ATCmdStack->ATSendlen = strlen("AT+CGATT=0\r");
	}
	else if (attdet == Attach) {											//����
		sprintf((char *)pClient->ATCmdStack->ATSendbuf, "AT+CGATT=1\r");
		pClient->ATCmdStack->ATSendlen = strlen("AT+CGATT=1\r");
	}
	else {
		NBStatus = NBIOT_ERROR;
	}
	
	pClient->ATCmdStack->ATack = "OK";
	pClient->ATCmdStack->ATNack = "ERROR";
	NBStatus = pClient->ATCmdStack->Write(pClient->ATCmdStack);
	
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadAttachOrDetach(NBIOT_ClientsTypeDef* pClient)
 @Description			NBIOT_Neul_NBxx_CheckReadAttachOrDetach		: ��ѯ�ն���������
 @Input				pClient								: NBIOT�ͻ���ʵ��
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadAttachOrDetach(NBIOT_ClientsTypeDef* pClient)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	Stm32_CalculagraphTypeDef ATCmd_timer_Ms;
	
	Stm32_Calculagraph_CountdownMS(&ATCmd_timer_Ms, pClient->Command_Timeout_Msec);
	pClient->ATCmdStack->CmdWaitTime = ATCmd_timer_Ms;
	
	sprintf((char *)pClient->ATCmdStack->ATSendbuf, "AT+CGATT?\r");
	pClient->ATCmdStack->ATSendlen = strlen("AT+CGATT?\r");
	pClient->ATCmdStack->ATack = "OK";
	pClient->ATCmdStack->ATNack = "ERROR";
	if ((NBStatus = pClient->ATCmdStack->Write(pClient->ATCmdStack)) == NBIOT_OK) {
		if (strstr((const char*)pClient->ATCmdStack->ATRecvbuf, "+CGATT:1")) {
			pClient->Parameter.netstate = Attach;
		}
		else if (strstr((const char*)pClient->ATCmdStack->ATRecvbuf, "+CGATT:0")) {
			pClient->Parameter.netstate = Detach;
		}
		else {
			NBStatus = NBIOT_ERROR;
		}
	}
	
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Neul_NBxx_SetMinOrFullFunc(NBIOT_ClientsTypeDef* pClient, NBIOT_FunctionalityTypeDef minfull)
 @Description			NBIOT_Neul_NBxx_SetMinOrFullFunc			: �����豸����
 @Input				pClient								: NBIOT�ͻ���ʵ��
					minfull								: MinFunc / FullFunc
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_SetMinOrFullFunc(NBIOT_ClientsTypeDef* pClient, NBIOT_FunctionalityTypeDef minfull)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	Stm32_CalculagraphTypeDef ATCmd_timer_Ms;
	
	Stm32_Calculagraph_CountdownMS(&ATCmd_timer_Ms, pClient->Command_Timeout_Msec);
	pClient->ATCmdStack->CmdWaitTime = ATCmd_timer_Ms;
	
	if (minfull == MinFunc) {											//��С����
		sprintf((char *)pClient->ATCmdStack->ATSendbuf, "AT+CFUN=0\r");
		pClient->ATCmdStack->ATSendlen = strlen("AT+CFUN=0\r");
	}
	else if (minfull == FullFunc) {										//ȫ����
		sprintf((char *)pClient->ATCmdStack->ATSendbuf, "AT+CFUN=1\r");
		pClient->ATCmdStack->ATSendlen = strlen("AT+CFUN=1\r");
	}
	else {
		NBStatus = NBIOT_ERROR;
	}
	
	pClient->ATCmdStack->ATack = "OK";
	pClient->ATCmdStack->ATNack = "ERROR";
	NBStatus = pClient->ATCmdStack->Write(pClient->ATCmdStack);
	
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadMinOrFullFunc(NBIOT_ClientsTypeDef* pClient)
 @Description			NBIOT_Neul_NBxx_CheckReadMinOrFullFunc		: ��ѯ�豸����
 @Input				pClient								: NBIOT�ͻ���ʵ��
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadMinOrFullFunc(NBIOT_ClientsTypeDef* pClient)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	Stm32_CalculagraphTypeDef ATCmd_timer_Ms;
	
	Stm32_Calculagraph_CountdownMS(&ATCmd_timer_Ms, pClient->Command_Timeout_Msec);
	pClient->ATCmdStack->CmdWaitTime = ATCmd_timer_Ms;
	
	sprintf((char *)pClient->ATCmdStack->ATSendbuf, "AT+CFUN?\r");
	pClient->ATCmdStack->ATSendlen = strlen("AT+CFUN?\r");
	pClient->ATCmdStack->ATack = "OK";
	pClient->ATCmdStack->ATNack = "ERROR";
	if ((NBStatus = pClient->ATCmdStack->Write(pClient->ATCmdStack)) == NBIOT_OK) {
		if (strstr((const char*)pClient->ATCmdStack->ATRecvbuf, "+CFUN:1")) {
			pClient->Parameter.functionality = FullFunc;
		}
		else if (strstr((const char*)pClient->ATCmdStack->ATRecvbuf, "+CFUN:0")) {
			pClient->Parameter.functionality = MinFunc;
		}
		else {
			NBStatus = NBIOT_ERROR;
		}
	}
	
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Neul_NBxx_SetNewMessageIndications(NBIOT_ClientsTypeDef* pClient, NBIOT_OpenOrCloseFuncTypeDef state)
 @Description			NBIOT_Neul_NBxx_SetNewMessageIndications	: ����NNMI�����Ƿ���
 @Input				pClient								: NBIOT�ͻ���ʵ��
					state								: CloseFunc / OpenFunc
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_SetNewMessageIndications(NBIOT_ClientsTypeDef* pClient, NBIOT_OpenOrCloseFuncTypeDef state)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	Stm32_CalculagraphTypeDef ATCmd_timer_Ms;
	
	Stm32_Calculagraph_CountdownMS(&ATCmd_timer_Ms, pClient->Command_Timeout_Msec);
	pClient->ATCmdStack->CmdWaitTime = ATCmd_timer_Ms;
	
	if (state == CloseFunc) {											//�ر�
		sprintf((char *)pClient->ATCmdStack->ATSendbuf, "AT+NNMI=0\r");
		pClient->ATCmdStack->ATSendlen = strlen("AT+NNMI=0\r");
	}
	else if (state == OpenFunc) {											//����
		sprintf((char *)pClient->ATCmdStack->ATSendbuf, "AT+NNMI=1\r");
		pClient->ATCmdStack->ATSendlen = strlen("AT+NNMI=1\r");
	}
	else {
		NBStatus = NBIOT_ERROR;
	}
	
	pClient->ATCmdStack->ATack = "OK";
	pClient->ATCmdStack->ATNack = "ERROR";
	NBStatus = pClient->ATCmdStack->Write(pClient->ATCmdStack);
	
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadNewMessageIndications(NBIOT_ClientsTypeDef* pClient)
 @Description			NBIOT_Neul_NBxx_CheckReadNewMessageIndications	: ��ѯNNMI�����Ƿ���
 @Input				pClient									: NBIOT�ͻ���ʵ��
 @Return				NBIOT_StatusTypeDef							: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadNewMessageIndications(NBIOT_ClientsTypeDef* pClient)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	Stm32_CalculagraphTypeDef ATCmd_timer_Ms;
	
	Stm32_Calculagraph_CountdownMS(&ATCmd_timer_Ms, pClient->Command_Timeout_Msec);
	pClient->ATCmdStack->CmdWaitTime = ATCmd_timer_Ms;
	
	sprintf((char *)pClient->ATCmdStack->ATSendbuf, "AT+NNMI?\r");
	pClient->ATCmdStack->ATSendlen = strlen("AT+NNMI?\r");
	pClient->ATCmdStack->ATack = "OK";
	pClient->ATCmdStack->ATNack = "ERROR";
	if ((NBStatus = pClient->ATCmdStack->Write(pClient->ATCmdStack)) == NBIOT_OK) {
		if (strstr((const char*)pClient->ATCmdStack->ATRecvbuf, "+NNMI:1")) {
			pClient->Parameter.nnmistate = OpenFunc;
		}
		else if (strstr((const char*)pClient->ATCmdStack->ATRecvbuf, "+NNMI:0")) {
			pClient->Parameter.nnmistate = CloseFunc;
		}
		else {
			NBStatus = NBIOT_ERROR;
		}
	}
	
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Neul_NBxx_SetSentMessageIndications(NBIOT_ClientsTypeDef* pClient, NBIOT_OpenOrCloseFuncTypeDef state)
 @Description			NBIOT_Neul_NBxx_SetSentMessageIndications	: ����NSMI�����Ƿ���
 @Input				pClient								: NBIOT�ͻ���ʵ��
					state								: CloseFunc / OpenFunc
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_SetSentMessageIndications(NBIOT_ClientsTypeDef* pClient, NBIOT_OpenOrCloseFuncTypeDef state)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	Stm32_CalculagraphTypeDef ATCmd_timer_Ms;
	
	Stm32_Calculagraph_CountdownMS(&ATCmd_timer_Ms, pClient->Command_Timeout_Msec);
	pClient->ATCmdStack->CmdWaitTime = ATCmd_timer_Ms;
	
	if (state == CloseFunc) {											//�ر�
		sprintf((char *)pClient->ATCmdStack->ATSendbuf, "AT+NSMI=0\r");
		pClient->ATCmdStack->ATSendlen = strlen("AT+NSMI=0\r");
	}
	else if (state == OpenFunc) {											//����
		sprintf((char *)pClient->ATCmdStack->ATSendbuf, "AT+NSMI=1\r");
		pClient->ATCmdStack->ATSendlen = strlen("AT+NSMI=1\r");
	}
	else {
		NBStatus = NBIOT_ERROR;
	}
	
	pClient->ATCmdStack->ATack = "OK";
	pClient->ATCmdStack->ATNack = "ERROR";
	NBStatus = pClient->ATCmdStack->Write(pClient->ATCmdStack);
	
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadSentMessageIndications(NBIOT_ClientsTypeDef* pClient)
 @Description			NBIOT_Neul_NBxx_CheckReadSentMessageIndications	: ��ѯNSMI�����Ƿ���
 @Input				pClient									: NBIOT�ͻ���ʵ��
 @Return				NBIOT_StatusTypeDef							: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadSentMessageIndications(NBIOT_ClientsTypeDef* pClient)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	Stm32_CalculagraphTypeDef ATCmd_timer_Ms;
	
	Stm32_Calculagraph_CountdownMS(&ATCmd_timer_Ms, pClient->Command_Timeout_Msec);
	pClient->ATCmdStack->CmdWaitTime = ATCmd_timer_Ms;
	
	sprintf((char *)pClient->ATCmdStack->ATSendbuf, "AT+NSMI?\r");
	pClient->ATCmdStack->ATSendlen = strlen("AT+NSMI?\r");
	pClient->ATCmdStack->ATack = "OK";
	pClient->ATCmdStack->ATNack = "ERROR";
	if ((NBStatus = pClient->ATCmdStack->Write(pClient->ATCmdStack)) == NBIOT_OK) {
		if (strstr((const char*)pClient->ATCmdStack->ATRecvbuf, "+NSMI:1")) {
			pClient->Parameter.nsmistate = OpenFunc;
		}
		else if (strstr((const char*)pClient->ATCmdStack->ATRecvbuf, "+NSMI:0")) {
			pClient->Parameter.nsmistate = CloseFunc;
		}
		else {
			NBStatus = NBIOT_ERROR;
		}
	}
	
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Neul_NBxx_SetSupportedBands(NBIOT_ClientsTypeDef* pClient, NBIOT_BandTypeDef bands)
 @Description			NBIOT_Neul_NBxx_SetSupportedBands			: ����Band
 @Input				pClient								: NBIOT�ͻ���ʵ��
					bands								: ChinaTelecom						= 5
														  ChinaMobile						= 8
														  ChinaUnicom						= 20
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_SetSupportedBands(NBIOT_ClientsTypeDef* pClient, NBIOT_BandTypeDef bands)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	Stm32_CalculagraphTypeDef ATCmd_timer_Ms;
	
	Stm32_Calculagraph_CountdownMS(&ATCmd_timer_Ms, pClient->Command_Timeout_Msec);
	pClient->ATCmdStack->CmdWaitTime = ATCmd_timer_Ms;
	
	memset((void *)pClient->DataProcessStack, 0x0, pClient->DataProcessStack_size);
	sprintf((char *)pClient->DataProcessStack, "AT+NBAND=%d\r", bands);
	
	memcpy(pClient->ATCmdStack->ATSendbuf, pClient->DataProcessStack, strlen((char *)pClient->DataProcessStack));
	pClient->ATCmdStack->ATSendlen = strlen((char *)pClient->DataProcessStack);
	pClient->ATCmdStack->ATack = "OK";
	pClient->ATCmdStack->ATNack = "ERROR";
	NBStatus = pClient->ATCmdStack->Write(pClient->ATCmdStack);
	
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadSupportedBands(NBIOT_ClientsTypeDef* pClient)
 @Description			NBIOT_Neul_NBxx_CheckReadSupportedBands		: ��ѯBand
 @Input				pClient								: NBIOT�ͻ���ʵ��
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadSupportedBands(NBIOT_ClientsTypeDef* pClient)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	Stm32_CalculagraphTypeDef ATCmd_timer_Ms;
	int bands = 0;
	
	Stm32_Calculagraph_CountdownMS(&ATCmd_timer_Ms, pClient->Command_Timeout_Msec);
	pClient->ATCmdStack->CmdWaitTime = ATCmd_timer_Ms;
	
	sprintf((char *)pClient->ATCmdStack->ATSendbuf, "AT+NBAND?\r");
	pClient->ATCmdStack->ATSendlen = strlen("AT+NBAND?\r");
	pClient->ATCmdStack->ATack = "OK";
	pClient->ATCmdStack->ATNack = "ERROR";
	if ((NBStatus = pClient->ATCmdStack->Write(pClient->ATCmdStack)) == NBIOT_OK) {
		if (sscanf((const char*)pClient->ATCmdStack->ATRecvbuf, "\r\n+NBAND:%d\r", &bands) <= 0) {
			NBStatus = NBIOT_ERROR;
		}
		else {
			if (bands == ChinaTelecom) {
				pClient->Parameter.band = ChinaTelecom;
			}
			else if (bands == ChinaMobile) {
				pClient->Parameter.band = ChinaMobile;
			}
			else if (bands == ChinaUnicom) {
				pClient->Parameter.band = ChinaUnicom;
			}
			else {
				NBStatus = NBIOT_ERROR;
			}
		}
	}
	
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Neul_NBxx_SetCDPServer(NBIOT_ClientsTypeDef* pClient, const char *host, unsigned short port)
 @Description			NBIOT_Neul_NBxx_SetCDPServer				: ����CDP������
 @Input				pClient								: NBIOT�ͻ���ʵ��
					host									: CDP Server Address
					port									: CDP Server Port
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_SetCDPServer(NBIOT_ClientsTypeDef* pClient, const char *host, unsigned short port)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	Stm32_CalculagraphTypeDef ATCmd_timer_Ms;
	
	Stm32_Calculagraph_CountdownMS(&ATCmd_timer_Ms, pClient->Command_Timeout_Msec);
	pClient->ATCmdStack->CmdWaitTime = ATCmd_timer_Ms;
	
	memset((void *)pClient->DataProcessStack, 0x0, pClient->DataProcessStack_size);
	sprintf((char *)pClient->DataProcessStack, "AT+NCDP=%s,%d\r", host, port);
	
	memcpy(pClient->ATCmdStack->ATSendbuf, pClient->DataProcessStack, strlen((char *)pClient->DataProcessStack));
	pClient->ATCmdStack->ATSendlen = strlen((char *)pClient->DataProcessStack);
	pClient->ATCmdStack->ATack = "OK";
	pClient->ATCmdStack->ATNack = "ERROR";
	NBStatus = pClient->ATCmdStack->Write(pClient->ATCmdStack);
	
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadCDPServer(NBIOT_ClientsTypeDef* pClient)
 @Description			NBIOT_Neul_NBxx_CheckReadCDPServer			: ��ѯCDP������
 @Input				pClient								: NBIOT�ͻ���ʵ��
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadCDPServer(NBIOT_ClientsTypeDef* pClient)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	Stm32_CalculagraphTypeDef ATCmd_timer_Ms;
	
	Stm32_Calculagraph_CountdownMS(&ATCmd_timer_Ms, pClient->Command_Timeout_Msec);
	pClient->ATCmdStack->CmdWaitTime = ATCmd_timer_Ms;
	
	memset((void *)&pClient->Parameter.cdpserver, 0x0, sizeof(pClient->Parameter.cdpserver));
	
	sprintf((char *)pClient->ATCmdStack->ATSendbuf, "AT+NCDP?\r");
	pClient->ATCmdStack->ATSendlen = strlen("AT+NCDP?\r");
	pClient->ATCmdStack->ATack = "OK";
	pClient->ATCmdStack->ATNack = "ERROR";
	if ((NBStatus = pClient->ATCmdStack->Write(pClient->ATCmdStack)) == NBIOT_OK) {
		if (sscanf((const char*)pClient->ATCmdStack->ATRecvbuf, "\r\n+NCDP:%[^,],%hu\r", (char *)&pClient->Parameter.cdpserver.CDPServerHost, &pClient->Parameter.cdpserver.CDPServerPort) <= 0) {
			NBStatus = NBIOT_ERROR;
		}
	}
	
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Neul_NBxx_SetConfigUE(NBIOT_ClientsTypeDef* pClient, const char *ncmd, NBIOT_NConfigTypeDef state)
 @Description			NBIOT_Neul_NBxx_SetConfigUE				: ����NCONFIGģʽ
 @Input				pClient								: NBIOT�ͻ���ʵ��
					ncmd									: Neul������
					state								: Neul����ģʽ
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_SetConfigUE(NBIOT_ClientsTypeDef* pClient, const char *ncmd, NBIOT_NConfigTypeDef state)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	Stm32_CalculagraphTypeDef ATCmd_timer_Ms;
	
	Stm32_Calculagraph_CountdownMS(&ATCmd_timer_Ms, pClient->Command_Timeout_Msec);
	pClient->ATCmdStack->CmdWaitTime = ATCmd_timer_Ms;
	
	memset((void *)pClient->DataProcessStack, 0x0, pClient->DataProcessStack_size);
	sprintf((char *)pClient->DataProcessStack, "AT+NCONFIG=%s,%s\r", ncmd, (state == NConfigTrue)?"TRUE":"FALSE");
	
	memcpy(pClient->ATCmdStack->ATSendbuf, pClient->DataProcessStack, strlen((char *)pClient->DataProcessStack));
	pClient->ATCmdStack->ATSendlen = strlen((char *)pClient->DataProcessStack);
	pClient->ATCmdStack->ATack = "OK";
	pClient->ATCmdStack->ATNack = "ERROR";
	NBStatus = pClient->ATCmdStack->Write(pClient->ATCmdStack);
	
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadConfigUE(NBIOT_ClientsTypeDef* pClient)
 @Description			NBIOT_Neul_NBxx_CheckReadCDPServer			: ��ѯNCONFIGģʽ
 @Input				pClient								: NBIOT�ͻ���ʵ��
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CheckReadConfigUE(NBIOT_ClientsTypeDef* pClient)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	Stm32_CalculagraphTypeDef ATCmd_timer_Ms;
	
	Stm32_Calculagraph_CountdownMS(&ATCmd_timer_Ms, pClient->Command_Timeout_Msec);
	pClient->ATCmdStack->CmdWaitTime = ATCmd_timer_Ms;
	
	sprintf((char *)pClient->ATCmdStack->ATSendbuf, "AT+NCONFIG?\r");
	pClient->ATCmdStack->ATSendlen = strlen("AT+NCONFIG?\r");
	pClient->ATCmdStack->ATack = "OK";
	pClient->ATCmdStack->ATNack = "ERROR";
	if ((NBStatus = pClient->ATCmdStack->Write(pClient->ATCmdStack)) == NBIOT_OK) {
		if (strstr((const char*)pClient->ATCmdStack->ATRecvbuf, "+NCONFIG:AUTOCONNECT,TRUE")) {
			pClient->Parameter.nconfig.autoConnect = NConfigTrue;
		}
		else if (strstr((const char*)pClient->ATCmdStack->ATRecvbuf, "+NCONFIG:AUTOCONNECT,FALSE")) {
			pClient->Parameter.nconfig.autoConnect = NConfigFalse;
		}
		else {
			NBStatus = NBIOT_ERROR;
			goto exit;
		}
		
		if (strstr((const char*)pClient->ATCmdStack->ATRecvbuf, "+NCONFIG:CR_0354_0338_SCRAMBLING,TRUE")) {
			pClient->Parameter.nconfig.crScrambling = NConfigTrue;
		}
		else if (strstr((const char*)pClient->ATCmdStack->ATRecvbuf, "+NCONFIG:CR_0354_0338_SCRAMBLING,FALSE")) {
			pClient->Parameter.nconfig.crScrambling = NConfigFalse;
		}
		else {
			NBStatus = NBIOT_ERROR;
			goto exit;
		}
		
		if (strstr((const char*)pClient->ATCmdStack->ATRecvbuf, "+NCONFIG:CR_0859_SI_AVOID,TRUE")) {
			pClient->Parameter.nconfig.crSiAvoid = NConfigTrue;
		}
		else if (strstr((const char*)pClient->ATCmdStack->ATRecvbuf, "+NCONFIG:CR_0859_SI_AVOID,FALSE")) {
			pClient->Parameter.nconfig.crSiAvoid = NConfigFalse;
		}
		else {
			NBStatus = NBIOT_ERROR;
			goto exit;
		}
		
		if (strstr((const char*)pClient->ATCmdStack->ATRecvbuf, "+NCONFIG:COMBINE_ATTACH,TRUE")) {
			pClient->Parameter.nconfig.combineAttach = NConfigTrue;
		}
		else if (strstr((const char*)pClient->ATCmdStack->ATRecvbuf, "+NCONFIG:COMBINE_ATTACH,FALSE")) {
			pClient->Parameter.nconfig.combineAttach = NConfigFalse;
		}
		else {
			NBStatus = NBIOT_ERROR;
			goto exit;
		}
		
		if (strstr((const char*)pClient->ATCmdStack->ATRecvbuf, "+NCONFIG:CELL_RESELECTION,TRUE")) {
			pClient->Parameter.nconfig.cellReselection = NConfigTrue;
		}
		else if (strstr((const char*)pClient->ATCmdStack->ATRecvbuf, "+NCONFIG:CELL_RESELECTION,FALSE")) {
			pClient->Parameter.nconfig.cellReselection = NConfigFalse;
		}
		else {
			NBStatus = NBIOT_ERROR;
			goto exit;
		}
		
		if (strstr((const char*)pClient->ATCmdStack->ATRecvbuf, "+NCONFIG:ENABLE_BIP,TRUE")) {
			pClient->Parameter.nconfig.enableBip = NConfigTrue;
		}
		else if (strstr((const char*)pClient->ATCmdStack->ATRecvbuf, "+NCONFIG:ENABLE_BIP,FALSE")) {
			pClient->Parameter.nconfig.enableBip = NConfigFalse;
		}
		else {
			NBStatus = NBIOT_ERROR;
			goto exit;
		}
	}
	
exit:
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Neul_NBxx_QuerySendMessageCOAPPayload(NBIOT_ClientsTypeDef* pClient)
 @Description			NBIOT_Neul_NBxx_QuerySendMessageCOAPPayload	: COAP��ѯ��Ϣ���ʹ�NB
 @Input				pClient								: NBIOT�ͻ���ʵ��
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_QuerySendMessageCOAPPayload(NBIOT_ClientsTypeDef* pClient)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	Stm32_CalculagraphTypeDef ATCmd_timer_Ms;
	
	Stm32_Calculagraph_CountdownMS(&ATCmd_timer_Ms, pClient->Command_Timeout_Msec);
	pClient->ATCmdStack->CmdWaitTime = ATCmd_timer_Ms;
	
	sprintf((char *)pClient->ATCmdStack->ATSendbuf, "AT+NQMGS\r");
	pClient->ATCmdStack->ATSendlen = strlen("AT+NQMGS\r");
	pClient->ATCmdStack->ATack = "OK";
	pClient->ATCmdStack->ATNack = NULL;
	if ((NBStatus = pClient->ATCmdStack->Write(pClient->ATCmdStack)) == NBIOT_OK) {
		if (sscanf((const char*)pClient->ATCmdStack->ATRecvbuf, "\r\nPENDING=%d,SENT=%d,ERROR=%d\r", &pClient->Parameter.coapSendMessage.pending,
																				   &pClient->Parameter.coapSendMessage.sent,
																				   &pClient->Parameter.coapSendMessage.error) <= 0) {
			NBStatus = NBIOT_ERROR;
		}
	}
	
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Neul_NBxx_QueryReadMessageCOAPPayload(NBIOT_ClientsTypeDef* pClient)
 @Description			NBIOT_Neul_NBxx_QueryReadMessageCOAPPayload	: COAP��ѯ��Ϣ�Ƿ�����
 @Input				pClient								: NBIOT�ͻ���ʵ��
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_QueryReadMessageCOAPPayload(NBIOT_ClientsTypeDef* pClient)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	Stm32_CalculagraphTypeDef ATCmd_timer_Ms;
	
	Stm32_Calculagraph_CountdownMS(&ATCmd_timer_Ms, pClient->Command_Timeout_Msec);
	pClient->ATCmdStack->CmdWaitTime = ATCmd_timer_Ms;
	
	sprintf((char *)pClient->ATCmdStack->ATSendbuf, "AT+NQMGR\r");
	pClient->ATCmdStack->ATSendlen = strlen("AT+NQMGR\r");
	pClient->ATCmdStack->ATack = "OK";
	pClient->ATCmdStack->ATNack = "ERROR";
	if ((NBStatus = pClient->ATCmdStack->Write(pClient->ATCmdStack)) == NBIOT_OK) {
		if (sscanf((const char*)pClient->ATCmdStack->ATRecvbuf, "\r\nBUFFERED=%d,RECEIVED=%d,DROPPED=%d\r", &pClient->Parameter.coapReadMessage.buffered,
																						&pClient->Parameter.coapReadMessage.received,
																						&pClient->Parameter.coapReadMessage.dropped) <= 0) {
			NBStatus = NBIOT_ERROR;
		}
	}
	
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Neul_NBxx_SendCOAPPayload(NBIOT_ClientsTypeDef* pClient, const char *buf, u16 sendlen)
 @Description			NBIOT_Neul_NBxx_SendCOAPPayload			: COAP����һ����������
 @Input				pClient								: NBIOT�ͻ���ʵ��
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
 @attention			����ע���ɹ��ſɷ��͸�������
					�����Ч���ݳ���Ϊ512�ֽ�
					ÿ��ֻ�ܻ���һ����Ϣ
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_SendCOAPPayload(NBIOT_ClientsTypeDef* pClient)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	Stm32_CalculagraphTypeDef ATCmd_timer_Ms;
	u16 length = 0;
	
	if ((pClient->Sendlen > pClient->Sendbuf_size) || (((2 * pClient->Sendlen) + 15) > pClient->DataProcessStack_size) || (((2 * pClient->Sendlen) + 15) > pClient->ATCmdStack->ATSendbuf_size)) {
		NBStatus = NBIOT_ERROR;
		goto exit;
	}
	
	Stm32_Calculagraph_CountdownMS(&ATCmd_timer_Ms, pClient->Command_Timeout_Msec);
	pClient->ATCmdStack->CmdWaitTime = ATCmd_timer_Ms;
	
	memset((void *)pClient->DataProcessStack, 0x0, pClient->DataProcessStack_size);
	sprintf((char *)pClient->DataProcessStack, "AT+NMGS=%d,", pClient->Sendlen);
	length = strlen((const char*)pClient->DataProcessStack);
	for (int i = 0; i < pClient->Sendlen; i++) {
		sprintf((char *)(pClient->DataProcessStack + length + i * 2), "%02X", pClient->Sendbuf[i]);
	}
	length = length + pClient->Sendlen * 2;
	sprintf((char *)(pClient->DataProcessStack + length), "%c", '\r');
	
	memcpy(pClient->ATCmdStack->ATSendbuf, pClient->DataProcessStack, strlen((char *)pClient->DataProcessStack));
	pClient->ATCmdStack->ATSendlen = strlen((char *)pClient->DataProcessStack);
	pClient->ATCmdStack->ATack = "OK";
	pClient->ATCmdStack->ATNack = "ERROR";
	NBStatus = pClient->ATCmdStack->Write(pClient->ATCmdStack);
	
exit:
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Neul_NBxx_ReadCOAPPayload(NBIOT_ClientsTypeDef* pClient)
 @Description			NBIOT_Neul_NBxx_ReadCOAPPayload			: COAP��ȡһ����������
 @Input				pClient								: NBIOT�ͻ���ʵ��
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_ReadCOAPPayload(NBIOT_ClientsTypeDef* pClient)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	Stm32_CalculagraphTypeDef ATCmd_timer_Ms;
	u32 utmp = 0;
	
	Stm32_Calculagraph_CountdownMS(&ATCmd_timer_Ms, pClient->Command_Timeout_Msec);
	pClient->ATCmdStack->CmdWaitTime = ATCmd_timer_Ms;
	
	memset((void *)pClient->DataProcessStack, 0x0, pClient->DataProcessStack_size);
	sprintf((char *)pClient->ATCmdStack->ATSendbuf, "AT+NMGR\r");
	pClient->ATCmdStack->ATSendlen = strlen("AT+NMGR\r");
	pClient->ATCmdStack->ATack = "OK";
	pClient->ATCmdStack->ATNack = "ERROR";
	if ((NBStatus = pClient->ATCmdStack->Write(pClient->ATCmdStack)) == NBIOT_OK) {
		if (sscanf((const char*)pClient->ATCmdStack->ATRecvbuf, "\r\n%hd,%s\r", &pClient->Recvlen, pClient->DataProcessStack) <= 0) {
			NBStatus = NBIOT_ERROR;
			goto exit;
		}
		memset((void *)pClient->Recvbuf, 0x0, pClient->Recvbuf_size);
		for (int i = 0; i < pClient->Recvlen; i++) {
			sscanf((const char*)(pClient->DataProcessStack + i * 2), "%02X", &utmp);
			pClient->Recvbuf[i] = utmp;
		}
	}
	
exit:
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CreateUDPSocket(NBIOT_ClientsTypeDef* pClient, u16 localport, u8 receivectl, int *socket)
 @Description			NBIOT_Neul_NBxx_CreateUDPSocket			: ����UDP����
 @Input				pClient								: NBIOT�ͻ���ʵ��
					localport								: 0 ~ 65535
					receivectl							: 1 is Enable Receive 0 is Ignored Receive
					socket								: Created Socket Num
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CreateUDPSocket(NBIOT_ClientsTypeDef* pClient, u16 localport, u8 receivectl, int *socket)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	Stm32_CalculagraphTypeDef ATCmd_timer_Ms;
	
	Stm32_Calculagraph_CountdownMS(&ATCmd_timer_Ms, pClient->Command_Timeout_Msec);
	pClient->ATCmdStack->CmdWaitTime = ATCmd_timer_Ms;
	
	memset((void *)pClient->DataProcessStack, 0x0, pClient->DataProcessStack_size);
	sprintf((char *)pClient->DataProcessStack, "AT+NSOCR=%s,%d,%d,%d\r", "DGRAM", 17, localport, receivectl);
	
	memcpy(pClient->ATCmdStack->ATSendbuf, pClient->DataProcessStack, strlen((char *)pClient->DataProcessStack));
	pClient->ATCmdStack->ATSendlen = strlen((char *)pClient->DataProcessStack);
	pClient->ATCmdStack->ATack = "OK";
	pClient->ATCmdStack->ATNack = "ERROR";
	if ((NBStatus = pClient->ATCmdStack->Write(pClient->ATCmdStack)) == NBIOT_OK) {
		if (sscanf((const char*)pClient->ATCmdStack->ATRecvbuf, "\r\n%d\r\n", socket) <= 0) {
			NBStatus = NBIOT_ERROR;
		}
	}
	
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CloseUDPSocket(NBIOT_ClientsTypeDef* pClient, int socket)
 @Description			NBIOT_Neul_NBxx_CloseUDPSocket			: �ر�UDP����
 @Input				pClient								: NBIOT�ͻ���ʵ��
					socket								: Closeed Socket Num
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_CloseUDPSocket(NBIOT_ClientsTypeDef* pClient, int socket)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	Stm32_CalculagraphTypeDef ATCmd_timer_Ms;
	
	Stm32_Calculagraph_CountdownMS(&ATCmd_timer_Ms, pClient->Command_Timeout_Msec);
	pClient->ATCmdStack->CmdWaitTime = ATCmd_timer_Ms;
	
	memset((void *)pClient->DataProcessStack, 0x0, pClient->DataProcessStack_size);
	sprintf((char *)pClient->DataProcessStack, "AT+NSOCL=%d\r", socket);
	
	memcpy(pClient->ATCmdStack->ATSendbuf, pClient->DataProcessStack, strlen((char *)pClient->DataProcessStack));
	pClient->ATCmdStack->ATSendlen = strlen((char *)pClient->DataProcessStack);
	pClient->ATCmdStack->ATack = "OK";
	pClient->ATCmdStack->ATNack = "ERROR";
	NBStatus = pClient->ATCmdStack->Write(pClient->ATCmdStack);
	
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Neul_NBxx_SendUDPPayload(NBIOT_ClientsTypeDef* pClient, int socket, const char *addr, u16 port)
 @Description			NBIOT_Neul_NBxx_SendUDPPayload			: UDP���͸�������
 @Input				pClient								: NBIOT�ͻ���ʵ��
					socket								: Socket Num
					addr									: Ŀ��IP
					port									: Ŀ��˿�
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_SendUDPPayload(NBIOT_ClientsTypeDef* pClient, int socket, const char *addr, u16 port)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	Stm32_CalculagraphTypeDef ATCmd_timer_Ms;
	u16 length = 0;
	u16 rdsocket = 0;
	u16 rdlength = 0;
	
	if ((pClient->Sendlen > pClient->Sendbuf_size) || (((2 * pClient->Sendlen) + 15) > pClient->DataProcessStack_size) || (((2 * pClient->Sendlen) + 15) > pClient->ATCmdStack->ATSendbuf_size)) {
		NBStatus = NBIOT_ERROR;
		goto exit;
	}
	
	Stm32_Calculagraph_CountdownMS(&ATCmd_timer_Ms, pClient->Command_Timeout_Msec);
	pClient->ATCmdStack->CmdWaitTime = ATCmd_timer_Ms;
	
	memset((void *)pClient->DataProcessStack, 0x0, pClient->DataProcessStack_size);
	sprintf((char *)pClient->DataProcessStack, "AT+NSOST=%d,%s,%d,%d,", socket, addr, port, pClient->Sendlen);
	length = strlen((const char*)pClient->DataProcessStack);
	for (int i = 0; i < pClient->Sendlen; i++) {
		sprintf((char *)(pClient->DataProcessStack + length + i * 2), "%02X", pClient->Sendbuf[i]);
	}
	length = length + pClient->Sendlen * 2;
	sprintf((char *)(pClient->DataProcessStack + length), "%c", '\r');
	
	memcpy(pClient->ATCmdStack->ATSendbuf, pClient->DataProcessStack, strlen((char *)pClient->DataProcessStack));
	pClient->ATCmdStack->ATSendlen = strlen((char *)pClient->DataProcessStack);
	pClient->ATCmdStack->ATack = "OK";
	pClient->ATCmdStack->ATNack = "ERROR";
	if ((NBStatus = pClient->ATCmdStack->Write(pClient->ATCmdStack)) == NBIOT_OK) {
		if (sscanf((const char*)pClient->ATCmdStack->ATRecvbuf, "\r\n%hu,%hu\r\n", &rdsocket, &rdlength) <= 0) {
			NBStatus = NBIOT_ERROR;
		}
		else {
			if ((rdsocket != socket) || (rdlength != pClient->Sendlen)) {
				NBStatus = NBIOT_ERROR;
				goto exit;
			}
		}
	}
	
exit:
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Neul_NBxx_SendUDPPayloadFlag(NBIOT_ClientsTypeDef* pClient, int socket, const char *addr, u16 port, const char *flag)
 @Description			NBIOT_Neul_NBxx_SendUDPPayloadFlag			: UDP���͸�������(Я����־)
 @Input				pClient								: NBIOT�ͻ���ʵ��
					socket								: Socket Num
					addr									: Ŀ��IP
					port									: Ŀ��˿�
					flag							0x100	: ���͸����ȼ���Ϣ
												0x200	: ���ͳɹ������Ͻ���IDLE
												0x400	: �������ݵõ��ظ������IDLE
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_SendUDPPayloadFlag(NBIOT_ClientsTypeDef* pClient, int socket, const char *addr, u16 port, const char *flag)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	Stm32_CalculagraphTypeDef ATCmd_timer_Ms;
	u16 length = 0;
	u16 rdsocket = 0;
	u16 rdlength = 0;
	
	if ((pClient->Sendlen > pClient->Sendbuf_size) || (((2 * pClient->Sendlen) + 15) > pClient->DataProcessStack_size) || (((2 * pClient->Sendlen) + 15) > pClient->ATCmdStack->ATSendbuf_size)) {
		NBStatus = NBIOT_ERROR;
		goto exit;
	}
	
	Stm32_Calculagraph_CountdownMS(&ATCmd_timer_Ms, pClient->Command_Timeout_Msec);
	pClient->ATCmdStack->CmdWaitTime = ATCmd_timer_Ms;
	
	memset((void *)pClient->DataProcessStack, 0x0, pClient->DataProcessStack_size);
	sprintf((char *)pClient->DataProcessStack, "AT+NSOSTF=%d,%s,%d,%s,%d,", socket, addr, port, flag, pClient->Sendlen);
	length = strlen((const char*)pClient->DataProcessStack);
	for (int i = 0; i < pClient->Sendlen; i++) {
		sprintf((char *)(pClient->DataProcessStack + length + i * 2), "%02X", pClient->Sendbuf[i]);
	}
	length = length + pClient->Sendlen * 2;
	sprintf((char *)(pClient->DataProcessStack + length), "%c", '\r');
	
	memcpy(pClient->ATCmdStack->ATSendbuf, pClient->DataProcessStack, strlen((char *)pClient->DataProcessStack));
	pClient->ATCmdStack->ATSendlen = strlen((char *)pClient->DataProcessStack);
	pClient->ATCmdStack->ATack = "OK";
	pClient->ATCmdStack->ATNack = "ERROR";
	if ((NBStatus = pClient->ATCmdStack->Write(pClient->ATCmdStack)) == NBIOT_OK) {
		if (sscanf((const char*)pClient->ATCmdStack->ATRecvbuf, "\r\n%hu,%hu\r\n", &rdsocket, &rdlength) <= 0) {
			NBStatus = NBIOT_ERROR;
		}
		else {
			if ((rdsocket != socket) || (rdlength != pClient->Sendlen)) {
				NBStatus = NBIOT_ERROR;
				goto exit;
			}
		}
	}
	
exit:
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Neul_NBxx_ReadUDPPayload(NBIOT_ClientsTypeDef* pClient, int socket, int maxrlen, int *rleft, char *addr, u16 *port)
 @Description			NBIOT_Neul_NBxx_ReadUDPPayload			: UDP��ȡ��������
 @Input				pClient								: NBIOT�ͻ���ʵ��
					socket								: Socket Num
					maxrlen								: ����ȡ���ݴ�С
					rleft								: ʣ���ȡ���ݳ���
					addr									: Ŀ��IP
					port									: Ŀ��˿�
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Neul_NBxx_ReadUDPPayload(NBIOT_ClientsTypeDef* pClient, int socket, int maxrlen, int *rleft, char *addr, u16 *port)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	Stm32_CalculagraphTypeDef ATCmd_timer_Ms;
	u32 utmp = 0;
	int rdsocket = 0;
	
	Stm32_Calculagraph_CountdownMS(&ATCmd_timer_Ms, pClient->Command_Timeout_Msec);
	pClient->ATCmdStack->CmdWaitTime = ATCmd_timer_Ms;
	
	memset((void *)pClient->DataProcessStack, 0x0, pClient->DataProcessStack_size);
	sprintf((char *)pClient->DataProcessStack, "AT+NSORF=%d,%d\r", socket, maxrlen);
	
	memcpy(pClient->ATCmdStack->ATSendbuf, pClient->DataProcessStack, strlen((char *)pClient->DataProcessStack));
	pClient->ATCmdStack->ATSendlen = strlen((char *)pClient->DataProcessStack);
	pClient->ATCmdStack->ATack = "OK";
	pClient->ATCmdStack->ATNack = "ERROR";
	memset((void *)pClient->DataProcessStack, 0x0, pClient->DataProcessStack_size);
	if ((NBStatus = pClient->ATCmdStack->Write(pClient->ATCmdStack)) == NBIOT_OK) {
		if (sscanf((const char*)pClient->ATCmdStack->ATRecvbuf, "\x0D\x0A%d,%[^,],%hu,%hd,%[^,],%d\r", &rdsocket, addr, port, &pClient->Recvlen, pClient->DataProcessStack, rleft) <= 0) {
			NBStatus = NBIOT_ERROR;
			goto exit;
		}
		else {
			if (rdsocket != socket) {
				NBStatus = NBIOT_ERROR;
				goto exit;
			}
		}
		memset((void *)pClient->Recvbuf, 0x0, pClient->Recvbuf_size);
		for (int i = 0; i < pClient->Recvlen; i++) {
			sscanf((const char*)(pClient->DataProcessStack + i * 2), "%02X", &utmp);
			pClient->Recvbuf[i] = utmp;
		}
	}
	
exit:
	return NBStatus;
}

/********************************************** END OF FLEE **********************************************/
