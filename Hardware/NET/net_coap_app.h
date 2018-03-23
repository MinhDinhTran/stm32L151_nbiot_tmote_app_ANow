#ifndef __NET_COAP_APP_H
#define   __NET_COAP_APP_H

#include "sys.h"
#include "nbiotconfig.h"
#include "nbiotfunc.h"
#include "nbiottransport.h"
#include "net_coap_message_operate.h"

#define COAPCDPADDR						"117.60.157.137"					//�������� : "117.60.157.137", ���Ų��� : "180.101.147.115"
#define COAPCDPPORT						5683

void NET_COAP_APP_PollExecution(NBIOT_ClientsTypeDef* pClient);					//COAP�߼�����
void NET_COAP_NBIOT_Event_StopMode(NBIOT_ClientsTypeDef* pClient);				//ֹͣģʽ
void NET_COAP_NBIOT_Event_HardwareReboot(NBIOT_ClientsTypeDef* pClient);			//Ӳ������
void NET_COAP_NBIOT_Event_ModuleCheck(NBIOT_ClientsTypeDef* pClient);				//ģ����
void NET_COAP_NBIOT_Event_ParameterConfig(NBIOT_ClientsTypeDef* pClient);			//��������
void NET_COAP_NBIOT_Event_SimICCIDCheck(NBIOT_ClientsTypeDef* pClient);			//Sim�����
void NET_COAP_NBIOT_Event_FullFunctionality(NBIOT_ClientsTypeDef* pClient);		//��������
void NET_COAP_NBIOT_Event_MinimumFunctionality(NBIOT_ClientsTypeDef* pClient);		//��С����
void NET_COAP_NBIOT_Event_CDPServerCheck(NBIOT_ClientsTypeDef* pClient);			//CDP��������ѯ
void NET_COAP_NBIOT_Event_CDPServerConfig(NBIOT_ClientsTypeDef* pClient);			//CDP����������
void NET_COAP_NBIOT_Event_MiscEquipConfig(NBIOT_ClientsTypeDef* pClient);			//��������
void NET_COAP_NBIOT_Event_AttachCheck(NBIOT_ClientsTypeDef* pClient);				//ע�����
void NET_COAP_NBIOT_Event_AttachExecute(NBIOT_ClientsTypeDef* pClient);			//ע������
void NET_COAP_NBIOT_Event_AttachInquire(NBIOT_ClientsTypeDef* pClient);			//ע����ѯ
void NET_COAP_NBIOT_Event_PatameterCheckOut(NBIOT_ClientsTypeDef* pClient);		//�������
void NET_COAP_NBIOT_Event_SendData(NBIOT_ClientsTypeDef* pClient);				//��������
void NET_COAP_NBIOT_Event_RecvData(NBIOT_ClientsTypeDef* pClient);				//��������
void NET_COAP_NBIOT_Event_ExecutDownlinkData(NBIOT_ClientsTypeDef* pClient);		//�������ݴ���





#endif
