/**
  *********************************************************************************************************
  * @file    net_nbiot_app.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2018-04-26
  * @brief   
  *********************************************************************************************************
  * @attention
  *
  *
  *
  *********************************************************************************************************
  */

#include "net_nbiot_app.h"
#include "net_coap_app.h"
#include "net_mqttsn_app.h"
#include "stm32l1xx_config.h"
#include "platform_config.h"
#include "platform_map.h"
#include "hal_rtc.h"
#include "string.h"

NETCoapNeedSendCodeTypeDef	NETCoapNeedSendCode = NETCoapNeedSendCode_initializer;
NETMqttSNNeedSendCodeTypeDef	NETMqttSNNeedSendCode = NETMqttSNNeedSendCode_initializer;

/**********************************************************************************************************
 @Function			void NET_NBIOT_Initialization(void)
 @Description			NET_NBIOT_Initialization						: NET初始化
 @Input				void
 @Return				void
**********************************************************************************************************/
void NET_NBIOT_Initialization(void)
{
	NBIOT_Transport_Init(&NbiotATCmdHandler);												//NBIOT数据传输接口初始化
	NBIOT_Client_Init(&NbiotClientHandler, &NbiotATCmdHandler);									//NBIOT客户端初始化
	
	MQTTSN_Transport_Init(&MqttSNSocketNetHandler, &NbiotClientHandler, 4000, "106.14.142.169", 1884);	//MqttSN数据传输接口初始化
	MQTTSN_Client_Init(&MqttSNClientHandler, &MqttSNSocketNetHandler);							//MQTTSN客户端初始化
}

