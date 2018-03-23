#ifndef __NET_MQTTSN_APP_H
#define   __NET_MQTTSN_APP_H

#include "sys.h"
#include "mqttsnconfig.h"
#include "mqttsnfunc.h"
#include "mqttsntransport.h"
#include "nbiotconfig.h"
#include "nbiotfunc.h"
#include "nbiottransport.h"
#include "net_mqttsn_message_operate.h"

#define MQTTSN_CLIENT_ID					"MVBKK_83010001"
#define MQTTSN_SUBSCRIBE_ID				"MVBKK_83010001"

#define MQTTSN_NBIOT_BAND				ChinaMobile

#define TOPICID_MOTESTATUS				27921							//27921 -->mvb/51/1/status
#define TOPICID_MOTEINFO					27922							//27922 -->mvb/51/1info

#define TOPICID_PARKINGDEVICEMVB_A_STATUS	28921							//28921 -->ParkingDeviceMvb/A/status
#define TOPICID_PARKINGDEVICEMVB_A_MOTEINFO	28922							//28921 -->ParkingDeviceMvb/A/moteinfo
#define TOPICID_PARKINGDEVICEMVB_A_WORKINFO	28923							//28921 -->ParkingDeviceMvb/A/workinfo
#define TOPICID_PARKINGDEVICEMVB_A_RADARDBG	28924							//28921 -->ParkingDeviceMvb/A/radardbg

#define TOPICID_TESTING_MVBKK				29921							//29921 -->/testing/mvbkk/

#define TNET_MQTTSN_ACTIVE_DURATION		100
#define TNET_MQTTSN_SLEEP_DURATION			1800
#define TNET_MQTTSN_PINGREG_DURATION		180
#define TNET_MQTTSN_LOST_DURATION			300

/* MQTTSN ObjectPacket */
typedef enum
{
	OBJECT_TYPE_TMOTES_STATUS_BASIC_PUT	= 0x00,							//BASIC		STATUS
	OBJECT_TYPE_TMOTES_STATUS_EXTEND_PUT	= 0x01							//EXTEND		STATUS
}NET_MQTTSN_ObjectPacketTypeDef;

MQTTSN_StatusTypeDef messageHandlerFunction(MQTTSN_ClientsTypeDef* pClient, MQTTSN_MessageDataTypeDef* messageHandler);			//MQTTSN���մ���
MQTTSN_StatusTypeDef NET_MQTTSN_SendPayloadPacket(MQTTSN_ClientsTypeDef* pClient, NET_MQTTSN_ObjectPacketTypeDef ObjectPacket);	//MQTTSN���͸��ذ�

void NET_MQTTSN_APP_PollExecution(MQTTSN_ClientsTypeDef* pClient);				//MqttSN�߼�����
void NET_MQTTSN_NBIOT_Event_StopMode(MQTTSN_ClientsTypeDef* pClient);				//ֹͣģʽ
void NET_MQTTSN_NBIOT_Event_HardwareReboot(MQTTSN_ClientsTypeDef* pClient);		//Ӳ������
void NET_MQTTSN_NBIOT_Event_ModuleCheck(MQTTSN_ClientsTypeDef* pClient);			//ģ����
void NET_MQTTSN_NBIOT_Event_ParameterConfig(MQTTSN_ClientsTypeDef* pClient);		//��������
void NET_MQTTSN_NBIOT_Event_SimICCIDCheck(MQTTSN_ClientsTypeDef* pClient);			//Sim�����
void NET_MQTTSN_NBIOT_Event_MiscEquipConfig(MQTTSN_ClientsTypeDef* pClient);		//��������
void NET_MQTTSN_NBIOT_Event_AttachCheck(MQTTSN_ClientsTypeDef* pClient);			//ע�����
void NET_MQTTSN_NBIOT_Event_AttachExecute(MQTTSN_ClientsTypeDef* pClient);			//ע������
void NET_MQTTSN_NBIOT_Event_AttachInquire(MQTTSN_ClientsTypeDef* pClient);			//ע����ѯ
void NET_MQTTSN_NBIOT_Event_PatameterCheckOut(MQTTSN_ClientsTypeDef* pClient);		//�������

void NET_MQTTSN_APP_ProcessExecution(MQTTSN_ClientsTypeDef* pClient);				//MqttSNЭ���߼�����
void NET_MQTTSN_Event_Init(MQTTSN_ClientsTypeDef* pClient);						//INIT
void NET_MQTTSN_Event_Disconnect(MQTTSN_ClientsTypeDef* pClient);				//DISCONNECT
void NET_MQTTSN_Event_Active(MQTTSN_ClientsTypeDef* pClient);					//ACTIVE
void NET_MQTTSN_Event_Sleep(MQTTSN_ClientsTypeDef* pClient);					//SLEEP
void NET_MQTTSN_Event_Aweak(MQTTSN_ClientsTypeDef* pClient);					//AWAKE
void NET_MQTTSN_Event_Lost(MQTTSN_ClientsTypeDef* pClient);						//LOST





#endif