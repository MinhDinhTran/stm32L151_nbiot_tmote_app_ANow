/**
  *********************************************************************************************************
  * @file    main.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2017-10-23
  * @brief   
  *********************************************************************************************************
  * @attention
  *
  *
  *
  *********************************************************************************************************
  */

#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "stm32l1xx_config.h"
#include "platform_config.h"
#include "hal_rtc.h"
#include "hal_iwdg.h"
#include "hal_beep.h"
#include "hal_switch.h"
#include "hal_vbat.h"
#include "hal_temperature.h"
#include "hal_eeprom.h"
#include "hal_qmc5883l.h"
#include "hal_radar.h"
#include "net_coap_app.h"
#include "net_mqttsn_app.h"


/* Debug Include File */
#define NBIOTDEBUG			0
#define OTDEBUG			0
#define UARTDEBUG			0

COAP_PacketShortTypeDef		CoapShortStructure;
COAP_PacketLongTypeDef		CoapLongStructure;
COAP_PacketInfoTypeDef		CoapInfoStructure;

MQTTSN_StatusBasicTypeDef	MqttSNStatusBasicStructure;
MQTTSN_StatusExtendTypeDef	MqttSNStatusExtendStructure;
/*
	CoapShortStructure.HeadPacket.DataLen = 0x00;
	CoapShortStructure.HeadPacket.ProtocolType = 0x00;
	CoapShortStructure.HeadPacket.Reserved1 = 0x00;
	CoapShortStructure.HeadPacket.ProtocolVersion = 0x00;
	CoapShortStructure.HeadPacket.Reserved2 = 0x00;
	CoapShortStructure.HeadPacket.PacketType = 0x05;
	CoapShortStructure.HeadPacket.PacketNumber = 0x00;
	CoapShortStructure.HeadPacket.DeviceSN = 0x81010001;
	CoapShortStructure.MsgPacket.DestSN = 0x00;
	CoapShortStructure.MsgPacket.Version = 0x01;
	CoapShortStructure.MsgPacket.Type = 0x37;
	CoapLongStructure.MsgPacket.Type = 0x3A;
	CoapInfoStructure.MsgPacket.Type = 0x35;
*/

NBIOT_ATCmdTypeDef		NbiotATCmdHandler;
NBIOT_ClientsTypeDef	NbiotClientHandler;
MQTTSN_SocketNetTypeDef	MqttSNSocketNetHandler;
MQTTSN_ClientsTypeDef	MqttSNClientHandler;

RCC_RESET_FLAG_TypeDef ResetStatus;
unsigned int SendTimes = 0;
int DebugMain(void);
/* Debug Ending */


