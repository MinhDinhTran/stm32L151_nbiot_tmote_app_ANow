#ifndef __PLATFORM_MAP_H
#define   __PLATFORM_MAP_H

#include "sys.h"

#define MVB_STATICMAGNETIC_MODEL			51
#define MVB_FLOWMAGNETIC_MODEL			52
#define MVB_MODEL_TYPE					MVB_STATICMAGNETIC_MODEL




#define TCFG_FLAGTYPE_GENERAL				0
#define TCFG_FLAGTYPE_MAGAlGORITHM			1
#define TCFG_FLAGTYPE_SECU				2

#define TCFG_X_AXIS						0
#define TCFG_Y_AXIS						1
#define TCFG_Z_AXIS						2





#define EEPROM_BASE_ADDRESS				0x08080000
#define EEPROM_CONFIG_OFFSET				0x08080000
#define EEPROM_CONFIG_SIZE				0x0300

#define TCFG_FACTORY_BRAND_SN_OFFSET		0x0400											//0x08080400
#define TCFG_FACTORY_BRAND_SN_LENGTH		8												//

/* The environment parameters are used both by bootLoader and application */
#define TCFG_ENVFLAG_OFFSET				TCFG_FACTORY_BRAND_SN_OFFSET + TCFG_FACTORY_BRAND_SN_LENGTH	//0x08080408
#define TCFG_ENVFLAG_LENGTH				4												//"TCLD"
#define TCFG_ENV_BOOTMODE_OFFSET			TCFG_ENVFLAG_OFFSET + TCFG_ENVFLAG_LENGTH				//0x0808040C
#define TCFG_ENV_BOOTMODE_LENGTH			1												//boot app from flash or load app from air
/********************************** end ***********************************/

#define TCFG_HEADFLAG_OFFSET				TCFG_ENV_BOOTMODE_OFFSET + TCFG_ENV_BOOTMODE_LENGTH		//0x0808040D
#define TCFG_HEADFLAG_LENGTH				4												//"TCLD"
#define TCFG_SN_OFFSET					TCFG_HEADFLAG_OFFSET + TCFG_HEADFLAG_LENGTH				//0x08080411
#define TCFG_SN_LENGTH					4												//SN
#define TCFG_SPOTID_OFFSET				TCFG_SN_OFFSET + TCFG_SN_LENGTH						//0x08080415
#define TCFG_SPOTID_LENGTH				16												//SPOTID
#define TCFG_HEARTINTERVAL_OFFSET			TCFG_SPOTID_OFFSET + TCFG_SPOTID_LENGTH					//0x08080425
#define TCFG_HEARTINTERVAL_LENGTH			2												//Heart Interval
#define TCFG_MAG_SENSITIVITY_OFFSET		TCFG_HEARTINTERVAL_OFFSET + TCFG_HEARTINTERVAL_LENGTH		//0x08080427
#define TCFG_MAG_SENSITIVITY_LENGTH		1												//Sensitivity
#define TCFG_MAG_FREQ_OFFSET				TCFG_MAG_SENSITIVITY_OFFSET + TCFG_MAG_SENSITIVITY_LENGTH	//0x08080428
#define TCFG_MAG_FREQ_LENGTH				1												//Freq

#define TCFG_MAGFLAG_OFFSET				TCFG_MAG_FREQ_OFFSET + TCFG_MAG_FREQ_LENGTH				//0x08080429
#define TCFG_MAGFLAG_LENGTH				1												//'T' = 0x54
#define TCFG_MAG_BACK_X_OFFSET			TCFG_MAGFLAG_OFFSET + TCFG_MAGFLAG_LENGTH				//0x0808042A
#define TCFG_MAG_BACK_X_LENGTH			2												//Mag X Back
#define TCFG_MAG_BACK_Y_OFFSET			TCFG_MAG_BACK_X_OFFSET + TCFG_MAG_BACK_X_LENGTH			//0x0808042C
#define TCFG_MAG_BACK_Y_LENGTH			2												//Mag Y Back
#define TCFG_MAG_BACK_Z_OFFSET			TCFG_MAG_BACK_Y_OFFSET + TCFG_MAG_BACK_Y_LENGTH			//0x0808042E
#define TCFG_MAG_BACK_Z_LENGTH			2												//Mag Z Back

