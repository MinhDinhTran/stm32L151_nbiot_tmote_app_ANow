#ifndef __NBIOT_CONFIG_H
#define   __NBIOT_CONFIG_H

#include "sys.h"

#define NBIOT_RESET_GPIOx				GPIOB
#define NBIOT_RESET_PIN					GPIO_PIN_2
#define NBIOT_RESET_RCC_GPIO_CLK_ENABLE()	__HAL_RCC_GPIOB_CLK_ENABLE()
#define NBIOT_RESET_RCC_GPIO_CLK_DISABLE()	__HAL_RCC_GPIOB_CLK_DISABLE()

#define NBIOT_COMMAND_TIMEOUT_MSEC			8000
#define NBIOT_COMMAND_FAILURE_CNT			3

/* NBIOT Э��ջ���ٻ����С */
#define NBIOT_BUFFER_SIZE				256
#define NBIOT_ATBUFFER_SIZE				512
#define NBIOT_DATASTACK_SIZE				512

/* NCONFIG���ò��� */
#define AutoConnect						"AUTOCONNECT"
#define CrScrambling					"CR_0354_0338_SCRAMBLING"
#define CrSiAvoid						"CR_0859_SI_AVOID"
#define CombineAttach					"COMBINE_ATTACH"
#define CellReselection					"CELL_RESELECTION"
#define EnableBip						"ENABLE_BIP"

typedef struct NBIOT_CDPServerTypeDef		NBIOT_CDPServerTypeDef;
typedef struct NBIOT_ParameterTypeDef		NBIOT_ParameterTypeDef;
typedef struct NBIOT_ATCmdTypeDef			NBIOT_ATCmdTypeDef;
typedef struct NBIOT_ClientsTypeDef		NBIOT_ClientsTypeDef;

/* NBIOT Status */
typedef enum
{
	NBIOT_OK       					= 0x00,
	NBIOT_ERROR    					= 0x01,
	NBIOT_CMD_TIMEOUT					= 0x02
}NBIOT_StatusTypeDef;

/* NBIOT BAND */
typedef enum
{
	ChinaTelecom						= 5,
	ChinaMobile						= 8,
	ChinaUnicom						= 20
}NBIOT_BandTypeDef;

/* NBIOT Detach or Attach Netstate */
typedef enum
{
	Detach							= 0x00,
	Attach							= 0x01
}NBIOT_NetstateTypeDef;

/* NBIOT MinFunc of FullFunc Functionality */
typedef enum
{
	MinFunc							= 0x00,
	FullFunc							= 0x01
}NBIOT_FunctionalityTypeDef;

/* NBIOT Function is Open? */
typedef enum
{
	CloseFunc							= 0x00,
	OpenFunc							= 0x01
}NBIOT_OpenOrCloseFuncTypeDef;

/* NBIOT NConfig is True? */
typedef enum
{
	NConfigFalse						= 0x00,
	NConfigTrue						= 0x01
}NBIOT_NConfigTypeDef;

/* NBIOT Dictate Event */
typedef enum
{
	STOP_MODE							= 0x00,											//ֹͣģʽ
	HARDWARE_REBOOT					= 0x01,											//Ӳ������
	MODULE_CHECK						= 0x02,											//ģ����
	PARAMETER_CONFIG					= 0x03,											//��������
	ICCID_CHECK						= 0x04,											//���SIM��
	MISC_EQUIP_CONFIG					= 0x05,											//��������
	ATTACH_CHECK						= 0x06,											//ע�����
	ATTACH_EXECUTE						= 0x07,											//ע������
	ATTACH_INQUIRE						= 0x08,											//ע����ѯ
	PARAMETER_CHECKOUT					= 0x09,											//�������
	MINIMUM_FUNCTIONALITY				= 0x0A,											//��С����
	FULL_FUNCTIONALITY					= 0x0B,											//��������
	CDP_SERVER_CHECK					= 0x0C,											//CDP��������ѯ
	CDP_SERVER_CONFIG					= 0x0D,											//CDP����������
	SEND_DATA							= 0x0E,											//��������
	RECV_DATA							= 0x0F,											//��������
	EXECUT_DOWNLINK_DATA				= 0x10,											//ִ����������
	MQTTSN_PROCESS_STACK				= 0x11											//MQTTSNִ��ջ
}NBIOT_DictateEventTypeDef;

/* NBIOT CDP Server Address */
struct NBIOT_CDPServerTypeDef
{
	unsigned char						CDPServerHost[16];
	unsigned short						CDPServerPort;
};

