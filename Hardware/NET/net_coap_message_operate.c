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
#include "string.h"

COAP_SwapSendDataTypeDef		NETCoapMessageSendPark;
COAP_SwapRecvDataTypeDef		NETCoapMessageRecvPark;

/**********************************************************************************************************
 @Function			static bool NET_Coap_Message_SendDataisFull(void)
 @Description			NET_Coap_Message_SendDataisFull	: ��鷢�Ͷ����Ƿ�����
 @Input				void
 @Return				true							: ����
					false						: δ��
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
 @Description			NET_Coap_Message_SendDataisFull	: �����ն����Ƿ�����
 @Input				void
 @Return				true							: ����
					false						: δ��
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
 @Description			NET_Coap_Message_SendDataisEmpty	: ��鷢�Ͷ����Ƿ��ѿ�
 @Input				void
 @Return				true							: �ѿ�
					false						: δ��
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
 @Description			NET_Coap_Message_SendDataisEmpty	: �����ն����Ƿ��ѿ�
 @Input				void
 @Return				true							: �ѿ�
					false						: δ��
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
 @Description			NET_Coap_Message_SendDataEnqueue	: ��������д�����
 @Input				dataBuf	 		 			: ��д������
					dataLength					: ��д�����ݳ���
 @Return				void
**********************************************************************************************************/
void NET_Coap_Message_SendDataEnqueue(unsigned char* dataBuf, unsigned short dataLength)
{
	if ((dataBuf == NULL) || (dataLength > COAP_SEND_BUFFER_SIZE)) {
		return;
	}
	
	if (NET_Coap_Message_SendDataisFull() == true) {													//��������
		NETCoapMessageSendPark.Rear = (NETCoapMessageSendPark.Rear + 1) % COAP_SEND_PARK_NUM;				//��βƫ��1
		memset((u8 *)&NETCoapMessageSendPark.Park[NETCoapMessageSendPark.Rear], 0x0, sizeof(NETCoapMessageSendPark.Park[NETCoapMessageSendPark.Rear]));
		memcpy(NETCoapMessageSendPark.Park[NETCoapMessageSendPark.Rear].Buffer, dataBuf, dataLength);
		NETCoapMessageSendPark.Park[NETCoapMessageSendPark.Rear].Length = dataLength;
		NETCoapMessageSendPark.Front = (NETCoapMessageSendPark.Front + 1) % COAP_SEND_PARK_NUM;				//��ͷƫ��1
	}
	else {																					//����δ��
		NETCoapMessageSendPark.Rear = (NETCoapMessageSendPark.Rear + 1) % COAP_SEND_PARK_NUM;				//��βƫ��1
		memset((u8 *)&NETCoapMessageSendPark.Park[NETCoapMessageSendPark.Rear], 0x0, sizeof(NETCoapMessageSendPark.Park[NETCoapMessageSendPark.Rear]));
		memcpy(NETCoapMessageSendPark.Park[NETCoapMessageSendPark.Rear].Buffer, dataBuf, dataLength);
		NETCoapMessageSendPark.Park[NETCoapMessageSendPark.Rear].Length = dataLength;
	}
}

/**********************************************************************************************************
 @Function			void NET_Coap_Message_RecvDataEnqueue(unsigned char* dataBuf, unsigned short dataLength)
 @Description			NET_Coap_Message_RecvDataEnqueue	: ��������д�����
 @Input				dataBuf	 		 			: ��д������
					dataLength					: ��д�����ݳ���
 @Return				void
**********************************************************************************************************/
void NET_Coap_Message_RecvDataEnqueue(unsigned char* dataBuf, unsigned short dataLength)
{
	if ((dataBuf == NULL) || (dataLength > COAP_RECV_BUFFER_SIZE)) {
		return;
	}
	
	if (NET_Coap_Message_RecvDataisFull() == true) {													//��������
		NETCoapMessageRecvPark.Rear = (NETCoapMessageRecvPark.Rear + 1) % COAP_RECV_PARK_NUM;				//��βƫ��1
		memset((u8 *)&NETCoapMessageRecvPark.Park[NETCoapMessageRecvPark.Rear], 0x0, sizeof(NETCoapMessageRecvPark.Park[NETCoapMessageRecvPark.Rear]));
		memcpy(NETCoapMessageRecvPark.Park[NETCoapMessageRecvPark.Rear].Buffer, dataBuf, dataLength);
		NETCoapMessageRecvPark.Park[NETCoapMessageRecvPark.Rear].Length = dataLength;
		NETCoapMessageRecvPark.Front = (NETCoapMessageRecvPark.Front + 1) % COAP_RECV_PARK_NUM;				//��ͷƫ��1
	}
	else {																					//����δ��
		NETCoapMessageRecvPark.Rear = (NETCoapMessageRecvPark.Rear + 1) % COAP_RECV_PARK_NUM;				//��βƫ��1
		memset((u8 *)&NETCoapMessageRecvPark.Park[NETCoapMessageRecvPark.Rear], 0x0, sizeof(NETCoapMessageRecvPark.Park[NETCoapMessageRecvPark.Rear]));
		memcpy(NETCoapMessageRecvPark.Park[NETCoapMessageRecvPark.Rear].Buffer, dataBuf, dataLength);
		NETCoapMessageRecvPark.Park[NETCoapMessageRecvPark.Rear].Length = dataLength;
	}
}

