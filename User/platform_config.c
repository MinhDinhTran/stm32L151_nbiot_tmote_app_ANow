/**
  *********************************************************************************************************
  * @file    platform_config.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2017-10-26
  * @brief   
  *********************************************************************************************************
  * @attention
  *			
  *
  *
  *********************************************************************************************************
  */

#include "platform_config.h"

__IO unsigned int SecondTickTimes = 0;										//SecondTick

/* Work Mode */
bool DeviceIdleMode			= false;										//IDLE MODE
bool DeviceActivedMode		= true;										//ACTIVED MODE

/* Coap Packet */
COAP_PacketShortTypeDef		CoapShortStructure;								//Coap Short Packet
COAP_PacketLongTypeDef		CoapLongStructure;								//Coap Long Packet
COAP_PacketInfoTypeDef		CoapInfoStructure;								//Coap Info Packet

/* MqttSN Packet */
MQTTSN_StatusBasicTypeDef	MqttSNStatusBasicStructure;						//MqttSN Basic Status Packet
MQTTSN_StatusExtendTypeDef	MqttSNStatusExtendStructure;						//MqttSN Extend Status Packet

/* NET Handler */
NBIOT_ATCmdTypeDef			NbiotATCmdHandler;								//NBIOT AT Cmd Handler
NBIOT_ClientsTypeDef		NbiotClientHandler;								//Coap Clinet Handler
MQTTSN_SocketNetTypeDef		MqttSNSocketNetHandler;							//MqttSN Net Handler
MQTTSN_ClientsTypeDef		MqttSNClientHandler;							//MqttSN Clinet Handler
























/********************************************** END OF FLEE **********************************************/