#define TCFG_SECUFLAG_OFFSET				TCFG_MAG_BACK_Z_OFFSET + TCFG_MAG_BACK_Z_LENGTH			//0x08080430
#define TCFG_SECUFLAG_LENGTH				1												//'T' = 0x54
#define TCFG_SECU_BRAND_OFFSET			TCFG_SECUFLAG_OFFSET + TCFG_SECUFLAG_LENGTH				//0x08080431
#define TCFG_SECU_BRAND_LENGTH			6												//BrandCode
#define TCFG_SECU_BRANDKEY_OFFSET			TCFG_SECU_BRAND_OFFSET + TCFG_SECU_BRAND_LENGTH			//0x08080437
#define TCFG_SECU_BRANDKEY_LENGTH			32												//Reserved
#define TCFG_SECU_BRANCHKEY_OFFSET			TCFG_SECU_BRANDKEY_OFFSET + TCFG_SECU_BRANDKEY_LENGTH		//0x08080457
#define TCFG_SECU_BRANCHKEY_LENGTH			32												//Reserved

#define TCFG_RECORD_RUNTIME_OFFSET			TCFG_SECU_BRANCHKEY_OFFSET + TCFG_SECU_BRANCHKEY_LENGTH	//0x08080477
#define TCFG_RECORD_RUNTIME_LENGTH			4												//Reserved

#define TCFG_RECORD_BOOTCNT_OFFSET			TCFG_RECORD_RUNTIME_OFFSET + TCFG_RECORD_RUNTIME_LENGTH	//0x0808047B
#define TCFG_RECORD_BOOTCNT_LENGTH			1												//BootCnt

#define TCFG_APP_ENV0_OFFSET				TCFG_RECORD_BOOTCNT_OFFSET + TCFG_RECORD_BOOTCNT_LENGTH	//0x0808047C	偏移地址
#define TCFG_APP_ENV0_ADDRESS				EEPROM_BASE_ADDRESS + TCFG_APP_ENV0_OFFSET				//0x0808047C	绝对地址
#define TCFG_APP_ENV0_SIZE				24												//参数区域0
#define TCFG_APP_ENV1_OFFSET				TCFG_APP_ENV0_OFFSET + TCFG_APP_ENV0_SIZE				//0x08080494	偏移地址
#define TCFG_APP_ENV1_ADDRESS				EEPROM_BASE_ADDRESS + TCFG_APP_ENV1_OFFSET				//0x08080494	绝对地址
#define TCFG_APP_ENV1_SIZE				24												//参数区域1

#define TCFG_RECORD_SERVER_OFFSET			TCFG_APP_ENV1_OFFSET + TCFG_APP_ENV1_SIZE				//0x080804AC
#define TCFG_RECORD_SERVER_LENGTH			6												//Server Coap















void TCFG_EEPROM_SetBootMode(char bootmode);													//保存Boot启动模式
char TCFG_EEPROM_GetBootMode(void);														//读取Boot启动模式

void TCFG_EEPROM_SetSpotID(char* spotID);													//保存SpotID
void TCFG_EEPROM_GetSpotID(char* spotID);													//读取SpotID

void TCFG_EEPROM_SetHeartinterval(unsigned short val);											//保存Heartinterval
unsigned short TCFG_EEPROM_GetHeartinterval(void);											//读取Heartinterval

void TCFG_EEPROM_SetSavedSensitivity(unsigned char val);										//保存Sensitivity
unsigned char TCFG_EEPROM_GetSavedSensitivity(void);											//读取Sensitivity

void TCFG_EEPROM_SetMagFreq(unsigned char val);												//保存Freq
unsigned char TCFG_EEPROM_GetMagFreq(void);													//读取Freq

void TCFG_EEPROM_SetMagBackgroud(int16_t x_axis, int16_t y_axis, int16_t z_axis);					//保存地磁背景值
unsigned short TCFG_EEPROM_GetMagBackgroud(char axis);											//读取地磁背景值

void TCFG_EEPROM_SetBrandCode(unsigned int brand);											//保存BrandCode
unsigned int TCFG_EEPROM_GetBrandCode(void);													//读取BrandCode

void TCFG_EEPROM_SetBootCount(unsigned char count);											//保存BootCount
unsigned char TCFG_EEPROM_GetBootCount(void);												//读取BootCount

void TCFG_EEPROM_SaveConfigInformation(unsigned int subSN);										//保存配置信息和SN
bool TCFG_EEPROM_CheckHeadFlag(char flagtype);												//检测保存信息标志位













#endif
