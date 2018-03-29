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
#include "radio.h"
#include "radio_msg_config.h"
#include "tmesh_xtea.h"
#include "platform_map.h"
#include "platform_config.h"
#include "delay.h"
#include "string.h"
#include <stdarg.h>

static frameInfo_t sInFrameQ[SIZE_INFRAME_Q];

uint8_t trf_send_buf[256] = {0};
uint8_t trf_recv_buf[256] = {0};

char trf_print2buffer[256] = {0};

static uint32_t hearttime_pre = 0;
uint16_t trf_heart_interval = 30;

uint8_t trf_work_mode = NORMAL_WORK;
uint8_t trf_sensitivity = SENSE_HIGHEST;

bool isIdleMode = true;
bool is_actived = true;

void tmesh_rf_QInit(void)
{
	memset(sInFrameQ, 0, sizeof(sInFrameQ));
}

void tmesh_rf_QPush(mrfiPacket_t* rfpacket)
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

mrfiPacket_t* tmesh_rf_QPop(void)
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

void tmesh_rf_data_handle_isr(mrfiPacket_t* rfpacket)
{
	tmesh_rf_QPush(rfpacket);
}

char tmesh_rf_receive(uint8_t *outmsg, uint8_t *len)
{
	mrfiPacket_t* rfpacket;
	rfpacket = tmesh_rf_QPop();
	
	if (rfpacket != NULL) {
		*len = 1 + (CFG_GET_FROM_FRAME(CFG_P_FRAME_LEN(rfpacket->frame), CFG_LEN_OS));
		memcpy(outmsg, rfpacket->frame, *len);
		
		return TRF_SUCCESS;
	}
	
	return TRF_NO_PAYLOAD;
}

char tmesh_rf_send(uint8_t *inmsg, uint8_t len)
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