/**********************************************************************************************************
 @Function			int main(void)
 @Description			Main
 @Input				void
 @Return				int
**********************************************************************************************************/
int main(void)
{
	HAL_Init();																//HAL���ʼ��
#ifndef SYSTEMCLOCK
	#error No Define SYSTEMCLOCK!
#else
#if (SYSTEMCLOCK == 4194000)
	Stm32_MSIClock_Init(RCC_MSIRANGE_6);											//����ʱ��MSI->4.194MHz
	Delay_Init(4194);															//��ʱ��ʼ��4.194MHzϵͳʱ��
#elif (SYSTEMCLOCK == 32000000)
	Stm32_Clock_Init(RCC_PLLMUL_6, RCC_PLLDIV_3);									//����ʱ��HSI->32MHz
	Delay_Init(32000);															//��ʱ��ʼ��32MHzϵͳʱ��
#else
	#error SYSTEMCLOCK Define Error
#endif
#endif
	
#if OTDEBUG
	ResetStatus = RCC_ResetFlag_GetStatus();										//��λ��־λ
#endif
	
	IWDG_Init(IWDG_PRESCALER_256, 0x0FFF);											//���Ź���ʼ��,��Ƶϵ��256,����ֵ0x0FFF,���ʱ��28s
	RTC_Init();																//RTC��ʼ��
	
	LowPowerCtrlIO_Init();														//�͹��Ŀ���IO��ʼ��
	PowerCtrlIO_Init();															//��Դ����IO��ʼ��
	
	Uart1_Init(9600);															//����1��ʼ��
	Uart2_Init(9600);															//����2��ʼ��
	
#if OTDEBUG
	QMC5883L_Init();															//QMC��ʼ��
	
	Radar_Init();																//�״��ʼ��
	if (Radar_InitBackGround() != TRADAR_OK) {										//�״ﱳ����ʼ��
		printf("No Radar!!\n");
	}
#endif
	
	BEEP_CtrlRepeat(2, 50);														//������
	
	CoapLongStructure.HeadPacket.DataLen = 0x00;
	CoapLongStructure.HeadPacket.ProtocolType = 0x00;
	CoapLongStructure.HeadPacket.Reserved1 = 0x00;
	CoapLongStructure.HeadPacket.ProtocolVersion = 0x00;
	CoapLongStructure.HeadPacket.Reserved2 = 0x00;
	CoapLongStructure.HeadPacket.PacketType = 0x05;
	CoapLongStructure.HeadPacket.PacketNumber = 0x00;
	CoapLongStructure.HeadPacket.DeviceSN = 0x81010001;
	CoapLongStructure.MsgPacket.DestSN = 0x00;
	CoapLongStructure.MsgPacket.Version = 0x01;
	CoapLongStructure.MsgPacket.Type = 0x3A;
	CoapLongStructure.MagneticX =  100;
	CoapLongStructure.MagneticY =  100;
	CoapLongStructure.MagneticZ =  100;
	CoapLongStructure.MagneticDiff = 300;
	CoapLongStructure.RadarDistance = 5;
	CoapLongStructure.RadarStrength = 10;
	CoapLongStructure.RadarCoverCount = 15;
	CoapLongStructure.RadarDiff = 20;
	
	MqttSNStatusExtendStructure.DeviceSN = 0x83010001;
	MqttSNStatusExtendStructure.MagX = 100;
	MqttSNStatusExtendStructure.MagY = 100;
	MqttSNStatusExtendStructure.MagZ = 100;
	MqttSNStatusExtendStructure.MagDiff = 300;
	MqttSNStatusExtendStructure.Distance = 5;
	MqttSNStatusExtendStructure.Strength = 10;
	MqttSNStatusExtendStructure.CoverCount = 15;
	MqttSNStatusExtendStructure.RadarDiff = 20;

#if NBIOTDEBUG
	NBIOT_Transport_Init(&NbiotATCmdHandler);												//NBIOT���ݴ���ӿڳ�ʼ��
	NBIOT_Client_Init(&NbiotClientHandler, &NbiotATCmdHandler);									//NBIOT�ͻ��˳�ʼ��
	
	MQTTSN_Transport_Init(&MqttSNSocketNetHandler, &NbiotClientHandler, 4000, "106.14.142.169", 1884);	//MqttSN���ݴ���ӿڳ�ʼ��
//	MQTTSN_Transport_Init(&MqttSNSocketNetHadnler, &NbiotClientHandler, 4000, "120.78.66.248", 1884);	//MqttSN���ݴ���ӿڳ�ʼ��
	MQTTSN_Client_Init(&MqttSNClientHandler, &MqttSNSocketNetHandler);							//MQTTSN�ͻ��˳�ʼ��
#endif
	
	while (1) {
		
#if NBIOTDEBUG
//		NET_COAP_APP_PollExecution(&NbiotClientHandler);
		NET_MQTTSN_APP_PollExecution(&MqttSNClientHandler);
		
		Uart2_PortSerialEnable(DISABLE, DISABLE);
		if (USART2_RX_STA & 0x8000) {
			USART2_RX_STA = 0;
			HAL_UART_Transmit(&UART2_Handler, (u8 *)"\r\nOK Data Enqueue Pack!\r\n", sizeof("\r\nOK Data Enqueue Pack!\r\n"), 0xFFFF);
			BEEP_CtrlRepeat(1, 500);
			BEEP_CtrlRepeat(1, 50);
			/* COAP DATA ENQUEUE */
			CoapLongStructure.SpotStatus = CoapLongStructure.SpotStatus ? 0 : 1;
			CoapLongStructure.SpotCount += 1;
			CoapLongStructure.DateTime = RTC_GetUnixTimeToStamp();
			NET_Coap_Message_SendDataEnqueue((unsigned char *)&CoapLongStructure, sizeof(CoapLongStructure));
			/* MQTTSN DATA ENQUEUE */
			MqttSNStatusExtendStructure.Status = MqttSNStatusExtendStructure.Status ? 0 : 1;
			MqttSNStatusExtendStructure.Count += 1;
			MqttSNStatusExtendStructure.DateTime = RTC_GetUnixTimeToStamp();
			NET_MqttSN_Message_StatusExtendEnqueue(MqttSNStatusExtendStructure);
		}
		Uart2_PortSerialEnable(ENABLE, DISABLE);
		
		SendTimes++;
		if ((SendTimes % 600) == 0) {
			HAL_UART_Transmit(&UART2_Handler, (u8 *)"\r\nOK Data Enqueue Pack!\r\n", sizeof("\r\nOK Data Enqueue Pack!\r\n"), 0xFFFF);
			BEEP_CtrlRepeat(1, 500);
			BEEP_CtrlRepeat(1, 50);
			/* COAP DATA ENQUEUE */
			CoapLongStructure.SpotStatus = CoapLongStructure.SpotStatus ? 0 : 1;
			CoapLongStructure.SpotCount += 1;
			CoapLongStructure.DateTime = RTC_GetUnixTimeToStamp();
			NET_Coap_Message_SendDataEnqueue((unsigned char *)&CoapLongStructure, sizeof(CoapLongStructure));
			/* MQTTSN DATA ENQUEUE */
			MqttSNStatusExtendStructure.Status = MqttSNStatusExtendStructure.Status ? 0 : 1;
			MqttSNStatusExtendStructure.Count += 1;
			MqttSNStatusExtendStructure.DateTime = RTC_GetUnixTimeToStamp();
			NET_MqttSN_Message_StatusExtendEnqueue(MqttSNStatusExtendStructure);
		}
#endif
		
#if OTDEBUG
		if (Radar_GetDataPack(10) != TRADAR_OK) {									//��ȡ�״����ݰ�
			printf("No Radar!!\n");
		}
		Radar_DataPackToDataStruct();												//���״�������ȡ���״�ṹ��
		printf("NotargetNum : %d\n", radarDataStruct.NotargetNum);
		printf("CoverNum : %d\n", radarDataStruct.CoverNum);
		printf("DismagNum : %d\n", radarDataStruct.DismagNum);
		printf("DisVal : %d\n", radarDataStruct.DisVal);
		printf("MagVal : %d\n", radarDataStruct.MagVal);
		printf("Diff : %d\n\n", radarDataStruct.Diff);
		
		printf("%s\n", radarDataPack.RADARData[0]);
		printf("%s\n", radarDataPack.RADARData[1]);
		printf("%s\n", radarDataPack.RADARData[2]);
		printf("%s\n", radarDataPack.RADARData[3]);
		printf("%s\n", radarDataPack.RADARData[4]);
		printf("%s\n", radarDataPack.RADARData[5]);
		printf("%s\n", radarDataPack.RADARData[6]);
		printf("%s\n", radarDataPack.RADARData[7]);
		printf("%s\n", radarDataPack.RADARData[8]);
		printf("%s\n", radarDataPack.RADARData[9]);
		printf("\n");
		
		QMC5883L_Mode_Selection(QMC_MODE_CONTINOUS);
		QMC5883L_ReadData();
		QMC5883L_Mode_Selection(QMC_MODE_STANDBY);
		printf("X : %d\n", Qmc5883lData.X_Now);
		printf("Y : %d\n", Qmc5883lData.Y_Now);
		printf("Z : %d\n", Qmc5883lData.Z_Now);
		printf("\n");
		
		printf("VBAT : %d\n\n", VBAT_ADC_Read(1000));
		printf("TEMPERATURE : %d\n\n", TEMPERATURE_ADC_Read(1000));
		printf("Mercury : %d\n\n", Mercury_Read());
#endif

#if UARTDEBUG
		Uart1_PortSerialEnable(DISABLE, DISABLE);
		Uart2_PortSerialEnable(DISABLE, DISABLE);
		
		if (USART1_RX_STA & 0x8000) {
			BEEP_CtrlRepeat(1, 50);
			HAL_UART_Transmit(&UART1_Handler, USART1_RX_BUF, USART1_RX_STA & 0X3FFF, 0xFFFF);
			USART1_RX_STA = 0;
		}
		if (USART2_RX_STA & 0x8000) {
			BEEP_CtrlRepeat(1, 50);
			HAL_UART_Transmit(&UART2_Handler, USART2_RX_BUF, USART2_RX_STA & 0X3FFF, 0xFFFF);
			USART2_RX_STA = 0;
		}
		
		Uart1_PortSerialEnable(ENABLE, DISABLE);
		Uart2_PortSerialEnable(ENABLE, DISABLE);
#endif
		IWDG_Feed();
		Delay_MS(1000);
	}
}

/********************************************** END OF FLEE **********************************************/
