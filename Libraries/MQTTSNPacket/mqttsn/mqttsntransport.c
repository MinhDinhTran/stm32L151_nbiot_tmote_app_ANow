/**
  *********************************************************************************************************
  * @file    mqttsntransport.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2018-02-06
  * @brief   
  *********************************************************************************************************
  * @attention
  *
  *
  *
  *********************************************************************************************************
  */

#include "mqttsntransport.h"
#include "nbiotfunc.h"
#include "string.h"

/**********************************************************************************************************
 @Function			MQTTSN_StatusTypeDef MQTTSN_Transport_Open(MQTTSN_SocketNetTypeDef* NetSock)
 @Description			MQTTSN_Transport_Open	: ����һ��Socket����
 @Input				NetSock				: SocketNet Struct*
 @Return				MQTTSN_StatusTypeDef	: MQTTSN����״̬
**********************************************************************************************************/
MQTTSN_StatusTypeDef MQTTSN_Transport_Open(MQTTSN_SocketNetTypeDef* NetSock)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_ERROR;
	MQTTSN_StatusTypeDef MQTTSNStatus = MQTTSN_OK;
	int socketHandler = 0;
	
	NBStatus = NBIOT_Neul_NBxx_CreateUDPSocket(NetSock->NBIotStack, NetSock->LocalPort, 1, &socketHandler);
	if (NBStatus != NBIOT_OK) {
		NetSock->SocketHandler = -1;
		MQTTSNStatus = MQTTSN_ERROR;
	}
	else {
		NetSock->SocketHandler = socketHandler;
		MQTTSNStatus = MQTTSN_OK;
	}
	
	return MQTTSNStatus;
}

/**********************************************************************************************************
 @Function			MQTTSN_StatusTypeDef MQTTSN_Transport_Close(MQTTSN_SocketNetTypeDef* NetSock)
 @Description			MQTTSN_Transport_Close	: �ر�һ��Socket����
 @Input				NetSock				: SocketNet Struct*
 @Return				MQTTSN_StatusTypeDef	: MQTTSN����״̬
**********************************************************************************************************/
MQTTSN_StatusTypeDef MQTTSN_Transport_Close(MQTTSN_SocketNetTypeDef* NetSock)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_ERROR;
	MQTTSN_StatusTypeDef MQTTSNStatus = MQTTSN_OK;
	
	NBStatus = NBIOT_Neul_NBxx_CloseUDPSocket(NetSock->NBIotStack, NetSock->SocketHandler);
	if (NBStatus != NBIOT_OK) {
		MQTTSNStatus = MQTTSN_ERROR;
	}
	else {
		NetSock->SocketHandler = -1;
		MQTTSNStatus = MQTTSN_OK;
	}
	
	return MQTTSNStatus;
}

/**********************************************************************************************************
 @Function			MQTTSN_StatusTypeDef MQTTSN_Transport_Write(MQTTSN_SocketNetTypeDef* NetSock, const char *buf, u16 sendlen)
 @Description			MQTTSN_Transport_Write	: Socket���ӷ���һ����������
 @Input				NetSock				: SocketNet Struct*
					buf					: ��������
					sendlen 				: ���ݳ���
 @Return				MQTTSN_StatusTypeDef	: MQTTSN����״̬
**********************************************************************************************************/
MQTTSN_StatusTypeDef MQTTSN_Transport_Write(MQTTSN_SocketNetTypeDef* NetSock, const char *buf, u16 sendlen)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_ERROR;
	MQTTSN_StatusTypeDef MQTTSNStatus = MQTTSN_OK;
	
	if (NetSock->SocketHandler < 0) {
		MQTTSNStatus = MQTTSN_ERROR;
		goto exit;
	}
	
	memcpy(NetSock->NBIotStack->Sendbuf, buf, sendlen);
	NetSock->NBIotStack->Sendlen = sendlen;
	NBStatus = NBIOT_Neul_NBxx_SendUDPPayload(NetSock->NBIotStack, NetSock->SocketHandler, (char *)NetSock->ServerHost, NetSock->ServerPort);
	if (NBStatus != NBIOT_OK) {
		MQTTSNStatus = MQTTSN_ERROR;
	}
	else {
		MQTTSNStatus = MQTTSN_OK;
	}
	