/* NBIOT Parameter */
struct NBIOT_ParameterTypeDef
{
	char								manufacturer[10];
	char								manufacturermode[20];
	char								imei[20];
	char								imeisv[20];
	char								iccid[25];
	char								imsi[15];
	char								cgpaddr[20];
	char								cgdcont[30];
	int								rssi;
	
	struct StatisticsCELLTypeDef
	{
		int							earfcn;
		int							cellID;
		int							cell;
		int							rsrp;
		int							rsrq;
		int							cellrssi;
		int							snr;
	}statisticsCELL;
	
	struct DataTimeTypeDef
	{
		int							year;
		int							month;
		int							day;
		int							hour;
		int							min;
		int							sec;
		unsigned int					localstamp;
	}dataTime;
	
	struct COAPSendMessage
	{
		int							pending;
		int							sent;
		int							error;
	}coapSendMessage;
	
	struct COAPReadMessage
	{
		int							buffered;
		int							received;
		int							dropped;
	}coapReadMessage;
	
	struct NConfigTypeDef
	{
		NBIOT_NConfigTypeDef			autoConnect;
		NBIOT_NConfigTypeDef			crScrambling;
		NBIOT_NConfigTypeDef			crSiAvoid;
		NBIOT_NConfigTypeDef			combineAttach;
		NBIOT_NConfigTypeDef			cellReselection;
		NBIOT_NConfigTypeDef			enableBip;
	}nconfig;
	
	NBIOT_NetstateTypeDef				netstate;
	NBIOT_FunctionalityTypeDef			functionality;
	NBIOT_OpenOrCloseFuncTypeDef			nnmistate;
	NBIOT_OpenOrCloseFuncTypeDef			nsmistate;
	NBIOT_BandTypeDef					band;
	NBIOT_CDPServerTypeDef				cdpserver;
};

/* NBIOT ATCmd */
struct NBIOT_ATCmdTypeDef
{
	unsigned char*						ATSendbuf;
	unsigned char*						ATRecvbuf;
	size_t							ATSendbuf_size;
	size_t							ATRecvbuf_size;
	short							ATSendlen;
	short							ATRecvlen;
	char*							ATack;
	char*							ATNack;
	Stm32_CalculagraphTypeDef			CmdWaitTime;
	NBIOT_StatusTypeDef					(*Write)(NBIOT_ATCmdTypeDef*);
	NBIOT_StatusTypeDef 				(*Read)(NBIOT_ATCmdTypeDef*);
};

/* NBIOT Clients */
struct NBIOT_ClientsTypeDef
{
	unsigned char*						Sendbuf;
	unsigned char*						Recvbuf;
	size_t							Sendbuf_size;
	size_t							Recvbuf_size;
	short							Sendlen;
	short							Recvlen;
	unsigned char*						DataProcessStack;
	size_t							DataProcessStack_size;
	unsigned int						Command_Timeout_Msec;
	unsigned int						Command_Failure_Cnt;
	
	struct NBIOTDictateRuningCtlTypeDef
	{
		bool							dictateEnable;
		unsigned int					dictateTimeoutSec;
		unsigned char					dictateRebootFailureCnt;
		unsigned char					dictateModuleCheckFailureCnt;
		unsigned char					dictateParameterConfigFailureCnt;
		unsigned char					dictateSimICCIDCheckFailureCnt;
		unsigned char					dictateMiscEquipConfigFailureCnt;
		unsigned char					dictateAttachCheckFailureCnt;
		unsigned char					dictateAttachExecuteFailureCnt;
		unsigned char					dictateAttachInquireFailureCnt;
		unsigned char					dictatePatameterCheckOutFailureCnt;
		unsigned char					dictateFullFunctionalityFailureCnt;
		unsigned char					dictateMinimumFunctionalityFailureCnt;
		unsigned char					dictateCDPServerCheckFailureCnt;
		unsigned char					dictateCDPServerConfigFailureCnt;
		unsigned char					dictateSendDataFailureCnt;
		unsigned char					dictateRecvDataFailureCnt;
		Stm32_CalculagraphTypeDef		dictateRunTime;
		NBIOT_DictateEventTypeDef		dictateEvent;
	}DictateRunCtl;
	
	NBIOT_ParameterTypeDef				Parameter;
	NBIOT_ATCmdTypeDef*					ATCmdStack;
};

void NBIOT_Client_Init(NBIOT_ClientsTypeDef* pClient, NBIOT_ATCmdTypeDef* ATCmdStack);				//NBIOT�ͻ��˳�ʼ��









#endif
