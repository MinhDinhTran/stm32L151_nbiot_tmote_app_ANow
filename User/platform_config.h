#ifndef __PLATFORM_CONFIG_H
#define   __PLATFORM_CONFIG_H

#include "sys.h"

#define	SYSTEMCLOCKMSI					4194000
#define	SYSTEMCLOCKHSI					32000000
#define	SYSTEMCLOCK					SYSTEMCLOCKMSI						//系统时钟

#define	NETCOAP						0
#define	NETMQTTSN						1
#define	NETPROTOCAL					NETCOAP							//网络模式

#define	SOFTWAREMAJOR					2								//主固件版本
#define	SOFTWARESUB					3								//从固件版本
#define	HARDWAREMAJOR					2								//主硬件版本

#define	RADIO_SI4438

#define	IDLE_WORK						4								//休眠
#define	NOTACTIVE_WORK					3								//不工作

#define	DEBUG_WORK					1								//调试工作模式
#define	NORMAL_WORK					0								//正常工作模式

extern bool DeviceIdleMode;
extern bool DeviceActivedMode;









#endif
