/**
  *********************************************************************************************************
  * @file    net_coap_message_operate.c
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

#include "net_coap_message_operate.h"
#include "platform_config.h"
#include "platform_map.h"
#include "stm32l1xx_config.h"
#include "string.h"

COAP_SwapSendDataTypeDef		NETCoapMessageSendPark;
COAP_SwapRecvDataTypeDef		NETCoapMessageRecvPark;

/**********************************************************************************************************
 @Function			int NET_COAP_Message_Operate_Creat_Json_Work_Info(char* outBuffer)
 @Description			NET_COAP_Message_Operate_Creat_Json_Work_Info
 @Input				outBuffer
 @Return				Length
 @attention			!!<<< MaxLength 240Byte >>>!!
**********************************************************************************************************/
int NET_COAP_Message_Operate_Creat_Json_Work_Info(char* outBuffer)
{
	sprintf(outBuffer, 
		"{"
			"\"SN\":\"%08x\","
			"\"WorkInfo\":"
			"{"
				"\"Sense\":%d,"
				"\"Mode\":\"%s\","
				"\"Channel\":%d,"
			"}"
		"}",
		
		TCFG_EEPROM_Get_MAC_SN(),
		TCFG_EEPROM_GetSavedSensitivity(),
		TCFG_EEPROM_Get_WorkMode_String(),
		TCFG_EEPROM_GetRfChannel()
	);
	
	return strlen(outBuffer);
}

/**********************************************************************************************************
 @Function			int NET_COAP_Message_Operate_Creat_Json_Basic_Info(char* outBuffer)
 @Description			NET_COAP_Message_Operate_Creat_Json_Basic_Info
 @Input				outBuffer
 @Return				Length
 @attention			!!<<< MaxLength 240Byte >>>!!
**********************************************************************************************************/
int NET_COAP_Message_Operate_Creat_Json_Basic_Info(char* outBuffer)
{
	sprintf(outBuffer, 
		"{"
			"\"SN\":\"%08x\","
			"\"TMoteInfo\":"
			"{"
				"\"Type\":\"%d.1\","
				"\"Vender\":\"%s\","
				"\"Hard\":\"%d\","
				"\"Soft\":\"%d.%d\","
				"\"Build\":\"%s\","
				"\"Sim\":\"%s\","
				"\"Imei\":\"%s\","
				"\"Boot\":\"%d.%d\","
			"}"
		"}",
		
		TCFG_EEPROM_Get_MAC_SN(),
		MVB_MODEL_TYPE,
		TCFG_EEPROM_Get_Vender_String(),
		TCFG_Utility_Get_Major_Hardnumber(),
		TCFG_Utility_Get_Major_Softnumber(), TCFG_Utility_Get_Sub_Softnumber(),
		TCFG_Utility_Get_Build_Time_String(),
		TCFG_Utility_Get_Nbiot_Iccid_String(),
		TCFG_Utility_Get_Nbiot_Imei_String(),
		SoftResetFlag,
		TCFG_SystemData.DeviceBootCount
	);
	
	return strlen(outBuffer);
}

/**********************************************************************************************************
 @Function			int NET_COAP_Message_Operate_Creat_Json_Dynamic_Info(char* outBuffer)
 @Description			NET_COAP_Message_Operate_Creat_Json_Dynamic_Info
 @Input				outBuffer
 @Return				Length
 @attention			!!<<< MaxLength 240Byte >>>!!
**********************************************************************************************************/
int NET_COAP_Message_Operate_Creat_Json_Dynamic_Info(char* outBuffer)
{
	sprintf(outBuffer, 
		"{"
			"\"SN\":\"%08x\","
			"\"TMoteInfo\":"
			"{"
				"\"Runtime\":%d,"
				"\"Rssi\":%d,"
				"\"Batt\":%d,"
				"\"rlib\":\"%d\","
				"\"rcnt\":%d,"
				"\"temp\":%d,"
				"\"rdgb\":%d,"
				"\"psm\":%d,"
				"\"algo\":%d,"
				"\"qmcrbt\":%d,"
				"\"nbboot\":%d,"
				"\"nbsent\":%d,"
				"\"magmod\":%d,"
				"\"nbheart\":%d,"
			"}"
		"}",
		
		TCFG_EEPROM_Get_MAC_SN(),
		TCFG_Utility_Get_Run_Time(),
		TCFG_Utility_Get_Nbiot_Rssi_IntVal(),
		TCFG_Utility_Get_Device_Batt_ShortVal(),
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0
	);
	
	return strlen(outBuffer);
}

