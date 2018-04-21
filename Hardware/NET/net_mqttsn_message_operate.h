#ifndef __NET_MQTTSN_MESSAGE_OPERATE_H
#define   __NET_MQTTSN_MESSAGE_OPERATE_H

#include "sys.h"

#define NET_MESSAGE_GET_MAGICNUM(Byte)		{Byte[0]='T';Byte[1]='C';Byte[2]='L';Byte[3]='D';}
#define MESSAGE_VERSION					'1'

typedef enum
{
	MSG_JSON							= '1',
	MSG_FIRMWARE						= '3'
}NET_Message_MsgType_TypeDef;

typedef enum
{
	ENCRYPT_NONE						= '0',
	ENCRYPT_AES128						= '1',
	ENCRYPT_GZIP						= '2'
}NET_Message_EncryptMode_TypeDef;

typedef enum
{
	LOGIN_REQ							= '0',
	LOGIN_RSP							= '1',
	LOGOUT_REQ						= '2',
	LOGOUT_RSP						= '3',
	
	KEEPALIVE_REQ						= '4',
	KEEPALIVE_RSP						= '5',
	SYSINFO_REQ						= '6',
	SYSINFO_RSP						= '7',
	CONFIG_SET						= 'S',
	CONFIG_SET_RSP						= 's',
	CONFIG_GET						= 'G',
	CONFIG_GET_RSP						= 'g',
	
	MODIFYUSER_REQ						= 'c',
	MODIFYUSER_RSP						= 'd',
	MODIFYPASSWORD_REQ					= 'e',
	MODIFYPASSWORD_RSP					= 'f',
	
	CLOUD_VERSION_REQ					= 'g',
	CLOUD_VERSION_RSP					= 'i',
	CLOUD_UPGRADE_START_REQ				= 'j',
	CLOUD_UPGRADE_START_RSP				= 'k',
	CLOUD_UPGRADE_STOP_REQ				= 'l',
	CLOUD_UPGRADE_STOP_RSP				= 'm',
	CLOUD_UPGRADE_PROGRESS				= 'n',
	CLOUD_UPGRADE_DATA					= 'U',
	CLOUD_UPGRADE_DATA_RSP				= 'u',
	
	MSGID_POST						= 'S',
	MSGID_PUT							= 'P'
}NET_Message_CtrlMsg_TypeDef;

typedef __packed struct
{
	unsigned char						MagicNum[4];
	unsigned char						MsgType;
	unsigned char						Version;
	unsigned char						EncryptMode;
	unsigned char						MsgId;
}NET_Message_TcldHeadTypeDef;

typedef __packed struct
{
	NET_Message_TcldHeadTypeDef			MsgHead;
	char								pData[1];
}NET_Message_TcldMsgTypeDef;

#define MQTTSN_STATUS_BASIC_PARK_NUM		3
#define MQTTSN_STATUS_EXTEND_PARK_NUM		3

/* MqttSN Status Basic */
typedef __packed struct
{
	unsigned int						DeviceSN;
	unsigned char						Status;
	unsigned int						Count;
	unsigned int						DateTime;
}MQTTSN_StatusBasicTypeDef;

typedef struct
{
	unsigned char						Front;
	unsigned char						Rear;
	MQTTSN_StatusBasicTypeDef			StatusBasic[MQTTSN_STATUS_BASIC_PARK_NUM];
}MQTTSN_SwapStatusBasicTypeDef;

/* MqttSN Status Extend */
typedef __packed struct
{
	unsigned int						DeviceSN;
	unsigned char						Status;
	unsigned int						Count;
	unsigned int						DateTime;
	int								MagX;
	int								MagY;
	int								MagZ;
	int								MagDiff;
	int								Distance;
	int								Strength;
	int								CoverCount;
	int								RadarDiff;
}MQTTSN_StatusExtendTypeDef;

typedef struct
{
	unsigned char						Front;
	unsigned char						Rear;
	MQTTSN_StatusExtendTypeDef			StatusExtend[MQTTSN_STATUS_EXTEND_PARK_NUM];
}MQTTSN_SwapStatusExtendTypeDef;


int NET_Message_Operate_Creat_Json_MoteStatus_Basic(char* outBuffer);
int NET_Message_Operate_Creat_Json_MoteStatus_Extend(char* outBuffer);

bool NET_MqttSN_Message_StatusBasicisEmpty(void);
bool NET_MqttSN_Message_StatusExtendisEmpty(void);

void NET_MqttSN_Message_StatusBasicEnqueue(MQTTSN_StatusBasicTypeDef dataBuf);
void NET_MqttSN_Message_StatusExtendEnqueue(MQTTSN_StatusExtendTypeDef dataBuf);

bool NET_MqttSN_Message_StatusBasicDequeue(MQTTSN_StatusBasicTypeDef* dataBuf);
bool NET_MqttSN_Message_StatusExtendDequeue(MQTTSN_StatusExtendTypeDef* dataBuf);

bool NET_MqttSN_Message_StatusBasicOffSet(void);
bool NET_MqttSN_Message_StatusExtendOffSet(void);

unsigned char NET_MqttSN_Message_StatusBasicRear(void);
unsigned char NET_MqttSN_Message_StatusExtendRear(void);




#endif