char tmesh_rf_operate_recvmsg(uint8_t *inmsg, uint8_t len)
{
	char rc = TRF_SUCCESS;
	trf_msg_s* pPayload;
	
	if (CFG_GET_FROM_FRAME(CFG_P_FRAME_PRE(inmsg), CFG_PRE_TYPE_OS) == XMESHCFG_PRT)
	{
		pPayload = (trf_msg_s*)CFG_P_FRAME_PAYLOAD(inmsg);
		if (CFG_GET_FROM_FRAME(CFG_P_FRAME_HEAD(inmsg), CFG_HEAD_TYPE_OS) == TMOTE_PLAIN_SET)
		{
			uint32_t cnt;
			uint8_t payload_len;
			
			cnt = CFG_GET_FROM_FRAME(CFG_P_FRAME_HEAD(inmsg), CFG_PKTNUM_OS);
			payload_len = CFG_GET_PAYLOAD_LEN(inmsg) + CFG_FRAME_LEN_SIZE;
			tmesh_securityMix(0x81010001);
			tmesh_decipher((uint8_t*)pPayload, payload_len , &cnt);
			if (pPayload->head.destSN != 0x81010001) {
				rc = TRF_NOT_FORME;
				return rc;
			}
			
			if (pPayload->head.type == TRF_MSG_UPGRADE) {
				
				
			}
			else if (pPayload->head.type == TRF_MSG_SENSITIVITY) {
				
				
			}
			else if (pPayload->head.type == TRF_MSG_WORKMODE) {
				
				
			}
			else if (pPayload->head.type == TRF_MSG_RFHEART_INTERVAL) {
				
				
			}
			else if (pPayload->head.type == TRF_MSG_INITBACKGROUND) {
				
				
			}
			else if (pPayload->head.type == TRF_MSG_GENERAL_CMD) {
				
				
			}
		}
		else {
			if (pPayload->head.destSN != 0x81010001) {
				rc = TRF_NOT_FORME;
				return rc;
			}
			if (CFG_GET_FROM_FRAME(CFG_P_FRAME_HEAD(inmsg), CFG_HEAD_TYPE_OS) == TMOTE_PLAIN_GET)
			{
				if (pPayload->head.type == TRF_MSG_BASICINFO) {
					
				}
				else if (pPayload->head.type == TRF_MSG_SENSITIVITY) {
					
				}
				else if (pPayload->head.type == TRF_MSG_WORKMODE) {
					
				}
				else if (pPayload->head.type == TRF_MSG_INITBACKGROUND) {
					
				}
			}
			else if (CFG_GET_FROM_FRAME(CFG_P_FRAME_HEAD(inmsg), CFG_HEAD_TYPE_OS) == TMOTE_PLAIN_ACK)
			{
				
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
		trf_default_resp(100, pPayload->head.type);
	}
	
	return rc;
}

extern void BEEP_CtrlRepeat(u16 nCount, u16 nMs);

/**********************************************************************************************************
 @Function			void trf_app_task(void)
 @Description			trf_app_task
 @Input				void
 @Return				void
**********************************************************************************************************/
void trf_app_task(void)
{
	uint8_t len = 0;
	
	if (TRF_OK != Radio_Rf_get_Status()) {
		return;
	}
	
	hearttime_pre++;
	if ((hearttime_pre % 30) == 0) {
		Radio_Rf_Interface_Init();
		Radio_Rf_Interrupt_Init();
		
		trf_xmit_heartbeat();
		if (DEBUG_WORK == trf_get_workmode()) {
			
		}
	}
	
	if (TRF_SUCCESS == tmesh_rf_receive(trf_recv_buf, &len)) {
		if (TRF_SUCCESS == tmesh_rf_operate_recvmsg(trf_recv_buf, len)) {
			BEEP_CtrlRepeat(5, 100);
		}
	}
}

/**********************************************************************************************************
 @Function			void trf_cfg_buildframe(uint8_t *inmsg, uint8_t pkttype, uint8_t pktnum, uint32_t sn, uint8_t *outsend, uint8_t len)
 @Description			trf_cfg_buildframe
 @Input				...
 @Return				void
**********************************************************************************************************/
void trf_cfg_buildframe(uint8_t *inmsg, uint8_t pkttype, uint8_t pktnum, uint32_t sn, uint8_t *outsend, uint8_t len)
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
 @Function			uint8_t trf_xmit_get_pktnum(void)
 @Description			trf_xmit_get_pktnum
 @Input				void
 @Return				pktnum
**********************************************************************************************************/
uint8_t trf_xmit_get_pktnum(void)
{
	static uint8_t pktnum = 0;
	return pktnum++;
}

/**********************************************************************************************************
 @Function			void trf_default_resp(uint8_t ret, uint8_t type)
 @Description			trf_default_resp
 @Input				void
 @Return				void
**********************************************************************************************************/
void trf_default_resp(uint8_t ret, uint8_t type)
{
	trf_defaultrsp_s *pDefaultRsp = (trf_defaultrsp_s*)(trf_send_buf + 32);
	pDefaultRsp->head.destSN		= 0xFFFFFFFF;
	pDefaultRsp->head.version	= TRF_MSG_VERSION;
	pDefaultRsp->head.type		= type;
	pDefaultRsp->ret			= ret;
	
	trf_cfg_buildframe((uint8_t *)pDefaultRsp, TMOTE_PLAIN_RSP, trf_xmit_get_pktnum(), 0x81010001, trf_send_buf, sizeof(trf_defaultrsp_s));
	tmesh_rf_send(trf_send_buf, trf_send_buf[0]);
}

/**********************************************************************************************************
 @Function			uint8_t trf_get_workmode(void)
 @Description			trf_get_workmode
 @Input				void
 @Return				workmode
**********************************************************************************************************/
uint8_t trf_get_workmode(void)
{
	if (isIdleMode == true) {
		return IDLE_WORK;
	}
	else if (is_actived != true) {
		return NOTACTIVE_WORK;
	}
	else {
		return trf_work_mode;
	}
}

/**********************************************************************************************************
 @Function			void trf_set_workmode(uint8_t val)
 @Description			trf_set_workmode
 @Input				workmode
 @Return				void
**********************************************************************************************************/
void trf_set_workmode(uint8_t val)
{
	trf_work_mode = val;
}

/**********************************************************************************************************
 @Function			void trf_xmit_heartbeat(void)
 @Description			trf_xmit_heartbeat
 @Input				void
 @Return				void
**********************************************************************************************************/
void trf_xmit_heartbeat(void)
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
	if (is_actived == true) {
		pHeartBeat->workmode	= trf_work_mode;
	}
	else {
		pHeartBeat->workmode	= NOTACTIVE_WORK;
	}
	pHeartBeat->status			= 0;
	
	trf_cfg_buildframe((uint8_t *)pHeartBeat, TMOTE_PLAIN_PUB, trf_xmit_get_pktnum(), 0x81010001, trf_send_buf, sizeof(trf_heartbeat_s));
	tmesh_rf_send(trf_send_buf, trf_send_buf[0]);
	Delay_MS(6);
}

/**********************************************************************************************************
 @Function			void trf_do_heartbeat(void)
 @Description			trf_do_heartbeat
 @Input				void
 @Return				void
**********************************************************************************************************/
void trf_do_heartbeat(void)
{
#ifdef	RADIO_SI4438
	Radio_Rf_Interface_Init();
	Radio_Rf_Interrupt_Init();
	
	trf_xmit_heartbeat();
#endif
}

/**********************************************************************************************************
 @Function			void trf_do_rfpintf(char* info)
 @Description			trf_do_rfpintf
 @Input				info
 @Return				void
**********************************************************************************************************/
void trf_do_rfpintf(char* info)
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
	
	trf_cfg_buildframe((uint8_t *)pMsg, TMOTE_PLAIN_PUB, trf_xmit_get_pktnum(), 0x81010001, trf_send_buf, sizeof(trf_msghead_s) + infolen);
	tmesh_rf_send(trf_send_buf, trf_send_buf[0]);
	if (trf_send_buf[0] >=  RADIO_TX_ALMOST_EMPTY_THRESHOLD) {
		Radio_StartTx_dummy(RF_CHANNEL1);
	}
}

/**********************************************************************************************************
 @Function			void trf_dprintf(const char *fmt, ...)
 @Description			trf_dprintf			: Radio Debug Printf
 @Input				...
 @Return				void
**********************************************************************************************************/
void trf_dprintf(const char *fmt, ...)
{
	if (DEBUG_WORK == trf_get_workmode()){
#ifdef	RADIO_SI4438
		__va_list args;
		va_start (args, fmt);
		
		memset(trf_print2buffer, 0, sizeof(trf_print2buffer));
		vsprintf(trf_print2buffer, fmt, args);
		va_end (args);
		trf_do_rfpintf(trf_print2buffer);
#endif
	}
}

/**********************************************************************************************************
 @Function			void trf_printf(const char *fmt, ...)
 @Description			trf_printf			: Radio Printf
 @Input				...
 @Return				void
**********************************************************************************************************/
void trf_printf(const char *fmt, ...)
{
#ifdef	RADIO_SI4438
	__va_list args;
	va_start (args, fmt);
	
	memset(trf_print2buffer, 0, sizeof(trf_print2buffer));
	vsprintf (trf_print2buffer, fmt, args);
	va_end (args);
	trf_do_rfpintf(trf_print2buffer);
#endif
}

/********************************************** END OF FLEE **********************************************/