/**********************************************************************************************************
 @Function			void NET_NBIOT_App_Task(void)
 @Description			NET_NBIOT_App_Task							: NET处理
 @Input				void
 @Return				void
**********************************************************************************************************/
void NET_NBIOT_App_Task(void)
{
#if NETPROTOCAL == NETCOAP
	u32 len = 0;
	
	/* COAP SHORT STATUS DATA ENQUEUE */
	if (NETCoapNeedSendCode.ShortStatus) {
		CoapShortStructure.HeadPacket.DeviceSN				= TCFG_EEPROM_Get_MAC_SN();
		CoapShortStructure.HeadPacket.DataLen				= 0x00;
		CoapShortStructure.HeadPacket.ProtocolType			= 0x00;
		CoapShortStructure.HeadPacket.Reserved1				= 0x00;
		CoapShortStructure.HeadPacket.ProtocolVersion		= 0x00;
		CoapShortStructure.HeadPacket.Reserved2				= 0x00;
		CoapShortStructure.HeadPacket.PacketType			= 0x05;
		CoapShortStructure.HeadPacket.PacketNumber			= 0x00;
		CoapShortStructure.MsgPacket.DestSN				= 0x00;
		CoapShortStructure.MsgPacket.Version				= 0x01;
		CoapShortStructure.MsgPacket.Type					= COAP_MSGTYPE_TYPE_SHORT_STATUS;
		CoapShortStructure.DateTime						= RTC_GetUnixTimeToStamp();
		CoapShortStructure.SpotCount						= 1;
		CoapShortStructure.SpotStatus						= 0;
		NET_Coap_Message_SendDataEnqueue((unsigned char *)&CoapShortStructure, sizeof(CoapShortStructure));
	}
	/* COAP LONG STATUS DATA ENQUEUE */
	else if (NETCoapNeedSendCode.LongStatus) {
		CoapLongStructure.HeadPacket.DeviceSN				= TCFG_EEPROM_Get_MAC_SN();
		CoapLongStructure.HeadPacket.DataLen				= 0x00;
		CoapLongStructure.HeadPacket.ProtocolType			= 0x00;
		CoapLongStructure.HeadPacket.Reserved1				= 0x00;
		CoapLongStructure.HeadPacket.ProtocolVersion			= 0x00;
		CoapLongStructure.HeadPacket.Reserved2				= 0x00;
		CoapLongStructure.HeadPacket.PacketType				= 0x05;
		CoapLongStructure.HeadPacket.PacketNumber			= 0x00;
		CoapLongStructure.MsgPacket.DestSN					= 0x00;
		CoapLongStructure.MsgPacket.Version				= 0x01;
		CoapLongStructure.MsgPacket.Type					= COAP_MSGTYPE_TYPE_LONG_STATUS;
		CoapLongStructure.DateTime						= RTC_GetUnixTimeToStamp();
		CoapLongStructure.SpotCount						= 1;
		CoapLongStructure.SpotStatus						= 0;
		CoapLongStructure.MagneticX						= 100;
		CoapLongStructure.MagneticY						= 100;
		CoapLongStructure.MagneticZ						= 100;
		CoapLongStructure.MagneticDiff					= 300;
		CoapLongStructure.RadarDistance					= 0;
		CoapLongStructure.RadarStrength					= 0;
		CoapLongStructure.RadarCoverCount					= 0;
		CoapLongStructure.RadarDiff						= 0;
		NET_Coap_Message_SendDataEnqueue((unsigned char *)&CoapLongStructure, sizeof(CoapLongStructure));
	}
	/* COAP WORK INFO DATA ENQUEUE */
	else if (NETCoapNeedSendCode.WorkInfo) {
		memset((void*)&CoapInfoStructure.InfoData, 0, sizeof(CoapInfoStructure.InfoData));
		CoapInfoStructure.HeadPacket.DeviceSN				= TCFG_EEPROM_Get_MAC_SN();
		CoapInfoStructure.HeadPacket.DataLen				= 0x00;
		CoapInfoStructure.HeadPacket.ProtocolType			= 0x00;
		CoapInfoStructure.HeadPacket.Reserved1				= 0x00;
		CoapInfoStructure.HeadPacket.ProtocolVersion			= 0x00;
		CoapInfoStructure.HeadPacket.Reserved2				= 0x00;
		CoapInfoStructure.HeadPacket.PacketType				= 0x05;
		CoapInfoStructure.HeadPacket.PacketNumber			= 0x00;
		CoapInfoStructure.MsgPacket.DestSN					= 0x00;
		CoapInfoStructure.MsgPacket.Version				= 0x01;
		CoapInfoStructure.MsgPacket.Type					= COAP_MSGTYPE_TYPE_INFO;
		len = NET_COAP_Message_Operate_Creat_Json_Work_Info((char *)&CoapInfoStructure.InfoData);
		NET_Coap_Message_SendDataEnqueue((unsigned char *)&CoapInfoStructure, sizeof(CoapInfoStructure) - sizeof(CoapInfoStructure.InfoData) + len);
		NETCoapNeedSendCode.WorkInfo = 0;
	}
	/* COAP BASIC INFO DATA ENQUEUE */
	else if (NETCoapNeedSendCode.BasicInfo) {
		memset((void*)&CoapInfoStructure.InfoData, 0, sizeof(CoapInfoStructure.InfoData));
		CoapInfoStructure.HeadPacket.DeviceSN				= TCFG_EEPROM_Get_MAC_SN();
		CoapInfoStructure.HeadPacket.DataLen				= 0x00;
		CoapInfoStructure.HeadPacket.ProtocolType			= 0x00;
		CoapInfoStructure.HeadPacket.Reserved1				= 0x00;
		CoapInfoStructure.HeadPacket.ProtocolVersion			= 0x00;
		CoapInfoStructure.HeadPacket.Reserved2				= 0x00;
		CoapInfoStructure.HeadPacket.PacketType				= 0x05;
		CoapInfoStructure.HeadPacket.PacketNumber			= 0x00;
		CoapInfoStructure.MsgPacket.DestSN					= 0x00;
		CoapInfoStructure.MsgPacket.Version				= 0x01;
		CoapInfoStructure.MsgPacket.Type					= COAP_MSGTYPE_TYPE_INFO;
		len = NET_COAP_Message_Operate_Creat_Json_Basic_Info((char *)&CoapInfoStructure.InfoData);
		NET_Coap_Message_SendDataEnqueue((unsigned char *)&CoapInfoStructure, sizeof(CoapInfoStructure) - sizeof(CoapInfoStructure.InfoData) + len);
		NETCoapNeedSendCode.BasicInfo = 0;
	}
	/* COAP DYNAMIC INFO DATA ENQUEUE */
	else if (NETCoapNeedSendCode.DynamicInfo) {
		memset((void*)&CoapInfoStructure.InfoData, 0, sizeof(CoapInfoStructure.InfoData));
		CoapInfoStructure.HeadPacket.DeviceSN				= TCFG_EEPROM_Get_MAC_SN();
		CoapInfoStructure.HeadPacket.DataLen				= 0x00;
		CoapInfoStructure.HeadPacket.ProtocolType			= 0x00;
		CoapInfoStructure.HeadPacket.Reserved1				= 0x00;
		CoapInfoStructure.HeadPacket.ProtocolVersion			= 0x00;
		CoapInfoStructure.HeadPacket.Reserved2				= 0x00;
		CoapInfoStructure.HeadPacket.PacketType				= 0x05;
		CoapInfoStructure.HeadPacket.PacketNumber			= 0x00;
		CoapInfoStructure.MsgPacket.DestSN					= 0x00;
		CoapInfoStructure.MsgPacket.Version				= 0x01;
		CoapInfoStructure.MsgPacket.Type					= COAP_MSGTYPE_TYPE_INFO;
		len = NET_COAP_Message_Operate_Creat_Json_Dynamic_Info((char *)&CoapInfoStructure.InfoData);
		NET_Coap_Message_SendDataEnqueue((unsigned char *)&CoapInfoStructure, sizeof(CoapInfoStructure) - sizeof(CoapInfoStructure.InfoData) + len);
		NETCoapNeedSendCode.DynamicInfo = 0;
	}
#elif NETPROTOCAL == NETMQTTSN
	/* MQTTSN STATUS BASIC DATA ENQUEUE */
	if (NETMqttSNNeedSendCode.StatusBasic) {
		MqttSNStatusBasicStructure.DeviceSN				= TCFG_EEPROM_Get_MAC_SN();
		MqttSNStatusBasicStructure.Status					= 0;
		MqttSNStatusBasicStructure.Count					= 1;
		MqttSNStatusBasicStructure.DateTime				= RTC_GetUnixTimeToStamp();
		NET_MqttSN_Message_StatusBasicEnqueue(MqttSNStatusBasicStructure);
	}
	/* MQTTSN STATUS EXTEND DATA ENQUEUE */
	else if (NETMqttSNNeedSendCode.StatusExtend) {
		MqttSNStatusExtendStructure.DeviceSN				= TCFG_EEPROM_Get_MAC_SN();
		MqttSNStatusExtendStructure.Status					= 0;
		MqttSNStatusExtendStructure.Count					= 1;
		MqttSNStatusExtendStructure.DateTime				= RTC_GetUnixTimeToStamp();
		MqttSNStatusExtendStructure.MagX					= 100;
		MqttSNStatusExtendStructure.MagY					= 100;
		MqttSNStatusExtendStructure.MagZ					= 100;
		MqttSNStatusExtendStructure.MagDiff				= 300;
		MqttSNStatusExtendStructure.Distance				= 0;
		MqttSNStatusExtendStructure.Strength				= 0;
		MqttSNStatusExtendStructure.CoverCount				= 0;
		MqttSNStatusExtendStructure.RadarDiff				= 0;
		NET_MqttSN_Message_StatusExtendEnqueue(MqttSNStatusExtendStructure);
	}
	/* MQTTSN INFO WORK DATA ENQUEUE */
	else if (NETMqttSNNeedSendCode.InfoWork) {
		MqttSNInfoWorkStructure.DeviceSN					= TCFG_EEPROM_Get_MAC_SN();
		MqttSNInfoWorkStructure.Sensitivity				= TCFG_EEPROM_GetSavedSensitivity();
		MqttSNInfoWorkStructure.WorkMode					= TCFG_EEPROM_GetWorkMode();
		MqttSNInfoWorkStructure.RfChannel					= TCFG_EEPROM_GetRfChannel();
		NET_MqttSN_Message_InfoWorkEnqueue(MqttSNInfoWorkStructure);
		NETMqttSNNeedSendCode.InfoWork = 0;
	}
	/* MQTTSN INFO BASIC DATA ENQUEUE */
	else if (NETMqttSNNeedSendCode.InfoBasic) {
		MqttSNInfoBasicStructure.DeviceSN					= TCFG_EEPROM_Get_MAC_SN();
		MqttSNInfoBasicStructure.DeviceType				= MVB_MODEL_TYPE;
		NET_MqttSN_Message_InfoBasicEnqueue(MqttSNInfoBasicStructure);
		NETMqttSNNeedSendCode.InfoBasic = 0;
	}
	/* MQTTSN INFO DYNAMIC DATA ENQUEUE */
	else if (NETMqttSNNeedSendCode.InfoDynamic) {
		MqttSNInfoDynamicStructure.DeviceSN				= TCFG_EEPROM_Get_MAC_SN();
		MqttSNInfoDynamicStructure.Runtime					= TCFG_Utility_Get_Run_Time();
		MqttSNInfoDynamicStructure.NBIotRssi				= TCFG_Utility_Get_Nbiot_Rssi_IntVal();
		MqttSNInfoDynamicStructure.DeviceBatt				= TCFG_Utility_Get_Device_Batt_ShortVal();
		NET_MqttSN_Message_InfoDynamicEnqueue(MqttSNInfoDynamicStructure);
		NETMqttSNNeedSendCode.InfoDynamic = 0;
	}
#endif
}



















/********************************************** END OF FLEE **********************************************/
