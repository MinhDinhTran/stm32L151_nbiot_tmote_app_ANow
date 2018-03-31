/**
  *********************************************************************************************************
  * @file    tmesh_rf_app.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2018-03-29
  * @brief   
  *********************************************************************************************************
  * @attention
  *
  *
  *
  *********************************************************************************************************
  */

#include "radio_rf_app.h"
#include "radio_hal_rf.h"
#include "radio_msg_config.h"
#include "radio.h"
#include "tmesh_xtea.h"
#include "platform_map.h"
#include "platform_config.h"
#include "hal_beep.h"
#include "delay.h"
#include "string.h"
#include <stdarg.h>

static frameInfo_t sInFrameQ[SIZE_INFRAME_Q];

uint8_t trf_send_buf[256] = {0};
uint8_t trf_recv_buf[256] = {0};

char trf_print2buffer[256] = {0};

/**********************************************************************************************************
 @Function			void Radio_Rf_QInit(void)
 @Description			Radio_Rf_QInit
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radio_Rf_QInit(void)
{
	memset(sInFrameQ, 0, sizeof(sInFrameQ));
}

/**********************************************************************************************************
 @Function			void Radio_Rf_QPush(mrfiPacket_t* rfpacket)
 @Description			Radio_Rf_QPush
 @Input				rfpacket
 @Return				void
**********************************************************************************************************/
void Radio_Rf_QPush(mrfiPacket_t* rfpacket)
{
	int8_t i, oldest = 0, avail = -1;
	
	for (i = 0; i < SIZE_INFRAME_Q; i++) {
		if (sInFrameQ[i].fi_usage == FI_AVAILABLE) {
			avail = i;
		}
		if (sInFrameQ[i].orderStamp < sInFrameQ[oldest].orderStamp) {
			oldest = i;
		}
	}
	
	if (avail < 0) {
		avail = oldest;
	}
	
	memcpy(&sInFrameQ[avail].mrfiPkt, rfpacket, sizeof(mrfiPacket_t));
	sInFrameQ[avail].fi_usage = FI_INUSE_UNTIL_DEL;
	sInFrameQ[avail].orderStamp = sInFrameQ[oldest].orderStamp++;
}

/**********************************************************************************************************
 @Function			mrfiPacket_t* Radio_Rf_QPop(void)
 @Description			Radio_Rf_QPop
 @Input				void
 @Return				rfpacket
**********************************************************************************************************/
mrfiPacket_t* Radio_Rf_QPop(void)
{
	int8_t i;
	
	for (i = 0; i < SIZE_INFRAME_Q; i++) {
		if (sInFrameQ[i].fi_usage == FI_INUSE_UNTIL_DEL) {
			sInFrameQ[i].fi_usage = FI_AVAILABLE;
			return &sInFrameQ[i].mrfiPkt;
		}
	}
	
	return NULL;
}

/**********************************************************************************************************
 @Function			void Radio_Rf_Data_Handle_Isr(mrfiPacket_t* rfpacket)
 @Description			Radio_Rf_Data_Handle_Isr
 @Input				rfpacket
 @Return				void
**********************************************************************************************************/
void Radio_Rf_Data_Handle_Isr(mrfiPacket_t* rfpacket)
{
	Radio_Rf_QPush(rfpacket);
}

/**********************************************************************************************************
 @Function			char Radio_Rf_Receive(uint8_t *outmsg, uint8_t *len)
 @Description			Radio_Rf_Receive
 @Input				outmsg
					len
 @Return				void
**********************************************************************************************************/
char Radio_Rf_Receive(uint8_t *outmsg, uint8_t *len)
{
	mrfiPacket_t* rfpacket;
	rfpacket = Radio_Rf_QPop();
	
	if (rfpacket != NULL) {
		*len = 1 + (CFG_GET_FROM_FRAME(CFG_P_FRAME_LEN(rfpacket->frame), CFG_LEN_OS));
		memcpy(outmsg, rfpacket->frame, *len);
		
		return TRF_SUCCESS;
	}
	
	return TRF_NO_PAYLOAD;
}

/**********************************************************************************************************
 @Function			char Radio_Rf_Send(uint8_t *inmsg, uint8_t len)
 @Description			Radio_Rf_Send
 @Input				inmsg
					len
 @Return				void
**********************************************************************************************************/
char Radio_Rf_Send(uint8_t *inmsg, uint8_t len)
{
	static char error_cnt = 0;
	char rc;
	
	rc = Radio_Rf_PrepareToTx(inmsg, len);
	if (rc == TRF_ERROR) {
		error_cnt++;
		if (error_cnt > 30) {
			trf_status = TRF_ERROR;
		}
		else if ((error_cnt % 3) == 0) {
			//Todo 初始化地磁和无线
		}
	}
	else {
		error_cnt = 0;
	}
	
	return rc;
}