/**********************************************************************************************************
 @Function			static bool NET_Coap_Message_SendDataisFull(void)
 @Description			NET_Coap_Message_SendDataisFull	: 检查发送队列是否已满
 @Input				void
 @Return				true							: 已满
					false						: 未满
**********************************************************************************************************/
static bool NET_Coap_Message_SendDataisFull(void)
{
	bool MessageState;
	
	if ((NETCoapMessageSendPark.Rear + 1) % COAP_SEND_PARK_NUM == NETCoapMessageSendPark.Front) {
		MessageState = true;
	}
	else {
		MessageState = false;
	}
	
	return MessageState;
}

/**********************************************************************************************************
 @Function			static bool NET_Coap_Message_RecvDataisFull(void)
 @Description			NET_Coap_Message_SendDataisFull	: 检查接收队列是否已满
 @Input				void
 @Return				true							: 已满
					false						: 未满
**********************************************************************************************************/
static bool NET_Coap_Message_RecvDataisFull(void)
{
	bool MessageState;
	
	if ((NETCoapMessageRecvPark.Rear + 1) % COAP_RECV_PARK_NUM == NETCoapMessageRecvPark.Front) {
		MessageState = true;
	}
	else {
		MessageState = false;
	}
	
	return MessageState;
}

/**********************************************************************************************************
 @Function			static bool NET_Coap_Message_SendDataisEmpty(void)
 @Description			NET_Coap_Message_SendDataisEmpty	: 检查发送队列是否已空
 @Input				void
 @Return				true							: 已空
					false						: 未空
**********************************************************************************************************/
static bool NET_Coap_Message_SendDataisEmpty(void)
{
	bool MessageState;
	
	if (NETCoapMessageSendPark.Front == NETCoapMessageSendPark.Rear) {
		MessageState = true;
	}
	else {
		MessageState = false;
	}
	
	return MessageState;
}

/**********************************************************************************************************
 @Function			static bool NET_Coap_Message_RecvDataisEmpty(void)
 @Description			NET_Coap_Message_SendDataisEmpty	: 检查接收队列是否已空
 @Input				void
 @Return				true							: 已空
					false						: 未空
**********************************************************************************************************/
static bool NET_Coap_Message_RecvDataisEmpty(void)
{
	bool MessageState;
	
	if (NETCoapMessageRecvPark.Front == NETCoapMessageRecvPark.Rear) {
		MessageState = true;
	}
	else {
		MessageState = false;
	}
	
	return MessageState;
}

