/**
  *********************************************************************************************************
  * @file    platform_map.c
  * @author  Movebroad -- KK
  * @version V1.0
  * @date    2018-03-24
  * @brief   
  *********************************************************************************************************
  * @attention
  *			
  *
  *
  *********************************************************************************************************
  */

#include "platform_map.h"
#include "platform_config.h"
#include "hal_eeprom.h"
#include "radio_hal_rf.h"
#include "radio_rf_app.h"
#include "string.h"

TCFG_SystemDataTypeDef			TCFG_SystemData;

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_SystemInfo_Init(void)
 @Description			TCFG_EEPROM_SystemInfo_Init					: 系统信息初始化
 @Input				void
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_SystemInfo_Init(void)
{
	TCFG_EEPROM_ReadConfigData();
	TCFG_EEPROM_SetBootMode(TCFG_ENV_BOOTMODE_NORMAL);
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_WriteConfigData(void)
 @Description			TCFG_EEPROM_WriteConfigData					: 写入系统配置信息
 @Input				void
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_WriteConfigData(void)
{
	char vender[4];
	unsigned int Brand = 0;
	
	/* 生产商与设备号 */
	TCFG_EEPROM_SetSNfromBrandKey(TCFG_EEPROM_Get_MAC_SN());
	TCFG_EEPROM_GetVender(vender);
	Brand |= (vender[0] & 0x000000FF) << 3*8;
	Brand |= (vender[1] & 0x000000FF) << 2*8;
	Brand |= (vender[2] & 0x000000FF) << 1*8;
	Brand |= (vender[3] & 0x000000FF) << 0*8;
	TCFG_EEPROM_SetFactoryBrand(Brand);
	
	/* 传感器灵敏度值 */
	TCFG_SystemData.Sensitivity = SENSE_MIDDLE;
	TCFG_EEPROM_SetSavedSensitivity(TCFG_SystemData.Sensitivity);
	
	/* 地磁扫描频率 */
	TCFG_SystemData.MagFreq = 1;
	TCFG_EEPROM_SetMagFreq(TCFG_SystemData.MagFreq);
	
	/* 车辆进入参数 */
	TCFG_SystemData.CarinThreshhold = 80;
	TCFG_EEPROM_SetCarinThreshhold(TCFG_SystemData.CarinThreshhold);
	
	/* 车辆离开参数 */
	TCFG_SystemData.CaroutThreshhold = 13;
	TCFG_EEPROM_SetCaroutThreshhold(TCFG_SystemData.CaroutThreshhold);
	
	/* 微小变化重计算次数 */
	TCFG_SystemData.RecalibrationNum = 10;
	TCFG_EEPROM_SetRecalibrationNum(TCFG_SystemData.RecalibrationNum);
	
	/* 激烈变化重计算时间 */
	TCFG_SystemData.RecalibrationOvertime = 1;
	TCFG_EEPROM_SetRecalibrationOverTime(TCFG_SystemData.RecalibrationOvertime);
	
	/* 设备工作模式 */
	TCFG_SystemData.WorkMode = NORMAL_WORK;
	TCFG_EEPROM_SetWorkMode(TCFG_SystemData.WorkMode);
	
	/* 无线通道选择 */
	TCFG_SystemData.RFChannel = 36;
	TCFG_EEPROM_SetRfChannel(TCFG_SystemData.RFChannel);
	
	/* 无线心跳间隔 */
	TCFG_SystemData.Heartinterval = HEART_INTERVAL;
	TCFG_EEPROM_SetHeartinterval(TCFG_SystemData.Heartinterval);
	
	/* 设备重启次数 */
	TCFG_SystemData.DeviceBootCount = 0;
	TCFG_EEPROM_SetDevBootCnt(TCFG_SystemData.DeviceBootCount);
	
	TCFG_SystemData.NBIotBootCount = 0;
	TCFG_EEPROM_SetNbiotBootCnt(TCFG_SystemData.NBIotBootCount);
	
	TCFG_SystemData.CoapRecvCount = 0;
	TCFG_EEPROM_SetCoapRecvCnt(TCFG_SystemData.CoapRecvCount);
	
	TCFG_SystemData.CoapSentCount = 0;
	TCFG_EEPROM_SetCoapSentCnt(TCFG_SystemData.CoapSentCount);
	
	TCFG_SystemData.MqttSNRecvCount = 0;
	TCFG_EEPROM_SetCoapRecvCnt(TCFG_SystemData.MqttSNRecvCount);
	
	TCFG_SystemData.MqttSNSentCount = 0;
	TCFG_EEPROM_SetMqttSNSentCnt(TCFG_SystemData.MqttSNSentCount);
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_ReadConfigData(void)
 @Description			TCFG_EEPROM_ReadConfigData					: 读取系统配置信息
 @Input				void
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_ReadConfigData(void)
{
	/* 获取SubSN */
	TCFG_SystemData.SubSn = TCFG_EEPROM_Get_MAC_SN();
	TCFG_EEPROM_Get_MAC_SN_String();
	
	/* 获取传感器灵敏度值 */
	TCFG_SystemData.Sensitivity = TCFG_EEPROM_GetSavedSensitivity();
	if ((TCFG_SystemData.Sensitivity > SENSE_LOWEST) || (TCFG_SystemData.Sensitivity < SENSE_HIGHEST)) {
		TCFG_SystemData.Sensitivity = SENSE_MIDDLE;
		TCFG_EEPROM_SetSavedSensitivity(TCFG_SystemData.Sensitivity);
	}
	
	/* 获取地磁扫描频率 */
	TCFG_SystemData.MagFreq = TCFG_EEPROM_GetMagFreq();
	if (TCFG_SystemData.MagFreq >= 4) {
		TCFG_SystemData.MagFreq = 1;
		TCFG_EEPROM_SetMagFreq(TCFG_SystemData.MagFreq);
	}
	
	/* 获取车辆进入参数 */
	TCFG_SystemData.CarinThreshhold = TCFG_EEPROM_GetCarinThreshhold();
	
	/* 获取车辆离开参数 */
	TCFG_SystemData.CaroutThreshhold = TCFG_EEPROM_GetCaroutThreshhold();
	
	/* 获取微小变化重计算次数 */
	TCFG_SystemData.RecalibrationNum = TCFG_EEPROM_GetRecalibrationNum();
	if (TCFG_SystemData.RecalibrationNum > 15) {
		TCFG_SystemData.RecalibrationNum = 15;
		TCFG_EEPROM_SetRecalibrationNum(TCFG_SystemData.RecalibrationNum);
	}
	
	/* 获取激烈变化重计算时间 */
	TCFG_SystemData.RecalibrationOvertime = TCFG_EEPROM_GetRecalibrationOverTime();
	if (TCFG_SystemData.RecalibrationOvertime > 15) {
		TCFG_SystemData.RecalibrationOvertime = 15;
		TCFG_EEPROM_SetRecalibrationOverTime(TCFG_SystemData.RecalibrationOvertime);
	}
	
	/* 获取设备工作模式 */
	TCFG_SystemData.WorkMode = TCFG_EEPROM_GetWorkMode();
	if ((TCFG_SystemData.WorkMode != DEBUG_WORK) && (TCFG_SystemData.WorkMode != NORMAL_WORK)) {
		TCFG_SystemData.WorkMode = NORMAL_WORK;
		TCFG_EEPROM_SetWorkMode(TCFG_SystemData.WorkMode);
	}
	
	/* 无线通道选择 */
	TCFG_SystemData.RFChannel = TCFG_EEPROM_GetRfChannel();
	switch (TCFG_SystemData.RFChannel)
	{
	case 36:
		RF_CHANNEL1 = 36;
		break;
	case 4:
		RF_CHANNEL1 = 4;
		break;
	case 6:
		RF_CHANNEL1 = 6;
		break;
	case 16:
		RF_CHANNEL1 = 16;
		break;
	case 26:
		RF_CHANNEL1 = 26;
		break;
	default:
		RF_CHANNEL1 = 36;
		TCFG_SystemData.RFChannel = 36;
		TCFG_EEPROM_SetRfChannel(TCFG_SystemData.RFChannel);
		break;
	}
	
	/* 无线心跳间隔 */
	TCFG_SystemData.Heartinterval = TCFG_EEPROM_GetHeartinterval();
	if ((TCFG_SystemData.Heartinterval > 120) || (TCFG_SystemData.Heartinterval < 10)) {
		TCFG_SystemData.Heartinterval = HEART_INTERVAL;
		TCFG_EEPROM_SetHeartinterval(TCFG_SystemData.Heartinterval);
	}
	
	/* 设备重启次数 */
	TCFG_SystemData.DeviceBootCount = TCFG_EEPROM_GetDevBootCnt() + 1;
	TCFG_EEPROM_SetDevBootCnt(TCFG_SystemData.DeviceBootCount);
	
	TCFG_SystemData.NBIotBootCount = TCFG_EEPROM_GetNbiotBootCnt();
	TCFG_SystemData.CoapRecvCount = TCFG_EEPROM_GetCoapRecvCnt();
	TCFG_SystemData.CoapSentCount = TCFG_EEPROM_GetCoapSentCnt();
	TCFG_SystemData.MqttSNRecvCount = TCFG_EEPROM_GetMqttSNRecvCnt();
	TCFG_SystemData.MqttSNSentCount = TCFG_EEPROM_GetMqttSNSentCnt();
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_SetBootMode(char bootmode)
 @Description			TCFG_EEPROM_SetBootMode						: 保存Boot启动模式
 @Input				bootmode
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_SetBootMode(char bootmode)
{
	FLASH_EEPROM_WriteBuffer(TCFG_ENVFLAG_OFFSET, (unsigned char*)"TCLD", TCFG_ENVFLAG_LENGTH);
	FLASH_EEPROM_WriteByte(TCFG_ENV_BOOTMODE_OFFSET, bootmode);
}

/**********************************************************************************************************
 @Function			char TCFG_EEPROM_GetBootMode(void)
 @Description			TCFG_EEPROM_GetBootMode						: 读取Boot启动模式
 @Input				void
 @Return				bootmode
**********************************************************************************************************/
char TCFG_EEPROM_GetBootMode(void)
{
	return FLASH_EEPROM_ReadByte(TCFG_ENV_BOOTMODE_OFFSET);
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_SaveConfigInformation(unsigned int subSN)
 @Description			TCFG_EEPROM_SaveConfigInformation				: 保存配置信息和SN
 @Input				subSN
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_SaveConfigInformation(unsigned int subSN)
{
	FLASH_EEPROM_WriteBuffer(TCFG_HEADFLAG_OFFSET, (unsigned char*)"TCLD", TCFG_HEADFLAG_LENGTH);
	FLASH_EEPROM_WriteWord(TCFG_SN_OFFSET, subSN);
}

/**********************************************************************************************************
 @Function			bool TCFG_EEPROM_CheckHeadFlag(char flagtype)
 @Description			TCFG_EEPROM_CheckHeadFlag					: 检测保存信息标志位
 @Input				flagtype
 @Return				bool
**********************************************************************************************************/
bool TCFG_EEPROM_CheckHeadFlag(char flagtype)
{
	if (flagtype == TCFG_FLAGTYPE_GENERAL) {
		if ('T' != FLASH_EEPROM_ReadByte(TCFG_HEADFLAG_OFFSET)) {
			return false;
		}
		if ('C' != FLASH_EEPROM_ReadByte(TCFG_HEADFLAG_OFFSET + 1)) {
			return false;
		}
		if ('L' != FLASH_EEPROM_ReadByte(TCFG_HEADFLAG_OFFSET + 2)) {
			return false;
		}
		if ('D' != FLASH_EEPROM_ReadByte(TCFG_HEADFLAG_OFFSET + 3)) {
			return false;
		}
	}
	else if (flagtype == TCFG_FLAGTYPE_MAGAlGORITHM) {
		if ('T' != FLASH_EEPROM_ReadByte(TCFG_MAGFLAG_OFFSET)) {
			return false;
		}
	}
	else if (flagtype == TCFG_FLAGTYPE_SECU) {
		if ('T' != FLASH_EEPROM_ReadByte(TCFG_SECUFLAG_OFFSET)) {
			return false;
		}
	}
	
	return true;
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_SetSpotID(char* spotID)
 @Description			TCFG_EEPROM_SetSpotID						: 保存SpotID
 @Input				&spotID
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_SetSpotID(char* spotID)
{
	FLASH_EEPROM_WriteBuffer(TCFG_SPOTID_OFFSET, (unsigned char*)spotID, TCFG_SPOTID_LENGTH);
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_GetSpotID(char* spotID)
 @Description			TCFG_EEPROM_GetSpotID						: 读取SpotID
 @Input				&spotID
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_GetSpotID(char* spotID)
{
	FLASH_EEPROM_ReadBuffer(TCFG_SPOTID_OFFSET, (unsigned char*)spotID, TCFG_SPOTID_LENGTH);
	if (strlen(spotID) >= TCFG_SPOTID_LENGTH) {
		strcpy(spotID, "unset");
	}
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_SetHeartinterval(unsigned short val)
 @Description			TCFG_EEPROM_SetHeartinterval					: 保存Heartinterval
 @Input				val
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_SetHeartinterval(unsigned short val)
{
	FLASH_EEPROM_WriteHalfWord(TCFG_HEARTINTERVAL_OFFSET, val);
}

/**********************************************************************************************************
 @Function			unsigned short TCFG_EEPROM_GetHeartinterval(void)
 @Description			TCFG_EEPROM_GetHeartinterval					: 读取Heartinterval
 @Input				void
 @Return				val
**********************************************************************************************************/
unsigned short TCFG_EEPROM_GetHeartinterval(void)
{
	return FLASH_EEPROM_ReadHalfWord(TCFG_HEARTINTERVAL_OFFSET);
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_SetSavedSensitivity(unsigned char val)
 @Description			TCFG_EEPROM_SetSavedSensitivity				: 保存Sensitivity
 @Input				val
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_SetSavedSensitivity(unsigned char val)
{
	FLASH_EEPROM_WriteByte(TCFG_MAG_SENSITIVITY_OFFSET, val);
}

/**********************************************************************************************************
 @Function			unsigned char TCFG_EEPROM_GetSavedSensitivity(void)
 @Description			TCFG_EEPROM_GetSavedSensitivity				: 读取Sensitivity
 @Input				void
 @Return				val
**********************************************************************************************************/
unsigned char TCFG_EEPROM_GetSavedSensitivity(void)
{
	return FLASH_EEPROM_ReadByte(TCFG_MAG_SENSITIVITY_OFFSET);
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_SetMagFreq(unsigned char val)
 @Description			TCFG_EEPROM_SetMagFreq						: 保存Freq
 @Input				val
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_SetMagFreq(unsigned char val)
{
	FLASH_EEPROM_WriteByte(TCFG_MAG_FREQ_OFFSET, val);
}

/**********************************************************************************************************
 @Function			unsigned char TCFG_EEPROM_GetMagFreq(void)
 @Description			TCFG_EEPROM_GetMagFreq						: 读取Freq
 @Input				void
 @Return				val
**********************************************************************************************************/
unsigned char TCFG_EEPROM_GetMagFreq(void)
{
	return FLASH_EEPROM_ReadByte(TCFG_MAG_FREQ_OFFSET);
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_SetMagBackgroud(int16_t x_axis, int16_t y_axis, int16_t z_axis)
 @Description			TCFG_EEPROM_SetMagBackgroud					: 保存地磁背景值
 @Input				x_axis
					y_axis
					z_axis
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_SetMagBackgroud(int16_t x_axis, int16_t y_axis, int16_t z_axis)
{
	FLASH_EEPROM_WriteHalfWord(TCFG_MAG_BACK_X_OFFSET, x_axis);
	FLASH_EEPROM_WriteHalfWord(TCFG_MAG_BACK_Y_OFFSET, y_axis);
	FLASH_EEPROM_WriteHalfWord(TCFG_MAG_BACK_Z_OFFSET, z_axis);
	
	FLASH_EEPROM_WriteByte(TCFG_MAGFLAG_OFFSET, 'T');
}

/**********************************************************************************************************
 @Function			unsigned short TCFG_EEPROM_GetMagBackgroud(char axis)
 @Description			TCFG_EEPROM_GetMagBackgroud					: 读取地磁背景值
 @Input				axis
 @Return				val
**********************************************************************************************************/
unsigned short TCFG_EEPROM_GetMagBackgroud(char axis)
{
	if (true == TCFG_EEPROM_CheckHeadFlag(TCFG_FLAGTYPE_MAGAlGORITHM)) {
		if (axis == TCFG_X_AXIS) {
			return FLASH_EEPROM_ReadHalfWord(TCFG_MAG_BACK_X_OFFSET);
		}
		else if (axis == TCFG_Y_AXIS) {
			return FLASH_EEPROM_ReadHalfWord(TCFG_MAG_BACK_Y_OFFSET);
		}
		else if (axis == TCFG_Z_AXIS) {
			return FLASH_EEPROM_ReadHalfWord(TCFG_MAG_BACK_Z_OFFSET);
		}
	}
	
	return 0x7FFF;
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_SetSubSN(unsigned int subsn)
 @Description			TCFG_EEPROM_SetSubSN						: 保存SubSN
 @Input				subsn
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_SetSubSN(unsigned int subsn)
{
	FLASH_EEPROM_WriteWord(TCFG_SN_OFFSET, subsn);
}

/**********************************************************************************************************
 @Function			unsigned int TCFG_EEPROM_GetSubSN(void)
 @Description			TCFG_EEPROM_GetSubSN						: 读取SubSN
 @Input				void
 @Return				subsn
**********************************************************************************************************/
unsigned int TCFG_EEPROM_GetSubSN(void)
{
	return FLASH_EEPROM_ReadWord(TCFG_SN_OFFSET);
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_SetBrandCode(unsigned int brand)
 @Description			TCFG_EEPROM_SetBrandCode						: 保存BrandCode
 @Input				brand
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_SetBrandCode(unsigned int brand)
{
	FLASH_EEPROM_WriteWord(TCFG_SECU_BRAND_OFFSET, brand);
}

/**********************************************************************************************************
 @Function			unsigned int TCFG_EEPROM_GetBrandCode(void)
 @Description			TCFG_EEPROM_GetBrandCode						: 读取BrandCode
 @Input				void
 @Return				brand
**********************************************************************************************************/
unsigned int TCFG_EEPROM_GetBrandCode(void)
{
	return FLASH_EEPROM_ReadWord(TCFG_SECU_BRAND_OFFSET);
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_SetBootCount(unsigned char count)
 @Description			TCFG_EEPROM_SetBootCount						: 保存BootCount
 @Input				count
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_SetBootCount(unsigned char count)
{
	FLASH_EEPROM_WriteByte(TCFG_RECORD_BOOTCNT_OFFSET, count);
}

/**********************************************************************************************************
 @Function			unsigned char TCFG_EEPROM_GetBootCount(void)
 @Description			TCFG_EEPROM_GetBootCount						: 读取BootCount
 @Input				void
 @Return				count
**********************************************************************************************************/
unsigned char TCFG_EEPROM_GetBootCount(void)
{
	return FLASH_EEPROM_ReadByte(TCFG_RECORD_BOOTCNT_OFFSET);
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_SetServerIP(unsigned int val)
 @Description			TCFG_EEPROM_SetServerIP						: 保存ServerIP
 @Input				val
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_SetServerIP(unsigned int val)
{
	FLASH_EEPROM_WriteWord(TCFG_RECORD_SERVER_OFFSET, val);
}

/**********************************************************************************************************
 @Function			unsigned int TCFG_EEPROM_GetServerIP(void)
 @Description			TCFG_EEPROM_GetServerIP						: 读取ServerIP
 @Input				void
 @Return				val
**********************************************************************************************************/
unsigned int TCFG_EEPROM_GetServerIP(void)
{
	return FLASH_EEPROM_ReadWord(TCFG_RECORD_SERVER_OFFSET);
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_SetServerPort(unsigned short val)
 @Description			TCFG_EEPROM_SetServerPort					: 保存ServerPort
 @Input				val
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_SetServerPort(unsigned short val)
{
	FLASH_EEPROM_WriteHalfWord(TCFG_RECORD_SERVER_OFFSET + 4, val);
}

/**********************************************************************************************************
 @Function			unsigned short TCFG_EEPROM_GetServerPort(void)
 @Description			TCFG_EEPROM_GetServerPort					: 读取ServerPort
 @Input				void
 @Return				val
**********************************************************************************************************/
unsigned short TCFG_EEPROM_GetServerPort(void)
{
	return FLASH_EEPROM_ReadHalfWord(TCFG_RECORD_SERVER_OFFSET + 4);
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_SetWorkMode(unsigned char val)
 @Description			TCFG_EEPROM_SetWorkMode						: 保存WorkMode
 @Input				val
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_SetWorkMode(unsigned char val)
{
	FLASH_EEPROM_WriteByte(TCFG_WORKMODE_OFFSET, val);
}

/**********************************************************************************************************
 @Function			unsigned char TCFG_EEPROM_GetWorkMode(void)
 @Description			TCFG_EEPROM_GetWorkMode						: 读取WorkMode
 @Input				void
 @Return				val
**********************************************************************************************************/
unsigned char TCFG_EEPROM_GetWorkMode(void)
{
	return FLASH_EEPROM_ReadByte(TCFG_WORKMODE_OFFSET);
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_SetRadarCount(unsigned int val)
 @Description			TCFG_EEPROM_SetRadarCount					: 保存RadarCount
 @Input				val
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_SetRadarCount(unsigned int val)
{
	FLASH_EEPROM_WriteWord(TCFG_RADARCOUNT_OFFSET, val);
}

/**********************************************************************************************************
 @Function			unsigned int TCFG_EEPROM_GetRadarCount(void)
 @Description			TCFG_EEPROM_GetRadarCount					: 读取RadarCount
 @Input				void
 @Return				val
**********************************************************************************************************/
unsigned int TCFG_EEPROM_GetRadarCount(void)
{
	return FLASH_EEPROM_ReadWord(TCFG_RADARCOUNT_OFFSET);
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_SetRadarDbgMode(unsigned char val)
 @Description			TCFG_EEPROM_SetRadarDbgMode					: 保存RadarDbgMode
 @Input				val
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_SetRadarDbgMode(unsigned char val)
{
	if (val > 8) val = 0;
	else val = 8 - val;
	FLASH_EEPROM_WriteByte(TCFG_RADARDBG_OFFSET, val);
}

/**********************************************************************************************************
 @Function			unsigned char TCFG_EEPROM_GetRadarDbgMode(void)
 @Description			TCFG_EEPROM_GetRadarDbgMode					: 读取RadarDbgMode
 @Input				void
 @Return				val
**********************************************************************************************************/
unsigned char TCFG_EEPROM_GetRadarDbgMode(void)
{
	unsigned char val8;
	
	val8 = FLASH_EEPROM_ReadByte(TCFG_RADARDBG_OFFSET);
	if (val8 > 8) return 0;
	else val8 = 8 - val8;
	
	return val8;
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_SetStatusCount(unsigned int val)
 @Description			TCFG_EEPROM_SetStatusCount					: 保存StatusCount
 @Input				val
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_SetStatusCount(unsigned int val)
{
	FLASH_EEPROM_WriteWord(TCFG_STATUSCNT_OFFSET, val);
}

/**********************************************************************************************************
 @Function			unsigned int TCFG_EEPROM_GetStatusCount(void)
 @Description			TCFG_EEPROM_GetStatusCount					: 读取StatusCount
 @Input				void
 @Return				val
**********************************************************************************************************/
unsigned int TCFG_EEPROM_GetStatusCount(void)
{
	return FLASH_EEPROM_ReadWord(TCFG_STATUSCNT_OFFSET);
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_SetRfChannel(unsigned char val)
 @Description			TCFG_EEPROM_SetRfChannel						: 保存RfChannel
 @Input				val
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_SetRfChannel(unsigned char val)
{
	FLASH_EEPROM_WriteByte(TCFG_RFCHANNEL_OFFSET, val);
}

/**********************************************************************************************************
 @Function			unsigned char TCFG_EEPROM_GetRfChannel(void)
 @Description			TCFG_EEPROM_GetRfChannel						: 读取RfChannel
 @Input				void
 @Return				val
**********************************************************************************************************/
unsigned char TCFG_EEPROM_GetRfChannel(void)
{
	return FLASH_EEPROM_ReadByte(TCFG_RFCHANNEL_OFFSET);
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_SetEnableNBiotPSM(unsigned char val)
 @Description			TCFG_EEPROM_SetEnableNBiotPSM					: 保存EnableNBiotPSM
 @Input				val
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_SetEnableNBiotPSM(unsigned char val)
{
	FLASH_EEPROM_WriteByte(TCFG_ENABLE_NBIOTPSM_OFFSET, val);
}

/**********************************************************************************************************
 @Function			unsigned char TCFG_EEPROM_GetEnableNBiotPSM(void)
 @Description			TCFG_EEPROM_GetEnableNBiotPSM					: 读取EnableNBiotPSM
 @Input				void
 @Return				val
**********************************************************************************************************/
unsigned char TCFG_EEPROM_GetEnableNBiotPSM(void)
{
	return FLASH_EEPROM_ReadByte(TCFG_ENABLE_NBIOTPSM_OFFSET);
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_SetActiveDeice(unsigned char val)
 @Description			TCFG_EEPROM_SetActiveDeice					: 保存ActiveDeice
 @Input				val
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_SetActiveDevice(unsigned char val)
{
	if (val) val = 0;
	else val = 1;
	FLASH_EEPROM_WriteByte(TCFG_ACTIVE_DEVICE_OFFSET, val);
}

/**********************************************************************************************************
 @Function			unsigned char TCFG_EEPROM_GetActiveDeice(void)
 @Description			TCFG_EEPROM_GetActiveDeice					: 读取ActiveDeice
 @Input				void
 @Return				val
**********************************************************************************************************/
unsigned char TCFG_EEPROM_GetActiveDevice(void)
{
	if (1 == FLASH_EEPROM_ReadByte(TCFG_ACTIVE_DEVICE_OFFSET)) {
		return 0;
	}
	else {
		return 1;
	}
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_SetNbiotBootCnt(unsigned int val)
 @Description			TCFG_EEPROM_SetNbiotBootCnt					: 保存NbiotBootCnt
 @Input				val
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_SetNbiotBootCnt(unsigned int val)
{
	FLASH_EEPROM_WriteWord(TCFG_NBIOT_BOOTCNT_OFFSET, val);
}

/**********************************************************************************************************
 @Function			unsigned int TCFG_EEPROM_GetNbiotBootCnt(void)
 @Description			TCFG_EEPROM_GetNbiotBootCnt					: 读取NbiotBootCnt
 @Input				void
 @Return				val
**********************************************************************************************************/
unsigned int TCFG_EEPROM_GetNbiotBootCnt(void)
{
	return FLASH_EEPROM_ReadWord(TCFG_NBIOT_BOOTCNT_OFFSET);
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_SetCoapSentCnt(unsigned int val)
 @Description			TCFG_EEPROM_SetCoapSentCnt					: 保存CoapSentCnt
 @Input				val
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_SetCoapSentCnt(unsigned int val)
{
	FLASH_EEPROM_WriteWord(TCFG_COAP_SENTCNT_OFFSET, val);
}

/**********************************************************************************************************
 @Function			unsigned int TCFG_EEPROM_GetCoapSentCnt(void)
 @Description			TCFG_EEPROM_GetCoapSentCnt					: 读取CoapSentCnt
 @Input				void
 @Return				val
**********************************************************************************************************/
unsigned int TCFG_EEPROM_GetCoapSentCnt(void)
{
	return FLASH_EEPROM_ReadWord(TCFG_COAP_SENTCNT_OFFSET);
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_SetCoapRecvCnt(unsigned int val)
 @Description			TCFG_EEPROM_SetCoapRecvCnt					: 保存CoapRecvCnt
 @Input				val
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_SetCoapRecvCnt(unsigned int val)
{
	FLASH_EEPROM_WriteWord(TCFG_COAP_RECVCNT_OFFSET, val);
}

/**********************************************************************************************************
 @Function			unsigned int TCFG_EEPROM_GetCoapRecvCnt(void)
 @Description			TCFG_EEPROM_GetCoapRecvCnt					: 读取CoapRecvCnt
 @Input				void
 @Return				val
**********************************************************************************************************/
unsigned int TCFG_EEPROM_GetCoapRecvCnt(void)
{
	return FLASH_EEPROM_ReadWord(TCFG_COAP_RECVCNT_OFFSET);
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_SetMqttSNSentCnt(unsigned int val)
 @Description			TCFG_EEPROM_SetMqttSNSentCnt					: 保存MqttSNSentCnt
 @Input				val
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_SetMqttSNSentCnt(unsigned int val)
{
	FLASH_EEPROM_WriteWord(TCFG_MQTTSN_SENTCNT_OFFSET, val);
}

/**********************************************************************************************************
 @Function			unsigned int TCFG_EEPROM_GetMqttSNSentCnt(void)
 @Description			TCFG_EEPROM_GetMqttSNSentCnt					: 读取MqttSNSentCnt
 @Input				void
 @Return				val
**********************************************************************************************************/
unsigned int TCFG_EEPROM_GetMqttSNSentCnt(void)
{
	return FLASH_EEPROM_ReadWord(TCFG_MQTTSN_SENTCNT_OFFSET);
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_SetMqttSNRecvCnt(unsigned int val)
 @Description			TCFG_EEPROM_SetMqttSNRecvCnt					: 保存MqttSNRecvCnt
 @Input				val
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_SetMqttSNRecvCnt(unsigned int val)
{
	FLASH_EEPROM_WriteWord(TCFG_MQTTSN_RECVCNT_OFFSET, val);
}

/**********************************************************************************************************
 @Function			unsigned int TCFG_EEPROM_GetMqttSNRecvCnt(void)
 @Description			TCFG_EEPROM_GetMqttSNRecvCnt					: 读取MqttSNRecvCnt
 @Input				void
 @Return				val
**********************************************************************************************************/
unsigned int TCFG_EEPROM_GetMqttSNRecvCnt(void)
{
	return FLASH_EEPROM_ReadWord(TCFG_MQTTSN_RECVCNT_OFFSET);
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_SetDevBootCnt(unsigned short val)
 @Description			TCFG_EEPROM_SetDevBootCnt					: 保存DevBootCnt
 @Input				val
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_SetDevBootCnt(unsigned short val)
{
	FLASH_EEPROM_WriteHalfWord(TCFG_DEV_BOOTCNT_OFFSET, val);
}

/**********************************************************************************************************
 @Function			unsigned short TCFG_EEPROM_GetDevBootCnt(void)
 @Description			TCFG_EEPROM_GetDevBootCnt					: 读取DevBootCnt
 @Input				void
 @Return				val
**********************************************************************************************************/
unsigned short TCFG_EEPROM_GetDevBootCnt(void)
{
	return FLASH_EEPROM_ReadHalfWord(TCFG_DEV_BOOTCNT_OFFSET);
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_SetEventTime(unsigned int val)
 @Description			TCFG_EEPROM_SetEventTime						: 保存EventTime
 @Input				val
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_SetEventTime(unsigned int val)
{
	FLASH_EEPROM_WriteWord(TCFG_EVENT_TIME_OFFSET, val);
}

/**********************************************************************************************************
 @Function			unsigned int TCFG_EEPROM_GetEventTime(void)
 @Description			TCFG_EEPROM_GetEventTime						: 读取EventTime
 @Input				void
 @Return				val
**********************************************************************************************************/
unsigned int TCFG_EEPROM_GetEventTime(void)
{
	return FLASH_EEPROM_ReadWord(TCFG_EVENT_TIME_OFFSET);
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_SetBackgroundTemp(int16_t val)
 @Description			TCFG_EEPROM_SetBackgroundTemp					: 保存BackgroundTemp
 @Input				val
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_SetBackgroundTemp(int16_t val)
{
	FLASH_EEPROM_WriteHalfWord(TCFG_TEMP_BACKGROUND_OFFSET, val);
}

/**********************************************************************************************************
 @Function			int16_t TCFG_EEPROM_GetBackgroundTemp(void)
 @Description			TCFG_EEPROM_GetBackgroundTemp					: 读取BackgroundTemp
 @Input				void
 @Return				val
**********************************************************************************************************/
int16_t TCFG_EEPROM_GetBackgroundTemp(void)
{
	return FLASH_EEPROM_ReadHalfWord(TCFG_TEMP_BACKGROUND_OFFSET);
}

/**********************************************************************************************************
 @Function			bool TCFG_EEPROM_CheckInfoBurned(void)
 @Description			TCFG_EEPROM_CheckInfoBurned					: 检测生产商信息
 @Input				void
 @Return				bool
**********************************************************************************************************/
bool TCFG_EEPROM_CheckInfoBurned(void)
{
	if ( (0x00 == FLASH_EEPROM_ReadByte(TCFG_FACTORY_BRAND_SN_OFFSET + 0)) && \
		(0x00 == FLASH_EEPROM_ReadByte(TCFG_FACTORY_BRAND_SN_OFFSET + 1)) && \
		(0x00 == FLASH_EEPROM_ReadByte(TCFG_FACTORY_BRAND_SN_OFFSET + 2)) && \
		(0x00 == FLASH_EEPROM_ReadByte(TCFG_FACTORY_BRAND_SN_OFFSET + 3)) ) {
		return false;
	}
	else {
		return true;
	}
}

/**********************************************************************************************************
 @Function			bool TCFG_EEPROM_CheckNewSNorBrand(void)
 @Description			TCFG_EEPROM_CheckNewSNorBrand					: 检测新的设备号或厂牌信息
 @Input				void
 @Return				bool
**********************************************************************************************************/
bool TCFG_EEPROM_CheckNewSNorBrand(void)
{
	bool ret = false;
	char vender[4];
	unsigned int Brand = 0;
	
	if (TCFG_EEPROM_CheckInfoBurned() != true) {								//厂牌空
		ret = true;
	}
	
	if (TCFG_EEPROM_Get_MAC_SN() != TCFG_EEPROM_GetSNfromBrandKey()) {			//设备号不同
		ret = true;
	}
	
	TCFG_EEPROM_GetVender(vender);
	Brand |= (vender[0] & 0x000000FF) << 3*8;
	Brand |= (vender[1] & 0x000000FF) << 2*8;
	Brand |= (vender[2] & 0x000000FF) << 1*8;
	Brand |= (vender[3] & 0x000000FF) << 0*8;
	if (Brand != TCFG_EEPROM_GetFactoryBrand()) {							//厂牌号不同
		ret = true;
	}
	
	return ret;
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_SetSNfromBrandKey(unsigned int val)
 @Description			TCFG_EEPROM_SetSNfromBrandKey					: 保存SNfromBrandKey
 @Input				val
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_SetSNfromBrandKey(unsigned int val)
{
	FLASH_EEPROM_WriteWord(TCFG_FACTORY_BRAND_SN_OFFSET + 4, val);
}

/**********************************************************************************************************
 @Function			unsigned int TCFG_EEPROM_GetSNfromBrandKey(void)
 @Description			TCFG_EEPROM_GetSNfromBrandKey					: 读取SNfromBrandKey
 @Input				void
 @Return				val
**********************************************************************************************************/
unsigned int TCFG_EEPROM_GetSNfromBrandKey(void)
{
	return FLASH_EEPROM_ReadWord(TCFG_FACTORY_BRAND_SN_OFFSET + 4);
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_SetFactoryBrand(unsigned int val)
 @Description			TCFG_EEPROM_SetFactoryBrand					: 保存FactoryBrand
 @Input				val
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_SetFactoryBrand(unsigned int val)
{
	FLASH_EEPROM_WriteWord(TCFG_FACTORY_BRAND_SN_OFFSET, val);
}

/**********************************************************************************************************
 @Function			unsigned int TCFG_EEPROM_GetFactoryBrand(void)
 @Description			TCFG_EEPROM_GetFactoryBrand					: 读取FactoryBrand
 @Input				void
 @Return				val
**********************************************************************************************************/
unsigned int TCFG_EEPROM_GetFactoryBrand(void)
{
	return FLASH_EEPROM_ReadWord(TCFG_FACTORY_BRAND_SN_OFFSET);
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_Set_MAC_SN(unsigned int subsn)
 @Description			TCFG_EEPROM_Set_MAC_SN						: 保存MAC SN
 @Input				sn
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_Set_MAC_SN(unsigned int sn)
{
	unsigned int subsn = 0;
	
	subsn |= (sn & 0xFF000000) >> 3*8;
	subsn |= (sn & 0x00FF0000) >> 1*8;
	subsn |= (sn & 0x0000FF00) << 1*8;
	subsn |= (sn & 0x000000FF) << 3*8;
	
	FLASH_EEPROM_WriteWord(TCFG_FACTORY_MAC_SN_OFFSET, subsn);
}

/**********************************************************************************************************
 @Function			unsigned int TCFG_EEPROM_Get_MAC_SN(void)
 @Description			TCFG_EEPROM_Get_MAC_SN						: 读取MAC SN
 @Input				void
 @Return				sn
**********************************************************************************************************/
unsigned int TCFG_EEPROM_Get_MAC_SN(void)
{
	unsigned int subsn = 0;
	unsigned int sn = 0;
	
	subsn = FLASH_EEPROM_ReadWord(TCFG_FACTORY_MAC_SN_OFFSET);
	sn |= (subsn & 0xFF000000) >> 3*8;
	sn |= (subsn & 0x00FF0000) >> 1*8;
	sn |= (subsn & 0x0000FF00) << 1*8;
	sn |= (subsn & 0x000000FF) << 3*8;
	
	return sn;
}

/**********************************************************************************************************
 @Function			char* TCFG_EEPROM_Get_MAC_SN_String(void)
 @Description			TCFG_EEPROM_Get_MAC_SN_String					: 读取MAC SN字符串
 @Input				void
 @Return				sn_string
**********************************************************************************************************/
char* TCFG_EEPROM_Get_MAC_SN_String(void)
{
	unsigned int subsn = 0;
	unsigned int sn = 0;
	
	subsn = FLASH_EEPROM_ReadWord(TCFG_FACTORY_MAC_SN_OFFSET);
	sn |= (subsn & 0xFF000000) >> 3*8;
	sn |= (subsn & 0x00FF0000) >> 1*8;
	sn |= (subsn & 0x0000FF00) << 1*8;
	sn |= (subsn & 0x000000FF) << 3*8;
	
	memset((void *)TCFG_SystemData.SubMacSN, 0x0, sizeof(TCFG_SystemData.SubMacSN));
	sprintf((char*)TCFG_SystemData.SubMacSN, "%08x", sn);
	
	return (char*)TCFG_SystemData.SubMacSN;
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_SetVender(char* vender)
 @Description			TCFG_EEPROM_SetVender						: 保存vender
 @Input				&vender
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_SetVender(char* vender)
{
	FLASH_EEPROM_WriteBuffer(TCFG_FACTORY_VENDER_OFFSET, (unsigned char*)vender, TCFG_FACTORY_VENDER_LENGTH);
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_GetVender(char* vender)
 @Description			TCFG_EEPROM_GetVender						: 读取vender
 @Input				&vender
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_GetVender(char* vender)
{
	FLASH_EEPROM_ReadBuffer(TCFG_FACTORY_VENDER_OFFSET, (unsigned char*)vender, TCFG_FACTORY_VENDER_LENGTH);
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_SetCarinThreshhold(unsigned char val)
 @Description			TCFG_EEPROM_SetCarinThreshhold				: 保存carin_threshhold
 @Input				val
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_SetCarinThreshhold(unsigned char val)
{
	FLASH_EEPROM_WriteByte(TCFG_CARIN_THREHOLD_OFFSET, val);
}

/**********************************************************************************************************
 @Function			unsigned char TCFG_EEPROM_GetCarinThreshhold(void)
 @Description			TCFG_EEPROM_GetCarinThreshhold				: 读取carin_threshhold
 @Input				void
 @Return				val
**********************************************************************************************************/
unsigned char TCFG_EEPROM_GetCarinThreshhold(void)
{
	return FLASH_EEPROM_ReadByte(TCFG_CARIN_THREHOLD_OFFSET);
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_SetCaroutThreshhold(unsigned char val)
 @Description			TCFG_EEPROM_SetCaroutThreshhold				: 保存carout_threshhold
 @Input				val
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_SetCaroutThreshhold(unsigned char val)
{
	FLASH_EEPROM_WriteByte(TCFG_CAROUT_THREHOLD_OFFSET, val);
}

/**********************************************************************************************************
 @Function			unsigned char TCFG_EEPROM_GetCaroutThreshhold(void)
 @Description			TCFG_EEPROM_GetCaroutThreshhold				: 读取carout_threshhold
 @Input				void
 @Return				val
**********************************************************************************************************/
unsigned char TCFG_EEPROM_GetCaroutThreshhold(void)
{
	return FLASH_EEPROM_ReadByte(TCFG_CAROUT_THREHOLD_OFFSET);
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_SetRecalibrationNum(unsigned char val)
 @Description			TCFG_EEPROM_SetRecalibrationNum				: 保存recalibration_num
 @Input				val
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_SetRecalibrationNum(unsigned char val)
{
	FLASH_EEPROM_WriteByte(TCFG_RECALIBRA_NUM_OFFSET, val);
}

/**********************************************************************************************************
 @Function			unsigned char TCFG_EEPROM_GetRecalibrationNum(void)
 @Description			TCFG_EEPROM_GetRecalibrationNum				: 读取recalibration_num
 @Input				void
 @Return				val
**********************************************************************************************************/
unsigned char TCFG_EEPROM_GetRecalibrationNum(void)
{
	return FLASH_EEPROM_ReadByte(TCFG_RECALIBRA_NUM_OFFSET);
}

/**********************************************************************************************************
 @Function			void TCFG_EEPROM_SetRecalibrationOverTime(unsigned char val)
 @Description			TCFG_EEPROM_SetRecalibrationOverTime			: 保存recalibration_overtime
 @Input				val
 @Return				void
**********************************************************************************************************/
void TCFG_EEPROM_SetRecalibrationOverTime(unsigned char val)
{
	FLASH_EEPROM_WriteByte(TCFG_RECALIBRA_TIME_OFFSET, val);
}

/**********************************************************************************************************
 @Function			unsigned char TCFG_EEPROM_GetRecalibrationOverTime(void)
 @Description			TCFG_EEPROM_GetRecalibrationOverTime			: 读取recalibration_overtime
 @Input				void
 @Return				val
**********************************************************************************************************/
unsigned char TCFG_EEPROM_GetRecalibrationOverTime(void)
{
	return FLASH_EEPROM_ReadByte(TCFG_RECALIBRA_TIME_OFFSET);
}

/**********************************************************************************************************
 @Function			unsigned char TCFG_Utility_Get_Major_Softnumber(void)
 @Description			TCFG_Utility_Get_Major_Softnumber				: 读取Major_Softnumber
 @Input				void
 @Return				val
**********************************************************************************************************/
unsigned char TCFG_Utility_Get_Major_Softnumber(void)
{
	return SOFTWAREMAJOR;
}

/**********************************************************************************************************
 @Function			unsigned char TCFG_Utility_Get_Sub_Softnumber(void)
 @Description			TCFG_Utility_Get_Sub_Softnumber				: 读取Sub_Softnumber
 @Input				void
 @Return				val
**********************************************************************************************************/
unsigned char TCFG_Utility_Get_Sub_Softnumber(void)
{
	return SOFTWARESUB;
}

/**********************************************************************************************************
 @Function			unsigned char TCFG_Utility_Get_Major_Hardnumber(void)
 @Description			TCFG_Utility_Get_Sub_Hardnumber				: 读取Major_Hardnumber
 @Input				void
 @Return				val
**********************************************************************************************************/
unsigned char TCFG_Utility_Get_Major_Hardnumber(void)
{
	return HARDWAREMAJOR;
}

/********************************************** END OF FLEE **********************************************/