/**********************************************************************************************************
 @Function			char Radio_Rf_Operate_Recvmsg(uint8_t *inmsg, uint8_t len)
 @Description			Radio_Rf_Operate_Recvmsg
 @Input				inmsg
					len
 @Return				void
**********************************************************************************************************/
char Radio_Rf_Operate_Recvmsg(uint8_t *inmsg, uint8_t len)
{
	char rc = TRF_SUCCESS;
	trf_msg_s* pPayload;
	
	unsigned int mac_sn = 0;
	unsigned int uval32 = 0;
	
	mac_sn = TCFG_EEPROM_Get_MAC_SN();
	
	if (CFG_GET_FROM_FRAME(CFG_P_FRAME_PRE(inmsg), CFG_PRE_TYPE_OS) == XMESHCFG_PRT)
	{
		pPayload = (trf_msg_s*)CFG_P_FRAME_PAYLOAD(inmsg);
		if (CFG_GET_FROM_FRAME(CFG_P_FRAME_HEAD(inmsg), CFG_HEAD_TYPE_OS) == TMOTE_PLAIN_SET)
		{
			uint32_t cnt;
			uint8_t payload_len;
			
			BEEP_CtrlRepeat_Extend(1, 200, 0);
			cnt = CFG_GET_FROM_FRAME(CFG_P_FRAME_HEAD(inmsg), CFG_PKTNUM_OS);
			payload_len = CFG_GET_PAYLOAD_LEN(inmsg) + CFG_FRAME_LEN_SIZE;
			
			tmesh_securityMix(mac_sn);
			tmesh_decipher((uint8_t*)pPayload, payload_len , &cnt);
			
			if (pPayload->head.destSN != mac_sn) {
				rc = TRF_NOT_FORME;
				return rc;
			}
			
			/* 升级指令 */
			if (pPayload->head.type == TRF_MSG_UPGRADE) {
				for (int i = 0; i < 5; i++) {
					TCFG_EEPROM_SetBootMode(TCFG_ENV_BOOTMODE_TOUPDATE);
					if (TCFG_ENV_BOOTMODE_TOUPDATE == TCFG_EEPROM_GetBootMode()) {
						Radio_Trf_Default_Resp(100, TRF_MSG_UPGRADE);
						BEEP_CtrlRepeat_Extend(5, 25, 25);
						Stm32_System_Software_Reboot();
					}
				}
			}
			/* 传感器灵敏度配置指令 */
			else if (pPayload->head.type == TRF_MSG_SENSITIVITY) {
				TCFG_SystemData.Sensitivity = ((tmote_sensitivity_s*)CFG_P_FRAME_PAYLOAD(inmsg))->sensitivity;
				if ((TCFG_SystemData.Sensitivity > SENSE_LOWEST) || (TCFG_SystemData.Sensitivity < SENSE_HIGHEST)) {
					TCFG_SystemData.Sensitivity = SENSE_MIDDLE;
					TCFG_EEPROM_SetSavedSensitivity(TCFG_SystemData.Sensitivity);
					__NOP();
				}
			}
			/* 工作模式配置指令 */
			else if (pPayload->head.type == TRF_MSG_WORKMODE) {
				TCFG_SystemData.WorkMode = ((tmote_work_mode_s*)CFG_P_FRAME_PAYLOAD(inmsg))->mode;
				if ((TCFG_SystemData.WorkMode != DEBUG_WORK) && (TCFG_SystemData.WorkMode != NORMAL_WORK)) {
					TCFG_SystemData.WorkMode = NORMAL_WORK;
					TCFG_EEPROM_SetWorkMode(TCFG_SystemData.WorkMode);
					__NOP();
				}
			}
			/* 无线心跳间隔时间配置指令 */
			else if (pPayload->head.type == TRF_MSG_RFHEART_INTERVAL) {
				TCFG_SystemData.Heartinterval = ((tmote_beat_interval_s*)CFG_P_FRAME_PAYLOAD(inmsg))->seconds;
				if ((TCFG_SystemData.Heartinterval > 120) || (TCFG_SystemData.Heartinterval < 10)) {
					TCFG_SystemData.Heartinterval = HEART_INTERVAL;
					TCFG_EEPROM_SetHeartinterval(TCFG_SystemData.Heartinterval);
					__NOP();
				}
			}
			/* 初始化传感器指令 */
			else if (pPayload->head.type == TRF_MSG_INITBACKGROUND) {
				
				
			}
			/* 其他下行指令 */
			else if (pPayload->head.type == TRF_MSG_GENERAL_CMD) {
				((tmote_general_cmd_s*)CFG_P_FRAME_PAYLOAD(inmsg))->buf[15] = 0;
				/* reboot */
				if (strstr(((tmote_general_cmd_s*)CFG_P_FRAME_PAYLOAD(inmsg))->buf, "reboot")) {
					BEEP_CtrlRepeat_Extend(2, 500, 250);
					Stm32_System_Software_Reboot();
					__NOP();
				}
				/* newsn */
				else if (strstr(((tmote_general_cmd_s*)CFG_P_FRAME_PAYLOAD(inmsg))->buf, "newsn")) {
					sscanf(((tmote_general_cmd_s*)CFG_P_FRAME_PAYLOAD(inmsg))->buf, "newsn:%08x", &uval32);
					TCFG_EEPROM_Set_MAC_SN(uval32);
					__NOP();
				}
				/* ...... */
			}
		}
		else {
			if (pPayload->head.destSN != mac_sn) {
				rc = TRF_NOT_FORME;
				return rc;
			}
			
			if (CFG_GET_FROM_FRAME(CFG_P_FRAME_HEAD(inmsg), CFG_HEAD_TYPE_OS) == TMOTE_PLAIN_GET)
			{
				BEEP_CtrlRepeat_Extend(2, 50, 50);
				if (pPayload->head.type == TRF_MSG_BASICINFO) {
					__NOP();
				}
				else if (pPayload->head.type == TRF_MSG_SENSITIVITY) {
					__NOP();
				}
				else if (pPayload->head.type == TRF_MSG_WORKMODE) {
					__NOP();
				}
				else if (pPayload->head.type == TRF_MSG_INITBACKGROUND) {
					__NOP();
				}
			}
			else if (CFG_GET_FROM_FRAME(CFG_P_FRAME_HEAD(inmsg), CFG_HEAD_TYPE_OS) == TMOTE_PLAIN_ACK)
			{
				__NOP();
			}
			else {
				rc = TRF_BAD_MSGTYPE;
			}
		}
	}
	else {
		rc = TRF_BAD_PROTOCAL;
	}
	
	if (rc == TRF_SUCCESS) {
		Radio_Trf_Default_Resp(100, pPayload->head.type);
	}
	
	return rc;
}

