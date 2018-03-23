#ifndef __MQTTSN_TRANSPORT_H
#define   __MQTTSN_TRANSPORT_H

#include "sys.h"
#include "mqttsnconfig.h"

MQTTSN_StatusTypeDef MQTTSN_Transport_Open(MQTTSN_SocketNetTypeDef* NetSock);											//����һ��Socket����
MQTTSN_StatusTypeDef MQTTSN_Transport_Close(MQTTSN_SocketNetTypeDef* NetSock);											//�ر�һ��Socket����
MQTTSN_StatusTypeDef MQTTSN_Transport_Write(MQTTSN_SocketNetTypeDef* NetSock, const char *buf, u16 sendlen);					//����һ����������
MQTTSN_StatusTypeDef MQTTSN_Transport_Read(MQTTSN_SocketNetTypeDef* NetSock, char *buf, u16 maxrlen, int *rlen, int *rleft);		//��ȡһ����������

MQTTSN_StatusTypeDef MQTTSN_Transport_Init(MQTTSN_SocketNetTypeDef* , NBIOT_ClientsTypeDef* , u16 , const char *, u16 );		//��ʼ��

#endif
