/**
  *********************************************************************************************************
  * @file    mqttsnconfig.c
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

#include "mqttsnconfig.h"
#include "mqttsnfunc.h"
#include "mqttsntransport.h"

/* Debug Include File */
#include "hal_iwdg.h"
#include "delay.h"
#include "usart.h"
/* Debug Ending */

unsigned char MQTTSN_SendBuf[MQTTSN_BUFFER_SIZE];
unsigned char MQTTSN_RecvBuf[MQTTSN_BUFFER_SIZE];
unsigned char MQTTSN_DataStack[MQTTSN_DATASTACK_SIZE];

/**********************************************************************************************************
 @Function			void MQTTSN_WaitforCallback(MQTTSN_ClientsTypeDef* pClient)
 @Description			MQTTSN_WaitforCallback	: MQTTSN���ݵȴ����ջص�����
 @Input				pClient				: MqttSN�ͻ���ʵ��
 @Return				void
 @attention			MQTTSN�ȴ���ӦӦ����Ϣ����ʱ��ϳ�,�˻ص������ṩ�ڵȴ�ʱ���к���
**********************************************************************************************************/
void MQTTSN_WaitforCallback(MQTTSN_ClientsTypeDef* pClient)
{
/* Debug */
	//Todo
	printf("Wait For Call Back\n");
	IWDG_Feed();
	Delay_MS(1000);
/* End Debug */
}

/**********************************************************************************************************
 @Function			void MQTTSN_Client_Init(MQTTSN_ClientsTypeDef* pClient, MQTTSN_SocketNetTypeDef* NetSock)
 @Description			MQTTSN_Client_Init						: ��ʼ��MQTTSN�ͻ���
 @Input				pClient								: MQTTSN�ͻ���ʵ��
					NetSock								: MQTTSN Socket Э��ջ
 @Return				void
**********************************************************************************************************/
void MQTTSN_Client_Init(MQTTSN_ClientsTypeDef* pClient, MQTTSN_SocketNetTypeDef* NetSock)
{
	pClient->Sendbuf									= MQTTSN_SendBuf;
	pClient->Recvbuf									= MQTTSN_RecvBuf;
	pClient->Sendbuf_size								= sizeof(MQTTSN_SendBuf);
	pClient->Recvbuf_size								= sizeof(MQTTSN_RecvBuf);
	pClient->Sendlen									= 0;
	pClient->Recvlen									= 0;
	pClient->DataProcessStack							= MQTTSN_DataStack;
	pClient->DataProcessStack_size						= sizeof(MQTTSN_DataStack);
	
	pClient->Command_Timeout_Sec							= MQTTSN_COMMAND_TIMEOUT_SEC;
	pClient->Command_Failure_Cnt							= MQTTSN_COMMAND_FAILURE_CNT;
	
	pClient->DictateRunCtl.dictateEnable					= false;
	pClient->DictateRunCtl.dictateTimeoutSec				= 0;
	pClient->DictateRunCtl.dictateInitFailureCnt				= 0;
	pClient->DictateRunCtl.dictateDisconnectFailureCnt		= 0;
	pClient->DictateRunCtl.dictateActiveFailureCnt			= 0;
	pClient->DictateRunCtl.dictateSleepFailureCnt			= 0;
	pClient->DictateRunCtl.dictateAweakFailureCnt			= 0;
	pClient->DictateRunCtl.dictateLostFailureCnt				= 0;
	pClient->DictateRunCtl.dictateSubscribeStatus			= false;
	
	pClient->MessageSendCtl.messageStatusBasic				= false;
	pClient->MessageSendCtl.messageStatusExtend				= false;
	
	pClient->MsgId										= 1;
	pClient->defaultMessageHandler						= NULL;
	pClient->SubState									= MQTTSN_SUBSTATE_INIT;
	pClient->SocketStack								= NetSock;
	
	for (int i = 0; i < MAX_MESSAGE_HANDLERS; i++) {
		pClient->messageHandlers[i].topicFilter				= NULL;
		pClient->messageHandlers[i].topicid				= 0;
		pClient->messageHandlers[i].fp					= NULL;
	}
}












/********************************************** END OF FLEE **********************************************/