/**********************************************************************************************************
 @Function			void Radio_Trf_App_Task(void)
 @Description			Radio_Trf_App_Task
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radio_Trf_App_Task(void)
{
	static uint32_t hearttime_pre = 0;										//心跳包计时器
	uint8_t len = 0;
	
	if (TRF_OK != Radio_Rf_get_Status()) {
		return;
	}
	
	/* 到达心跳时间 */
	if ((hearttime_pre + TCFG_SystemData.Heartinterval) < Stm32_GetSecondTick()) {
		hearttime_pre = Stm32_GetSecondTick();
		Radio_Rf_Interface_Init();
		Radio_Rf_Interrupt_Init();
		/* 发送心跳包 */
		Radio_Trf_Xmit_Heartbeat();
		/* 发送调试信息 */
		if (DEBUG_WORK == Radio_Trf_Get_Workmode()) {
			
		}
	}
	
	/* 接收无线下行数据 */
	if (TRF_SUCCESS == Radio_Rf_Receive(trf_recv_buf, &len)) {
		if (TRF_SUCCESS == Radio_Rf_Operate_Recvmsg(trf_recv_buf, len)) {
			
		}
	}
}

/**********************************************************************************************************
 @Function			void Radio_Trf_Cfg_Buildframe(uint8_t *inmsg, uint8_t pkttype, uint8_t pktnum, uint32_t sn, uint8_t *outsend, uint8_t len)
 @Description			Radio_Trf_Cfg_Buildframe
 @Input				...
 @Return				void
**********************************************************************************************************/
void Radio_Trf_Cfg_Buildframe(uint8_t *inmsg, uint8_t pkttype, uint8_t pktnum, uint32_t sn, uint8_t *outsend, uint8_t len)
{
	uint8_t i;
	uint16_t check_sum = 0;
	
	CFG_PUT_INTO_FRAME(CFG_P_FRAME_LEN(outsend), CFG_LEN_OS, CFG_FRAME_PAYLOAD_OFS + CFG_FRAME_CRC_SIZE + len - CFG_FRAME_LEN_SIZE);
	CFG_PUT_INTO_FRAME(CFG_P_FRAME_PRE(outsend), CFG_PRE_TYPE_OS, XMESHCFG_PRT);
	CFG_PUT_INTO_FRAME(CFG_P_FRAME_PRE(outsend), CFG_PRE_VS_OS, CONFIG_VERSION);
	CFG_PUT_INTO_FRAME(CFG_P_FRAME_HEAD(outsend), CFG_HEAD_TYPE_OS, pkttype);
	
	if (pktnum) {
		CFG_PUT_INTO_FRAME(CFG_P_FRAME_HEAD(outsend), CFG_PKTNUM_OS, pktnum);
	}
	
	if (len) {
		memcpy(CFG_P_FRAME_PAYLOAD(outsend), inmsg, len);
	}
	
	memcpy(CFG_P_FRAME_SN(outsend), &sn, 4);
	
	for (i = 1; i < CFG_FRAME_PAYLOAD_OFS + len; i++) {
		check_sum += outsend[i];
	}
	
	outsend[CFG_FRAME_PAYLOAD_OFS + len] = check_sum / 0x100;
	outsend[CFG_FRAME_PAYLOAD_OFS + len + 1] = check_sum % 0x100;
	
	return;
}

