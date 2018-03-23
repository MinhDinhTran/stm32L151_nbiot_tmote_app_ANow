/**
  *********************************************************************************************************
  * @file    net_mqttsn_message_operate.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2018-03-02
  * @brief   
  *********************************************************************************************************
  * @attention
  *
  *
  *
  *********************************************************************************************************
  */

#include "net_mqttsn_message_operate.h"
#include "hal_rtc.h"
#include "string.h"

MQTTSN_SwapStatusBasicTypeDef		NETMqttSNMessageParkStatusBasic;
MQTTSN_SwapStatusExtendTypeDef	NETMqttSNMessageParkStatusExtend;

/**********************************************************************************************************
 @Function			int NET_Message_Operate_Creat_Json_MoteStatus_Basic(char* outBuffer)
 @Description			NET_Message_Operate_Creat_Json_MoteStatus_Basic
 @Input				outBuffer
 @Return				Length
**********************************************************************************************************/
int NET_Message_Operate_Creat_Json_MoteStatus_Basic(char* outBuffer)
{
	MQTTSN_StatusBasicTypeDef dataBuf;
	struct tm datatime;
	
	if (NET_MqttSN_Message_StatusBasicDequeue(&dataBuf) != true) {
		return 0;
	}
	
	datatime = RTC_ConvUnixToCalendar(dataBuf.DateTime + 8 * 3600);
	
	sprintf(outBuffer, 
		"{"
			"\"SN\":\"%08x\","
			"\"Name\":\"TMoteStatus\","
			"\"TMoteStatus\":"
			"{"
				"\"Status\":%d,"
				"\"Count\":%d,"
				"\"Time\":\"20%02d-%02d-%02d %02d:%02d:%02d\""
			"}"
		"}",
		
		dataBuf.DeviceSN,
		dataBuf.Status,
		dataBuf.Count,
		datatime.tm_year, datatime.tm_mon, datatime.tm_mday,
		datatime.tm_hour, datatime.tm_min, datatime.tm_sec
	);
	
	return strlen(outBuffer);
}

/**********************************************************************************************************
 @Function			int NET_Message_Operate_Creat_Json_MoteStatus_Extend(char* outBuffer)
 @Description			NET_Message_Operate_Creat_Json_MoteStatus_Extend
 @Input				outBuffer
 @Return				Length
**********************************************************************************************************/
int NET_Message_Operate_Creat_Json_MoteStatus_Extend(char* outBuffer)
{
	MQTTSN_StatusExtendTypeDef dataBuf;
	struct tm datatime;
	
	if (NET_MqttSN_Message_StatusExtendDequeue(&dataBuf) != true) {
		return 0;
	}
	
	datatime = RTC_ConvUnixToCalendar(dataBuf.DateTime + 8 * 3600);
	
	sprintf(outBuffer, 
		"{"
			"\"SN\":\"%08x\","
			"\"Name\":\"TMoteStatus\","
			"\"TMoteStatus\":"
			"{"
				"\"Status\":%d,"
				"\"Count\":%d,"
				"\"Time\":\"20%02d-%02d-%02d %02d:%02d:%02d\","
				"\"x\":%d,"
				"\"y\":%d,"
				"\"z\":%d,"
				"\"MagDiff\":%d,"
				"\"Distance\":%d,"
				"\"Strength\":%d,"
				"\"CoverCount\":%d,"
				"\"RadarDiff\":%d"
			"}"
		"}",
		
		dataBuf.DeviceSN,
		dataBuf.Status,
		dataBuf.Count,
		datatime.tm_year, datatime.tm_mon, datatime.tm_mday,
		datatime.tm_hour, datatime.tm_min, datatime.tm_sec,
		dataBuf.MagX,
		dataBuf.MagY,
		dataBuf.MagZ,
		dataBuf.MagDiff,
		dataBuf.Distance,
		dataBuf.Strength,
		dataBuf.CoverCount,
		dataBuf.RadarDiff
	);
	
	return strlen(outBuffer);
}

/********************************************Is Full******************************************************/
/**********************************************************************************************************
 @Function			static bool NET_MqttSN_Message_StatusBasicisFull(void)
 @Description			NET_MqttSN_Message_StatusBasicisFull
 @Input				void
 @Return				true							: 已满
					false						: 未满
**********************************************************************************************************/
static bool NET_MqttSN_Message_StatusBasicisFull(void)
{
	bool MessageState;
	
	if ((NETMqttSNMessageParkStatusBasic.Rear + 1) % MQTTSN_STATUS_BASIC_PARK_NUM == NETMqttSNMessageParkStatusBasic.Front) {
		MessageState = true;
	}
	else {
		MessageState = false;
	}
	
	return MessageState;
}