exit:
	return MQTTSNStatus;
}

/**********************************************************************************************************
 @Function			MQTTSN_StatusTypeDef MQTTSN_Transport_Read(MQTTSN_SocketNetTypeDef* NetSock, char *buf, u16 maxrlen, int *rlen, int *rleft)
 @Description			MQTTSN_Transport_Read	: Socket���Ӷ�ȡһ����������
 @Input				NetSock				: SocketNet Struct*
					buf					: ��ȡ���ݻ���
					maxrlen				: ����ȡ���ݴ�С
					rlen					: ��ȡ���ݳ��ȵ�ַ
					rleft				: ʣ���ȡ���ݳ��ȵ�ַ
 @Return				MQTTSN_StatusTypeDef	: MQTTSN����״̬
**********************************************************************************************************/
MQTTSN_StatusTypeDef MQTTSN_Transport_Read(MQTTSN_SocketNetTypeDef* NetSock, char *buf, u16 maxrlen, int *rlen, int *rleft)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_ERROR;
	MQTTSN_StatusTypeDef MQTTSNStatus = MQTTSN_OK;
	char addr[16];
	u16 port = 0;
	
	if (NetSock->SocketHandler < 0) {
		MQTTSNStatus = MQTTSN_ERROR;
		goto exit;
	}
	
	memset((void *)addr, 0x0, sizeof(addr));
	NBStatus = NBIOT_Neul_NBxx_ReadUDPPayload(NetSock->NBIotStack, NetSock->SocketHandler, maxrlen, rleft, addr, &port);
	if (NBStatus != NBIOT_OK) {
		MQTTSNStatus = MQTTSN_ERROR;
	}
	else {
		if ((strncmp(addr, (char *)NetSock->ServerHost, 16) == 0) && (port == NetSock->ServerPort)) {
			memcpy(buf, NetSock->NBIotStack->Recvbuf, NetSock->NBIotStack->Recvlen);
			*rlen = NetSock->NBIotStack->Recvlen;
			MQTTSNStatus = MQTTSN_OK;
		}
		else {
			MQTTSNStatus = MQTTSN_ERROR;
		}
	}
	
exit:
	return MQTTSNStatus;
}

/**********************************************************************************************************
 @Function			MQTTSN_StatusTypeDef MQTTSN_Transport_Init(MQTTSN_SocketNetTypeDef* NetSock, NBIOT_ClientsTypeDef* NBIotStack, u16 Localport, const char *Serverhost, u16 Serverport)
 @Description			MQTTSN_Transport_Init	: Initiative��ʼ��MqttSN���ݴ���ӿ�
 @Input				NetSock				: SocketNet Struct*
					NBIotStack			: NBIotЭ��ջ
					Localport				: ���ض˿�
					Serverhost			: ��������ַ
					Serverport			: �������˿�
 @Return				MQTTSN_StatusTypeDef	: MQTTSN����״̬
**********************************************************************************************************/
MQTTSN_StatusTypeDef MQTTSN_Transport_Init(MQTTSN_SocketNetTypeDef* NetSock, NBIOT_ClientsTypeDef* NBIotStack, u16 Localport, const char *Serverhost, u16 Serverport)
{
	MQTTSN_StatusTypeDef MQTTSNStatus = MQTTSN_OK;
	
	NetSock->SocketHandler = -1;
	NetSock->LocalPort = Localport;
	memcpy(NetSock->ServerHost, Serverhost, strlen(Serverhost));
	NetSock->ServerPort = Serverport;
	
	NetSock->NBIotStack = NBIotStack;
	NetSock->Open = MQTTSN_Transport_Open;
	NetSock->Close = MQTTSN_Transport_Close;
	NetSock->Write = MQTTSN_Transport_Write;
	NetSock->Read = MQTTSN_Transport_Read;
	
	return MQTTSNStatus;
}

/********************************************** END OF FLEE **********************************************/