/**********************************************************************************************************
 @Function			uint8_t Radio_Trf_Xmit_Get_Pktnum(void)
 @Description			Radio_Trf_Xmit_Get_Pktnum
 @Input				void
 @Return				pktnum
**********************************************************************************************************/
uint8_t Radio_Trf_Xmit_Get_Pktnum(void)
{
	static uint8_t pktnum = 0;
	return pktnum++;
}

/**********************************************************************************************************
 @Function			void Radio_Trf_Default_Resp(uint8_t ret, uint8_t type)
 @Description			Radio_Trf_Default_Resp
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radio_Trf_Default_Resp(uint8_t ret, uint8_t type)
{
	trf_defaultrsp_s *pDefaultRsp = (trf_defaultrsp_s*)(trf_send_buf + 32);
	pDefaultRsp->head.destSN		= 0xFFFFFFFF;
	pDefaultRsp->head.version	= TRF_MSG_VERSION;
	pDefaultRsp->head.type		= type;
	pDefaultRsp->ret			= ret;
	
	Radio_Trf_Cfg_Buildframe((uint8_t *)pDefaultRsp, TMOTE_PLAIN_RSP, Radio_Trf_Xmit_Get_Pktnum(), TCFG_EEPROM_Get_MAC_SN(), trf_send_buf, sizeof(trf_defaultrsp_s));
	Radio_Rf_Send(trf_send_buf, trf_send_buf[0]);
}

/**********************************************************************************************************
 @Function			uint8_t Radio_Trf_Get_Workmode(void)
 @Description			Radio_Trf_Get_Workmode
 @Input				void
 @Return				workmode
**********************************************************************************************************/
uint8_t Radio_Trf_Get_Workmode(void)
{
	if (DeviceIdleMode == true) {
		return IDLE_WORK;
	}
	else if (DeviceActivedMode != true) {
		return NOTACTIVE_WORK;
	}
	else {
		return TCFG_SystemData.WorkMode;
	}
}

/**********************************************************************************************************
 @Function			void Radio_Trf_Set_Workmode(uint8_t val)
 @Description			Radio_Trf_Set_Workmode
 @Input				workmode
 @Return				void
**********************************************************************************************************/
void Radio_Trf_Set_Workmode(uint8_t val)
{
	TCFG_SystemData.WorkMode = val;
}

