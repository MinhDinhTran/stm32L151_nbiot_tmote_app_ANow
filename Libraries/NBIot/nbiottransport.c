/**
  *********************************************************************************************************
  * @file    nbiottransport.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2018-03-03
  * @brief   
  *********************************************************************************************************
  * @attention
  *
  *
  *
  *********************************************************************************************************
  */

#include "nbiottransport.h"
#include "usart.h"

unsigned char NBIOT_ATSendBuf[NBIOT_ATBUFFER_SIZE];
unsigned char NBIOT_ATRecvBuf[NBIOT_ATBUFFER_SIZE];

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Transport_SendATCmd(NBIOT_ATCmdTypeDef* ATCmd)
 @Description			NBIOT_Transport_SendATCmd 				: ����ATָ��ȴ�Ӧ��
 @Input				ATCmd		 						: ATָ��ṹ��
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Transport_SendATCmd(NBIOT_ATCmdTypeDef* ATCmd)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	char *str = NULL;
	u16 ReceiveLength = 0;
	
	if (ATCmd->ATSendlen > ATCmd->ATSendbuf_size) {														//�������ݳ��ȼ��
		NBStatus = NBIOT_ERROR;
		goto exit;
	}
	
	Uart1_PortSerialEnable(ENABLE, DISABLE);															//���������ж�
	USART1_RX_STA = 0;																				//����״̬��λ
	memset((void *)USART1_RX_BUF, 0x0, sizeof(USART1_RX_BUF));												//��ջ���ռ�
	
	if (HAL_OK != HAL_UART_Transmit(&UART1_Handler, ATCmd->ATSendbuf, ATCmd->ATSendlen, 0xFFFF)) {					//��������
		NBStatus = NBIOT_ERROR;
		Uart1_PortSerialEnable(DISABLE, DISABLE);
		goto exit;
	}
	
	if ((ATCmd->ATack && ATCmd->CmdWaitTime.xTicksToWait) || (ATCmd->ATNack && ATCmd->CmdWaitTime.xTicksToWait)) {	//��Ҫ�ȴ�Ӧ��
		while (Stm32_Calculagraph_IsExpiredMS(&ATCmd->CmdWaitTime) != true) {									//�ȴ�����ʱ
			if (USART1_RX_STA & 0x8000) {																//���յ��ڴ���Ӧ����
				USART1_RX_BUF[USART1_RX_STA & 0x3FFF] = 0;												//��ӽ�����
				if (ATCmd->ATack && ((str = strstr((const char*)USART1_RX_BUF, (const char*)ATCmd->ATack)) != NULL)) {			//Found! OK
					if (ATCmd->ATRecvbuf) {															//��ȡӦ������
						if ((ReceiveLength + (USART1_RX_STA & 0x3FFF)) >= ATCmd->ATRecvbuf_size) {				//����AT���ݳ��ȼ��
							NBStatus = NBIOT_ERROR;
							goto exit;
						}
						strncpy((char *)ATCmd->ATRecvbuf + ReceiveLength, str, strlen(str));
						ATCmd->ATRecvbuf[strlen(str) + ReceiveLength] = '\0';
						ReceiveLength += USART1_RX_STA & 0x3FFF;
						ATCmd->ATRecvlen = ReceiveLength;
					}
					NBStatus = NBIOT_OK;
					break;
				}
				else if (ATCmd->ATNack && ((str = strstr((const char*)USART1_RX_BUF, (const char*)ATCmd->ATNack)) != NULL)) {		//Found! Err
					if (ATCmd->ATRecvbuf) {															//��ȡӦ������
						if ((ReceiveLength + (USART1_RX_STA & 0x3FFF)) >= ATCmd->ATRecvbuf_size) {				//����AT���ݳ��ȼ��
							NBStatus = NBIOT_ERROR;
							goto exit;
						}
						strncpy((char *)ATCmd->ATRecvbuf + ReceiveLength, str, strlen(str));
						ATCmd->ATRecvbuf[strlen(str) + ReceiveLength] = '\0';
						ReceiveLength += USART1_RX_STA & 0x3FFF;
						ATCmd->ATRecvlen = ReceiveLength;
					}
					NBStatus = NBIOT_ERROR;
					break;
				}
				else if (ATCmd->ATRecvbuf) {																		//No Found!
					if ((ReceiveLength + (USART1_RX_STA & 0x3FFF)) >= ATCmd->ATRecvbuf_size) {					//����AT���ݳ��ȼ��
						NBStatus = NBIOT_ERROR;
						goto exit;
					}
					strncpy((char *)ATCmd->ATRecvbuf + ReceiveLength, (char *)USART1_RX_BUF, USART1_RX_STA & 0x3FFF);
					ReceiveLength += USART1_RX_STA & 0x3FFF;
				}
				
				USART1_RX_STA = 0;																	//NotFound!
			}
		}
		
		if (Stm32_Calculagraph_IsExpiredMS(&ATCmd->CmdWaitTime) == true) {
			ATCmd->ATRecvlen = 0;
			NBStatus = NBIOT_CMD_TIMEOUT;
		}
	}
	