/**********************************************************************************************************
 @Function			void NET_Coap_Message_SendDataEnqueue(unsigned char* dataBuf, unsigned short dataLength)
 @Description			NET_Coap_Message_SendDataEnqueue	: 发送数据写入队列
 @Input				dataBuf	 		 			: 需写入数据
					dataLength					: 需写入数据长度
 @Return				void
**********************************************************************************************************/
void NET_Coap_Message_SendDataEnqueue(unsigned char* dataBuf, unsigned short dataLength)
{
	if ((dataBuf == NULL) || (dataLength > COAP_SEND_BUFFER_SIZE)) {
		return;
	}
	
	if (NET_Coap_Message_SendDataisFull() == true) {													//队列已满
		NETCoapMessageSendPark.Rear = (NETCoapMessageSendPark.Rear + 1) % COAP_SEND_PARK_NUM;				//队尾偏移1
		memset((u8 *)&NETCoapMessageSendPark.Park[NETCoapMessageSendPark.Rear], 0x0, sizeof(NETCoapMessageSendPark.Park[NETCoapMessageSendPark.Rear]));
		memcpy(NETCoapMessageSendPark.Park[NETCoapMessageSendPark.Rear].Buffer, dataBuf, dataLength);
		NETCoapMessageSendPark.Park[NETCoapMessageSendPark.Rear].Length = dataLength;
		NETCoapMessageSendPark.Front = (NETCoapMessageSendPark.Front + 1) % COAP_SEND_PARK_NUM;				//队头偏移1
	}
	else {																					//队列未满
		NETCoapMessageSendPark.Rear = (NETCoapMessageSendPark.Rear + 1) % COAP_SEND_PARK_NUM;				//队尾偏移1
		memset((u8 *)&NETCoapMessageSendPark.Park[NETCoapMessageSendPark.Rear], 0x0, sizeof(NETCoapMessageSendPark.Park[NETCoapMessageSendPark.Rear]));
		memcpy(NETCoapMessageSendPark.Park[NETCoapMessageSendPark.Rear].Buffer, dataBuf, dataLength);
		NETCoapMessageSendPark.Park[NETCoapMessageSendPark.Rear].Length = dataLength;
	}
}

/**********************************************************************************************************
 @Function			void NET_Coap_Message_RecvDataEnqueue(unsigned char* dataBuf, unsigned short dataLength)
 @Description			NET_Coap_Message_RecvDataEnqueue	: 接收数据写入队列
 @Input				dataBuf	 		 			: 需写入数据
					dataLength					: 需写入数据长度
 @Return				void
**********************************************************************************************************/
void NET_Coap_Message_RecvDataEnqueue(unsigned char* dataBuf, unsigned short dataLength)
{
	if ((dataBuf == NULL) || (dataLength > COAP_RECV_BUFFER_SIZE)) {
		return;
	}
	
	if (NET_Coap_Message_RecvDataisFull() == true) {													//队列已满
		NETCoapMessageRecvPark.Rear = (NETCoapMessageRecvPark.Rear + 1) % COAP_RECV_PARK_NUM;				//队尾偏移1
		memset((u8 *)&NETCoapMessageRecvPark.Park[NETCoapMessageRecvPark.Rear], 0x0, sizeof(NETCoapMessageRecvPark.Park[NETCoapMessageRecvPark.Rear]));
		memcpy(NETCoapMessageRecvPark.Park[NETCoapMessageRecvPark.Rear].Buffer, dataBuf, dataLength);
		NETCoapMessageRecvPark.Park[NETCoapMessageRecvPark.Rear].Length = dataLength;
		NETCoapMessageRecvPark.Front = (NETCoapMessageRecvPark.Front + 1) % COAP_RECV_PARK_NUM;				//队头偏移1
	}
	else {																					//队列未满
		NETCoapMessageRecvPark.Rear = (NETCoapMessageRecvPark.Rear + 1) % COAP_RECV_PARK_NUM;				//队尾偏移1
		memset((u8 *)&NETCoapMessageRecvPark.Park[NETCoapMessageRecvPark.Rear], 0x0, sizeof(NETCoapMessageRecvPark.Park[NETCoapMessageRecvPark.Rear]));
		memcpy(NETCoapMessageRecvPark.Park[NETCoapMessageRecvPark.Rear].Buffer, dataBuf, dataLength);
		NETCoapMessageRecvPark.Park[NETCoapMessageRecvPark.Rear].Length = dataLength;
	}
}

/**********************************************************************************************************
 @Function			bool NET_Coap_Message_SendDataDequeue(unsigned char* dataBuf, unsigned short* dataLength)
 @Description			NET_Coap_Message_SendDataDequeue	: 发送数据读出队列
 @Input				dataBuf	 		 			: 需读出数据地址
					dataLength					: 需读出数据长度地址
 @Return				true							: 未空
					false						: 已空
**********************************************************************************************************/
bool NET_Coap_Message_SendDataDequeue(unsigned char* dataBuf, unsigned short* dataLength)
{
	bool MessageState;
	unsigned char front;
	
	if (NET_Coap_Message_SendDataisEmpty() == true) {													//队列已空
		MessageState = false;
	}
	else {																					//队列未空
		front = (NETCoapMessageSendPark.Front + 1) % COAP_SEND_PARK_NUM;									//队头偏移1
		memcpy(dataBuf, NETCoapMessageSendPark.Park[front].Buffer, NETCoapMessageSendPark.Park[front].Length);
		*dataLength = NETCoapMessageSendPark.Park[front].Length;
		MessageState = true;
	}
	
	return MessageState;
}