/**********************************************************************************************************
 @Function			void Radio_Trf_Xmit_Heartbeat(void)
 @Description			Radio_Trf_Xmit_Heartbeat
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radio_Trf_Xmit_Heartbeat(void)
{
	trf_heartbeat_s *pHeartBeat = (trf_heartbeat_s*)(trf_send_buf + 32);
	
	if (TRF_OK != Radio_Rf_get_Status()) {
		return;
	}
	
	memset((void*)pHeartBeat, 0, sizeof(trf_heartbeat_s));
	pHeartBeat->head.destSN		= 0xFFFFFFFF;
	pHeartBeat->head.version		= TRF_MSG_VERSION;
	pHeartBeat->head.type		= TRF_MSG_HEART;
	pHeartBeat->major_softver	= TCFG_Utility_Get_Major_Softnumber();
	pHeartBeat->sub_softver		= TCFG_Utility_Get_Sub_Softnumber();
	pHeartBeat->DevType			= MVB_MODEL_TYPE;
	pHeartBeat->sub_hardver		= 2;
	if (DeviceActivedMode == true) {
		pHeartBeat->workmode	= TCFG_SystemData.WorkMode;
	}
	else {
		pHeartBeat->workmode	= NOTACTIVE_WORK;
	}
	pHeartBeat->status			= 0;
	
	Radio_Trf_Cfg_Buildframe((uint8_t *)pHeartBeat, TMOTE_PLAIN_PUB, Radio_Trf_Xmit_Get_Pktnum(), TCFG_EEPROM_Get_MAC_SN(), trf_send_buf, sizeof(trf_heartbeat_s));
	Radio_Rf_Send(trf_send_buf, trf_send_buf[0]);
	Delay_MS(6);
}

/**********************************************************************************************************
 @Function			void Radio_Trf_Do_Heartbeat(void)
 @Description			Radio_Trf_Do_Heartbeat
 @Input				void
 @Return				void
**********************************************************************************************************/
void Radio_Trf_Do_Heartbeat(void)
{
#ifdef	RADIO_SI4438
	Radio_Rf_Interface_Init();
	Radio_Rf_Interrupt_Init();
	
	Radio_Trf_Xmit_Heartbeat();
#endif
}

/**********************************************************************************************************
 @Function			void Radio_Trf_Do_Rf_Pintf(char* info)
 @Description			Radio_Trf_Do_Rf_Pintf
 @Input				info
 @Return				void
**********************************************************************************************************/
void Radio_Trf_Do_Rf_Pintf(char* info)
{
	uint8_t infolen;
	trf_msg_s *pMsg = (trf_msg_s*)(trf_send_buf + 32);
	
	infolen = strlen(info);
	if (infolen > 46) {
		infolen = 46;
	}
	
	Radio_Rf_Interface_Init();
	Radio_Rf_Interrupt_Init();
	
	if (TRF_OK != Radio_Rf_get_Status()) {
		return;
	}
	
	pMsg->head.destSN			= 0xFFFFFFFF;
	pMsg->head.version			= TRF_MSG_VERSION;
	pMsg->head.type			= TRF_MSG_DEBUG_INFO;
	strncpy(pMsg->pData, info, infolen);
	
	Radio_Trf_Cfg_Buildframe((uint8_t *)pMsg, TMOTE_PLAIN_PUB, Radio_Trf_Xmit_Get_Pktnum(), TCFG_EEPROM_Get_MAC_SN(), trf_send_buf, sizeof(trf_msghead_s) + infolen);
	Radio_Rf_Send(trf_send_buf, trf_send_buf[0]);
	if (trf_send_buf[0] >=  RADIO_TX_ALMOST_EMPTY_THRESHOLD) {
		Radio_StartTx_dummy(RF_CHANNEL1);
	}
}

/**********************************************************************************************************
 @Function			void Radio_Trf_Debug_Printf(const char *fmt, ...)
 @Description			Radio_Trf_Debug_Printf		: Radio Debug Printf
 @Input				...
 @Return				void
**********************************************************************************************************/
void Radio_Trf_Debug_Printf(const char *fmt, ...)
{
	if (DEBUG_WORK == Radio_Trf_Get_Workmode()){
#ifdef	RADIO_SI4438
		__va_list args;
		va_start (args, fmt);
		
		memset(trf_print2buffer, 0, sizeof(trf_print2buffer));
		vsprintf(trf_print2buffer, fmt, args);
		va_end (args);
		Radio_Trf_Do_Rf_Pintf(trf_print2buffer);
#endif
	}
}

/**********************************************************************************************************
 @Function			void Radio_Trf_Printf(const char *fmt, ...)
 @Description			Radio_Trf_Printf			: Radio Printf
 @Input				...
 @Return				void
**********************************************************************************************************/
void Radio_Trf_Printf(const char *fmt, ...)
{
#ifdef	RADIO_SI4438
	__va_list args;
	va_start (args, fmt);
	
	memset(trf_print2buffer, 0, sizeof(trf_print2buffer));
	vsprintf (trf_print2buffer, fmt, args);
	va_end (args);
	Radio_Trf_Do_Rf_Pintf(trf_print2buffer);
#endif
}

/********************************************** END OF FLEE **********************************************/