exit:
	Uart1_PortSerialEnable(DISABLE, DISABLE);
	
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Transport_RecvATCmd(NBIOT_ATCmdTypeDef* ATCmd)
 @Description			NBIOT_Transport_RecvATCmd 				: ����ATָ��Ӧ��
 @Input				ATCmd		 						: ATָ��ṹ��
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Transport_RecvATCmd(NBIOT_ATCmdTypeDef* ATCmd)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	char *str = NULL;
	u16 ReceiveLength = 0;
	
	Uart1_PortSerialEnable(ENABLE, DISABLE);															//���������ж�
	USART1_RX_STA = 0;																				//����״̬��λ
	memset((void *)USART1_RX_BUF, 0x0, sizeof(USART1_RX_BUF));												//��ջ���ռ�
	
	if ((ATCmd->ATack && ATCmd->CmdWaitTime.xTicksToWait) || (ATCmd->ATNack && ATCmd->CmdWaitTime.xTicksToWait)) {	//��Ҫ�ȴ�Ӧ��
		while (Stm32_Calculagraph_IsExpiredMS(&ATCmd->CmdWaitTime) != true) {									//�ȴ�����ʱ
			if (USART1_RX_STA & 0x8000) {																//���յ��ڴ���Ӧ����
				USART1_RX_BUF[USART1_RX_STA & 0x3FFF] = 0;												//��ӽ�����
				if (ATCmd->ATack && ((str = strstr((const char*)USART1_RX_BUF, (const char*)ATCmd->ATack)) != NULL)) {			//Found! OK
					if (ATCmd->ATRecvbuf) {															//��ȡӦ������
						if ((ReceiveLength + (USART1_RX_STA & 0x3FFF)) >= ATCmd->ATRecvbuf_size) {				//����AT���ݳ��ȼ��
							NBStatus = NBIOT_ERROR;
							goto exit;
						}
						strncpy((char *)ATCmd->ATRecvbuf + ReceiveLength, str, strlen(str));
						ATCmd->ATRecvbuf[strlen(str) + ReceiveLength] = '\0';
						ReceiveLength += USART1_RX_STA & 0x3FFF;
						ATCmd->ATRecvlen = ReceiveLength;
					}
					NBStatus = NBIOT_OK;
					break;
				}
				else if (ATCmd->ATNack && ((str = strstr((const char*)USART1_RX_BUF, (const char*)ATCmd->ATNack)) != NULL)) {		//Found! Err
					if (ATCmd->ATRecvbuf) {															//��ȡӦ������
						if ((ReceiveLength + (USART1_RX_STA & 0x3FFF)) >= ATCmd->ATRecvbuf_size) {				//����AT���ݳ��ȼ��
							NBStatus = NBIOT_ERROR;
							goto exit;
						}
						strncpy((char *)ATCmd->ATRecvbuf + ReceiveLength, str, strlen(str));
						ATCmd->ATRecvbuf[strlen(str) + ReceiveLength] = '\0';
						ReceiveLength += USART1_RX_STA & 0x3FFF;
						ATCmd->ATRecvlen = ReceiveLength;
					}
					NBStatus = NBIOT_ERROR;
					break;
				}
				else if (ATCmd->ATRecvbuf) {																		//No Found!
					if ((ReceiveLength + (USART1_RX_STA & 0x3FFF)) >= ATCmd->ATRecvbuf_size) {					//����AT���ݳ��ȼ��
						NBStatus = NBIOT_ERROR;
						goto exit;
					}
					strncpy((char *)ATCmd->ATRecvbuf + ReceiveLength, (char *)USART1_RX_BUF, USART1_RX_STA & 0x3FFF);
					ReceiveLength += USART1_RX_STA & 0x3FFF;
				}
				
				USART1_RX_STA = 0;																	//NotFound!
			}
		}
		
		if (Stm32_Calculagraph_IsExpiredMS(&ATCmd->CmdWaitTime) == true) {
			ATCmd->ATRecvlen = 0;
			NBStatus = NBIOT_CMD_TIMEOUT;
		}
	}
	
exit:
	Uart1_PortSerialEnable(DISABLE, DISABLE);
	
	return NBStatus;
}

/**********************************************************************************************************
 @Function			NBIOT_StatusTypeDef NBIOT_Transport_Init(NBIOT_ATCmdTypeDef* ATCmd)
 @Description			NBIOT_Transport_Init					: Initiative��ʼ��NBIOT���ݴ���ӿ�
 @Input				TCmd		 							: ATָ��ṹ��
 @Return				NBIOT_StatusTypeDef						: NBIOT����״̬
**********************************************************************************************************/
NBIOT_StatusTypeDef NBIOT_Transport_Init(NBIOT_ATCmdTypeDef* ATCmd)
{
	NBIOT_StatusTypeDef NBStatus = NBIOT_OK;
	
	ATCmd->ATSendbuf					= NBIOT_ATSendBuf;
	ATCmd->ATRecvbuf					= NBIOT_ATRecvBuf;
	ATCmd->ATSendbuf_size				= sizeof(NBIOT_ATSendBuf);
	ATCmd->ATRecvbuf_size				= sizeof(NBIOT_ATRecvBuf);
	
	ATCmd->ATSendlen					= 0;
	ATCmd->ATRecvlen					= 0;
	
	ATCmd->Write						= NBIOT_Transport_SendATCmd;
	ATCmd->Read						= NBIOT_Transport_RecvATCmd;
	
	return NBStatus;
}

/********************************************** END OF FLEE **********************************************/