/**********************************************************************************************************
 @Function			bool NET_Coap_Message_RecvDataDequeue(unsigned char* dataBuf, unsigned short* dataLength)
 @Description			NET_Coap_Message_RecvDataDequeue	: 接收数据读出队列
 @Input				dataBuf	 		 			: 需读出数据地址
					dataLength					: 需读出数据长度地址
 @Return				true							: 未空
					false						: 已空
**********************************************************************************************************/
bool NET_Coap_Message_RecvDataDequeue(unsigned char* dataBuf, unsigned short* dataLength)
{
	bool MessageState;
	unsigned char front;
	
	if (NET_Coap_Message_RecvDataisEmpty() == true) {													//队列已空
		MessageState = false;
	}
	else {																					//队列未空
		front = (NETCoapMessageRecvPark.Front + 1) % COAP_RECV_PARK_NUM;									//队头偏移1
		memcpy(dataBuf, NETCoapMessageRecvPark.Park[front].Buffer, NETCoapMessageRecvPark.Park[front].Length);
		*dataLength = NETCoapMessageRecvPark.Park[front].Length;
		MessageState = true;
	}
	
	return MessageState;
}

/**********************************************************************************************************
 @Function			bool NET_Coap_Message_SendDataOffSet(void)
 @Description			NET_Coap_Message_SendDataOffSet	: 发送数据队列(队列头偏移1)
 @Input				void
 @Return				true							: 未空
					false						: 已空
**********************************************************************************************************/
bool NET_Coap_Message_SendDataOffSet(void)
{
	bool MessageState;
	
	if (NET_Coap_Message_SendDataisEmpty() == true) {													//队列已空
		MessageState = false;
	}
	else {																					//队列未空
		NETCoapMessageSendPark.Front = (NETCoapMessageSendPark.Front + 1) % COAP_SEND_PARK_NUM;
		MessageState = true;
	}
	
	return MessageState;
}

/**********************************************************************************************************
 @Function			bool NET_Coap_Message_RecvDataOffSet(void)
 @Description			NET_Coap_Message_RecvDataOffSet	: 接收数据队列(队列头偏移1)
 @Input				void
 @Return				true							: 未空
					false						: 已空
**********************************************************************************************************/
bool NET_Coap_Message_RecvDataOffSet(void)
{
	bool MessageState;
	
	if (NET_Coap_Message_RecvDataisEmpty() == true) {													//队列已空
		MessageState = false;
	}
	else {																					//队列未空
		NETCoapMessageRecvPark.Front = (NETCoapMessageRecvPark.Front + 1) % COAP_RECV_PARK_NUM;
		MessageState = true;
	}
	
	return MessageState;
}

/**********************************************************************************************************
 @Function			unsigned char NET_Coap_Message_SendDataRear(void)
 @Description			NET_Coap_Message_SendDataRear		: 发送数据队尾值
 @Input				void
 @Return				发送数据队尾值
**********************************************************************************************************/
unsigned char NET_Coap_Message_SendDataRear(void)
{
	return NETCoapMessageSendPark.Rear;
}

/**********************************************************************************************************
 @Function			unsigned char NET_Coap_Message_RecvDataRear(void)
 @Description			NET_Coap_Message_SendDataRear		: 接收数据队尾值
 @Input				void
 @Return				接收数据队尾值
**********************************************************************************************************/
unsigned char NET_Coap_Message_RecvDataRear(void)
{
	return NETCoapMessageRecvPark.Rear;
}












/********************************************** END OF FLEE **********************************************/