/**********************************************************************************************************
 @Function			bool NET_Coap_Message_SendDataDequeue(unsigned char* dataBuf, unsigned short* dataLength)
 @Description			NET_Coap_Message_SendDataDequeue	: �������ݶ�������
 @Input				dataBuf	 		 			: ��������ݵ�ַ
					dataLength					: ��������ݳ��ȵ�ַ
 @Return				true							: δ��
					false						: �ѿ�
**********************************************************************************************************/
bool NET_Coap_Message_SendDataDequeue(unsigned char* dataBuf, unsigned short* dataLength)
{
	bool MessageState;
	unsigned char front;
	
	if (NET_Coap_Message_SendDataisEmpty() == true) {													//�����ѿ�
		MessageState = false;
	}
	else {																					//����δ��
		front = (NETCoapMessageSendPark.Front + 1) % COAP_SEND_PARK_NUM;									//��ͷƫ��1
		memcpy(dataBuf, NETCoapMessageSendPark.Park[front].Buffer, NETCoapMessageSendPark.Park[front].Length);
		*dataLength = NETCoapMessageSendPark.Park[front].Length;
		MessageState = true;
	}
	
	return MessageState;
}

/**********************************************************************************************************
 @Function			bool NET_Coap_Message_RecvDataDequeue(unsigned char* dataBuf, unsigned short* dataLength)
 @Description			NET_Coap_Message_RecvDataDequeue	: �������ݶ�������
 @Input				dataBuf	 		 			: ��������ݵ�ַ
					dataLength					: ��������ݳ��ȵ�ַ
 @Return				true							: δ��
					false						: �ѿ�
**********************************************************************************************************/
bool NET_Coap_Message_RecvDataDequeue(unsigned char* dataBuf, unsigned short* dataLength)
{
	bool MessageState;
	unsigned char front;
	
	if (NET_Coap_Message_RecvDataisEmpty() == true) {													//�����ѿ�
		MessageState = false;
	}
	else {																					//����δ��
		front = (NETCoapMessageRecvPark.Front + 1) % COAP_RECV_PARK_NUM;									//��ͷƫ��1
		memcpy(dataBuf, NETCoapMessageRecvPark.Park[front].Buffer, NETCoapMessageRecvPark.Park[front].Length);
		*dataLength = NETCoapMessageRecvPark.Park[front].Length;
		MessageState = true;
	}
	
	return MessageState;
}

/**********************************************************************************************************
 @Function			bool NET_Coap_Message_SendDataOffSet(void)
 @Description			NET_Coap_Message_SendDataOffSet	: �������ݶ���(����ͷƫ��1)
 @Input				void
 @Return				true							: δ��
					false						: �ѿ�
**********************************************************************************************************/
bool NET_Coap_Message_SendDataOffSet(void)
{
	bool MessageState;
	
	if (NET_Coap_Message_SendDataisEmpty() == true) {													//�����ѿ�
		MessageState = false;
	}
	else {																					//����δ��
		NETCoapMessageSendPark.Front = (NETCoapMessageSendPark.Front + 1) % COAP_SEND_PARK_NUM;
		MessageState = true;
	}
	
	return MessageState;
}

/**********************************************************************************************************
 @Function			bool NET_Coap_Message_RecvDataOffSet(void)
 @Description			NET_Coap_Message_RecvDataOffSet	: �������ݶ���(����ͷƫ��1)
 @Input				void
 @Return				true							: δ��
					false						: �ѿ�
**********************************************************************************************************/
bool NET_Coap_Message_RecvDataOffSet(void)
{
	bool MessageState;
	
	if (NET_Coap_Message_RecvDataisEmpty() == true) {													//�����ѿ�
		MessageState = false;
	}
	else {																					//����δ��
		NETCoapMessageRecvPark.Front = (NETCoapMessageRecvPark.Front + 1) % COAP_RECV_PARK_NUM;
		MessageState = true;
	}
	
	return MessageState;
}

/**********************************************************************************************************
 @Function			unsigned char NET_Coap_Message_SendDataRear(void)
 @Description			NET_Coap_Message_SendDataRear		: �������ݶ�βֵ
 @Input				void
 @Return				�������ݶ�βֵ
**********************************************************************************************************/
unsigned char NET_Coap_Message_SendDataRear(void)
{
	return NETCoapMessageSendPark.Rear;
}

/**********************************************************************************************************
 @Function			unsigned char NET_Coap_Message_RecvDataRear(void)
 @Description			NET_Coap_Message_SendDataRear		: �������ݶ�βֵ
 @Input				void
 @Return				�������ݶ�βֵ
**********************************************************************************************************/
unsigned char NET_Coap_Message_RecvDataRear(void)
{
	return NETCoapMessageRecvPark.Rear;
}












/********************************************** END OF FLEE **********************************************/