/**********************************************************************************************************
 @Function			static bool NET_MqttSN_Message_StatusExtendisFull(void)
 @Description			NET_MqttSN_Message_StatusExtendisFull
 @Input				void
 @Return				true							: 已满
					false						: 未满
**********************************************************************************************************/
static bool NET_MqttSN_Message_StatusExtendisFull(void)
{
	bool MessageState;
	
	if ((NETMqttSNMessageParkStatusExtend.Rear + 1) % MQTTSN_STATUS_EXTEND_PARK_NUM == NETMqttSNMessageParkStatusExtend.Front) {
		MessageState = true;
	}
	else {
		MessageState = false;
	}
	
	return MessageState;
}

/********************************************Is Empty*****************************************************/
/**********************************************************************************************************
 @Function			bool NET_MqttSN_Message_StatusBasicisEmpty(void)
 @Description			NET_MqttSN_Message_StatusBasicisEmpty
 @Input				void
 @Return				true							: 已空
					false						: 未空
**********************************************************************************************************/
bool NET_MqttSN_Message_StatusBasicisEmpty(void)
{
	bool MessageState;
	
	if (NETMqttSNMessageParkStatusBasic.Front == NETMqttSNMessageParkStatusBasic.Rear) {
		MessageState = true;
	}
	else {
		MessageState = false;
	}
	
	return MessageState;
}

/**********************************************************************************************************
 @Function			bool NET_MqttSN_Message_StatusExtendisEmpty(void)
 @Description			NET_MqttSN_Message_StatusExtendisEmpty
 @Input				void
 @Return				true							: 已空
					false						: 未空
**********************************************************************************************************/
bool NET_MqttSN_Message_StatusExtendisEmpty(void)
{
	bool MessageState;
	
	if (NETMqttSNMessageParkStatusExtend.Front == NETMqttSNMessageParkStatusExtend.Rear) {
		MessageState = true;
	}
	else {
		MessageState = false;
	}
	
	return MessageState;
}

/******************************************** Enqueue ****************************************************/
/**********************************************************************************************************
 @Function			void NET_MqttSN_Message_StatusBasicEnqueue(StatusBasicTypeDef dataBuf)
 @Description			NET_MqttSN_Message_StatusBasicEnqueue
 @Input				dataBuf	 		 				: 需写入数据
 @Return				void
**********************************************************************************************************/
void NET_MqttSN_Message_StatusBasicEnqueue(MQTTSN_StatusBasicTypeDef dataBuf)
{
	if (NET_MqttSN_Message_StatusBasicisFull() == true) {															//队列已满
		NETMqttSNMessageParkStatusBasic.Rear = (NETMqttSNMessageParkStatusBasic.Rear + 1) % MQTTSN_STATUS_BASIC_PARK_NUM;		//队尾偏移1
		NETMqttSNMessageParkStatusBasic.StatusBasic[NETMqttSNMessageParkStatusBasic.Rear].DeviceSN = dataBuf.DeviceSN;
		NETMqttSNMessageParkStatusBasic.StatusBasic[NETMqttSNMessageParkStatusBasic.Rear].Status = dataBuf.Status;
		NETMqttSNMessageParkStatusBasic.StatusBasic[NETMqttSNMessageParkStatusBasic.Rear].Count = dataBuf.Count;
		NETMqttSNMessageParkStatusBasic.StatusBasic[NETMqttSNMessageParkStatusBasic.Rear].DateTime = dataBuf.DateTime;
		NETMqttSNMessageParkStatusBasic.Front = (NETMqttSNMessageParkStatusBasic.Front + 1) % MQTTSN_STATUS_BASIC_PARK_NUM;	//队头偏移1
	}
	else {																								//队列未满
		NETMqttSNMessageParkStatusBasic.Rear = (NETMqttSNMessageParkStatusBasic.Rear + 1) % MQTTSN_STATUS_BASIC_PARK_NUM;		//队尾偏移1
		NETMqttSNMessageParkStatusBasic.StatusBasic[NETMqttSNMessageParkStatusBasic.Rear].DeviceSN = dataBuf.DeviceSN;
		NETMqttSNMessageParkStatusBasic.StatusBasic[NETMqttSNMessageParkStatusBasic.Rear].Status = dataBuf.Status;
		NETMqttSNMessageParkStatusBasic.StatusBasic[NETMqttSNMessageParkStatusBasic.Rear].Count = dataBuf.Count;
		NETMqttSNMessageParkStatusBasic.StatusBasic[NETMqttSNMessageParkStatusBasic.Rear].DateTime = dataBuf.DateTime;
	}
}

