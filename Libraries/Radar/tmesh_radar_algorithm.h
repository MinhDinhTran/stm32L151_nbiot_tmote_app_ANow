/**
  *********************************************************************************************************
  * @file    tmesh_radar_algorithm.h
  * @author  Movebroad -- Bell Zhong
  * @version V0.2.2
  * @date    2017-11-14
  * @brief   
  *********************************************************************************************************
  * @attention
  *		   171114 : 根据3.3V供电mcu进行调整, 对应运放(R23,R25) = (10K,100R),
  *				  旧款雷达模块, VPTAT没有接到VTUNE, 对应DAC输出值变化为 : 50->(50+304*6)
  *
  *
  *********************************************************************************************************
  */

#ifndef __TMESH_RADAR_ALGORITHM_H
#define   __TMESH_RADAR_ALGORITHM_H

#include "sys.h"

#define	NOT_COVERED			0
#define	BE_COVERED			1
#define	MAY_COVERED			2

#define	STATUS_NO_TARGET		0
#define	STATUS_COVER_WATER		1
#define	STATUS_COVER_SOLID		2
#define	STATUS_HAS_TARGET		3

#define	TRADAR_SAMPLE_NUM		(256+48)
#define	TRADAR_BACKGROUND_NUM	15

enum TRADAR_DETECT_STATUS
{
	TRADAR_NO_TARGET			= 0x00,
	TRADAR_BE_COVERED			= 0x01,
	TRADAR_HAS_TARGET			= 0x02
};

enum TRADAR_ERRCODE
{
	TRADAR_OK					= 0x00,
	TRADAR_ERROR				= 0x01,
	TRADAR_NO_BACKGROUND		= 0x02,
	TRADAR_UNKNOWN				= 0x03
};

enum TRADAR_VDD
{
	TRADAR_3V3				= 0x00,
	TRADAR_2V8				= 0x01
};

enum TRADAR_SENSITIVITY
{
	RADAR_HIGHEST				= 0x01,
	RADAR_HIGH				= 0x02,
	RADAR_MIDDLE				= 0x03,
	RADAR_LOW					= 0x04,
	RADAR_LOWEST				= 0x05,
};

typedef struct
{
	u32	signal_strength;
	u8	distance_cm;
	u8	status;
	u16	strenth_total_diff;
	u16	debuginfo[10];
}tradar_targetinfo_s;

/*
 * ----------------------------------------------------------------------------
 * Function Name: tradar_background_transform
 * Purpose: get the backgroud frequency spectrum
 * Params: 
 *		datain_t: input, the amplitude in time domain
 *		in_num: input, the number of datain_t,it must the same with TRADAR_SAMPLE_NUM
 *		fre_strenth_out: output, the amplitude in frequency domain
 *		out_num: output, the number of fre_strenth_out,it must be the same with TRADAR_BACKGROUND_NUM
 * Returns:TRADAR_ERROR or TRADAR_OK
 * Note:
 *		__attribute((section(".ARM.__at_0x8010000")));
 * ----------------------------------------------------------------------------
 */
char tradar_background_transform(u32 datain_t[], u16 in_num, u16 fre_strenth_out[], u8 out_num);

/*
 * ----------------------------------------------------------------------------
 * Function Name: tradar_background_set
 * Purpose: set the background frequency spectrum
 * Params:
 *		fre_magBG: input, the amplitude in time domain
 *		in_num: input, the number of fre_magBG,it must the same with TRADAR_SAMPLE_NUM
 * Returns:TRADAR_ERROR or TRADAR_OK
 * Note:
 *		__attribute((section(".ARM.__at_0x8010030")));
 * ----------------------------------------------------------------------------
 */
char tradar_background_set(u16* fre_magBG, u8 in_num);

/*
 * ----------------------------------------------------------------------------
 * Function Name: tradar_target_detect
 * Purpose: detect the target info
 * Params:
 *		datain_t: input, the amplitude in time domain
 *		in_num: input, the number of datain_t,it must the same with TRADAR_SAMPLE_NUM
 *		targetinfo: output, the target info
 * Returns:
 * Note:
 *		__attribute((section(".ARM.__at_0x8010060")));
 * ----------------------------------------------------------------------------
 */
char tradar_target_detect(u32 datain_t[], u16 in_mum, tradar_targetinfo_s* targetinfo);

/*
 * ----------------------------------------------------------------------------
 * Function Name: tradar_get_lib_version
 * Purpose: get the library version
 * Params:
 *		none
 * Returns: the version description
 * Note:
 *		__attribute((section(".ARM.__at_0x8010060")));
 * ----------------------------------------------------------------------------
 */
char* tradar_get_lib_version(void);

/*
 * ----------------------------------------------------------------------------
 * Function Name: tradar_get_lib_num
 * Purpose: get the library version
 * Params:
 *		none
 * Returns: the version numer
 * Note:
 * ----------------------------------------------------------------------------
 */
char tradar_get_lib_num(void);

/*
 * ----------------------------------------------------------------------------
 * Function Name: tradar_set_vdd
 * Purpose: set the vdd of mcu
 * Params:
 *		TRADAR_3V3,TRADAR_2V8
 * Returns: none
 * Note:
 * ----------------------------------------------------------------------------
 */
void tradar_set_vdd(char vdd_type);

/*
 * ----------------------------------------------------------------------------
 * Function Name: tradar_set_sensetivity
 * Purpose: set sensetivity of radar
 * Params:
 *		sensetivity
 * Returns: none
 * Note:
 * ----------------------------------------------------------------------------
 */
void tradar_set_sensetivity(unsigned char sense);

/*
 * ----------------------------------------------------------------------------
 * Function Name: tradar_get_sensetivity
 * Purpose: get sensetivity of radar
 * Params:
 *		none
 * Returns: sensetivity
 * Note:
 * ----------------------------------------------------------------------------
 */
unsigned char tradar_get_sensetivity(void);

#endif
