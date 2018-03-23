#ifndef __MQTTSN_FUNC_H
#define   __MQTTSN_FUNC_H

#include "sys.h"
#include "mqttsnconfig.h"

MQTTSN_StatusTypeDef MQTTSN_ReadPacket(MQTTSN_ClientsTypeDef* pClient, int *msgTypes);								//��ȡ���ݰ���������Ϣ����
MQTTSN_StatusTypeDef MQTTSN_RecvPacketProcess(MQTTSN_ClientsTypeDef* pClient, int *msgTypes);							//�������ݰ�����
MQTTSN_StatusTypeDef MQTTSN_WaitforRecvAck(MQTTSN_ClientsTypeDef* pClient, int msgType, Stm32_CalculagraphTypeDef* timerS);	//�ȴ����յ���ӦӦ����Ϣ����

/* Application Programming Interface */
MQTTSN_StatusTypeDef MQTTSN_Connect(MQTTSN_ClientsTypeDef* pClient, MQTTSNPacket_connectData* options);

MQTTSN_StatusTypeDef MQTTSN_DisConnect(MQTTSN_ClientsTypeDef* pClient, int duration);

MQTTSN_StatusTypeDef MQTTSN_Register(MQTTSN_ClientsTypeDef* pClient, const char* topicName, unsigned short* topicid);

MQTTSN_StatusTypeDef MQTTSN_Publish(MQTTSN_ClientsTypeDef* pClient, MQTTSN_topicid topic, MQTTSN_MessageTypeDef* message);

MQTTSN_StatusTypeDef MQTTSN_Pingreq(MQTTSN_ClientsTypeDef* pClient, MQTTSNString* clientid);

MQTTSN_StatusTypeDef MQTTSN_Subscribe(MQTTSN_ClientsTypeDef* pClient, const char* topicFilter, enum QoS qos, messageHandlerFp messageHandler);

MQTTSN_StatusTypeDef MQTTSN_RecvAck(MQTTSN_ClientsTypeDef* pClient);






#endif