/**********************************************************************************************************
 @Function			void NET_MqttSN_Message_StatusExtendEnqueue(StatusExtendTypeDef dataBuf)
 @Description			NET_MqttSN_Message_StatusExtendEnqueue
 @Input				dataBuf	 		 				: 需写入数据
 @Return				void
**********************************************************************************************************/
void NET_MqttSN_Message_StatusExtendEnqueue(MQTTSN_StatusExtendTypeDef dataBuf)
{
	if (NET_MqttSN_Message_StatusExtendisFull() == true) {															//队列已满
		NETMqttSNMessageParkStatusExtend.Rear = (NETMqttSNMessageParkStatusExtend.Rear + 1) % MQTTSN_STATUS_EXTEND_PARK_NUM;	//队尾偏移1
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].DeviceSN = dataBuf.DeviceSN;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].Status = dataBuf.Status;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].Count = dataBuf.Count;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].DateTime = dataBuf.DateTime;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].MagX = dataBuf.MagX;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].MagY = dataBuf.MagY;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].MagZ = dataBuf.MagZ;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].MagDiff = dataBuf.MagDiff;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].Distance = dataBuf.Distance;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].Strength = dataBuf.Strength;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].CoverCount = dataBuf.CoverCount;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].RadarDiff = dataBuf.RadarDiff;
		NETMqttSNMessageParkStatusExtend.Front = (NETMqttSNMessageParkStatusExtend.Front + 1) % MQTTSN_STATUS_EXTEND_PARK_NUM;	//队头偏移1
	}
	else {																								//队列未满
		NETMqttSNMessageParkStatusExtend.Rear = (NETMqttSNMessageParkStatusExtend.Rear + 1) % MQTTSN_STATUS_EXTEND_PARK_NUM;	//队尾偏移1
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].DeviceSN = dataBuf.DeviceSN;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].Status = dataBuf.Status;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].Count = dataBuf.Count;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].DateTime = dataBuf.DateTime;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].MagX = dataBuf.MagX;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].MagY = dataBuf.MagY;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].MagZ = dataBuf.MagZ;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].MagDiff = dataBuf.MagDiff;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].Distance = dataBuf.Distance;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].Strength = dataBuf.Strength;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].CoverCount = dataBuf.CoverCount;
		NETMqttSNMessageParkStatusExtend.StatusExtend[NETMqttSNMessageParkStatusExtend.Rear].RadarDiff = dataBuf.RadarDiff;
	}
}

/******************************************** Dequeue ****************************************************/
/**********************************************************************************************************
 @Function			bool NET_MqttSN_Message_StatusBasicDequeue(StatusBasicTypeDef* dataBuf)
 @Description			NET_MqttSN_Message_StatusBasicDequeue
 @Input				dataBuf	 		 				: 需读出数据地址
 @Return				true								: 未空
					false							: 已空
**********************************************************************************************************/
bool NET_MqttSN_Message_StatusBasicDequeue(MQTTSN_StatusBasicTypeDef* dataBuf)
{
	bool MessageState;
	unsigned char front;
	
	if (NET_MqttSN_Message_StatusBasicisEmpty() == true) {															//队列已空
		MessageState = false;
	}
	else {																								//队列未空
		front = (NETMqttSNMessageParkStatusBasic.Front + 1) % MQTTSN_STATUS_BASIC_PARK_NUM;								//队头偏移1
		dataBuf->DeviceSN = NETMqttSNMessageParkStatusBasic.StatusBasic[front].DeviceSN;
		dataBuf->Status = NETMqttSNMessageParkStatusBasic.StatusBasic[front].Status;
		dataBuf->Count = NETMqttSNMessageParkStatusBasic.StatusBasic[front].Count;
		dataBuf->DateTime = NETMqttSNMessageParkStatusBasic.StatusBasic[front].DateTime;
		MessageState = true;
	}
	
	return MessageState;
}

/**********************************************************************************************************
 @Function			bool NET_MqttSN_Message_StatusExtendDequeue(StatusExtendTypeDef* dataBuf)
 @Description			NET_MqttSN_Message_StatusExtendDequeue
 @Input				dataBuf	 		 				: 需读出数据地址
 @Return				true								: 未空
					false							: 已空
**********************************************************************************************************/
bool NET_MqttSN_Message_StatusExtendDequeue(MQTTSN_StatusExtendTypeDef* dataBuf)
{
	bool MessageState;
	unsigned char front;
	
	if (NET_MqttSN_Message_StatusExtendisEmpty() == true) {														//队列已空
		MessageState = false;
	}
	else {																								//队列未空
		front = (NETMqttSNMessageParkStatusExtend.Front + 1) % MQTTSN_STATUS_EXTEND_PARK_NUM;							//队头偏移1
		dataBuf->DeviceSN = NETMqttSNMessageParkStatusExtend.StatusExtend[front].DeviceSN;
		dataBuf->Status = NETMqttSNMessageParkStatusExtend.StatusExtend[front].Status;
		dataBuf->Count = NETMqttSNMessageParkStatusExtend.StatusExtend[front].Count;
		dataBuf->DateTime = NETMqttSNMessageParkStatusExtend.StatusExtend[front].DateTime;
		dataBuf->MagX = NETMqttSNMessageParkStatusExtend.StatusExtend[front].MagX;
		dataBuf->MagY = NETMqttSNMessageParkStatusExtend.StatusExtend[front].MagY;
		dataBuf->MagZ = NETMqttSNMessageParkStatusExtend.StatusExtend[front].MagZ;
		dataBuf->MagDiff = NETMqttSNMessageParkStatusExtend.StatusExtend[front].MagDiff;
		dataBuf->Distance = NETMqttSNMessageParkStatusExtend.StatusExtend[front].Distance;
		dataBuf->Strength = NETMqttSNMessageParkStatusExtend.StatusExtend[front].Strength;
		dataBuf->CoverCount = NETMqttSNMessageParkStatusExtend.StatusExtend[front].CoverCount;
		dataBuf->RadarDiff = NETMqttSNMessageParkStatusExtend.StatusExtend[front].RadarDiff;
		MessageState = true;
	}
	
	return MessageState;
}

/********************************************* OffSet ****************************************************/
/**********************************************************************************************************
 @Function			bool NET_MqttSN_Message_StatusBasicOffSet(void)
 @Description			NET_MqttSN_Message_StatusBasicOffSet
 @Input				void
 @Return				true							: 未空
					false						: 已空
**********************************************************************************************************/
bool NET_MqttSN_Message_StatusBasicOffSet(void)
{
	bool MessageState;
	
	if (NET_MqttSN_Message_StatusBasicisEmpty() == true) {															//队列已空
		MessageState = false;
	}
	else {																								//队列未空
		NETMqttSNMessageParkStatusBasic.Front = (NETMqttSNMessageParkStatusBasic.Front + 1) % MQTTSN_STATUS_BASIC_PARK_NUM;
		MessageState = true;
	}
	
	return MessageState;
}

/**********************************************************************************************************
 @Function			bool NET_MqttSN_Message_StatusExtendOffSet(void)
 @Description			NET_MqttSN_Message_StatusExtendOffSet
 @Input				void
 @Return				true							: 未空
					false						: 已空
**********************************************************************************************************/
bool NET_MqttSN_Message_StatusExtendOffSet(void)
{
	bool MessageState;
	
	if (NET_MqttSN_Message_StatusExtendisEmpty() == true) {														//队列已空
		MessageState = false;
	}
	else {																								//队列未空
		NETMqttSNMessageParkStatusExtend.Front = (NETMqttSNMessageParkStatusExtend.Front + 1) % MQTTSN_STATUS_EXTEND_PARK_NUM;
		MessageState = true;
	}
	
	return MessageState;
}

/********************************************* Rear *****************************************************/
/**********************************************************************************************************
 @Function			unsigned char NET_MqttSN_Message_StatusBasicRear(void)
 @Description			NET_MqttSN_Message_StatusBasicRear
 @Input				void
 @Return				队尾值
**********************************************************************************************************/
unsigned char NET_MqttSN_Message_StatusBasicRear(void)
{
	return NETMqttSNMessageParkStatusBasic.Rear;
}

/**********************************************************************************************************
 @Function			unsigned char NET_MqttSN_Message_StatusExtendRear(void)
 @Description			NET_MqttSN_Message_StatusExtendRear
 @Input				void
 @Return				队尾值
**********************************************************************************************************/
unsigned char NET_MqttSN_Message_StatusExtendRear(void)
{
	return NETMqttSNMessageParkStatusExtend.Rear;
}

























/********************************************** END OF FLEE **********************************************/
