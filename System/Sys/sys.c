/**
  *********************************************************************************************************
  * @file    sys.c
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

__IO u32 SystemRunningTimesSecond = 0;										//ϵͳ����ʱ��(S)

/**********************************************************************************************************
 @Function			void Stm32_Clock_Init(u32 pllmul, u32 plldiv)
 @Description			ʱ�����ú���(MAX32MHz)
 @Input				pllmul	: ��PLL��Ƶϵ��(PLL��Ƶ),ȡֵ��Χ:0 ~ 48
					plldiv	: ϵͳʱ�ӵ���PLL��Ƶϵ��(PLL֮��ķ�Ƶ),ȡֵ��Χ:2,3,4.(������3��ֵ!)
 @Return				void
 @attention			Fsys = Fs*(pllmul/plldiv);

					Fsys : ϵͳʱ��Ƶ��
					Fs	: PLL����ʱ��Ƶ��,������HSI,HSE��
**********************************************************************************************************/
void Stm32_Clock_Init(u32 pllmul, u32 plldiv)
{
	HAL_StatusTypeDef ret = HAL_OK;
	RCC_OscInitTypeDef RCC_OscInitStructure;
	RCC_ClkInitTypeDef RCC_ClkInitStructure;
	
	HAL_RCC_DeInit();

	RCC_OscInitStructure.OscillatorType = RCC_OSCILLATORTYPE_HSI;				//ʱ��ԴΪHSI
	RCC_OscInitStructure.HSIState = RCC_HSI_ON;								//��HSI
	RCC_OscInitStructure.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;		//HSIĬ��У׼
	RCC_OscInitStructure.PLL.PLLState = RCC_PLL_ON;							//��PLL
	RCC_OscInitStructure.PLL.PLLSource = RCC_PLLSOURCE_HSI;					//PLLʱ��Դѡ��HSI
	RCC_OscInitStructure.PLL.PLLMUL = pllmul;								//��PLL��Ƶϵ��(PLL��Ƶ)
	RCC_OscInitStructure.PLL.PLLDIV = plldiv;								//ϵͳʱ�ӵ���PLL��Ƶϵ��(PLL֮��ķ�Ƶ)
	ret = HAL_RCC_OscConfig(&RCC_OscInitStructure);							//��ʼ��
	if (ret != HAL_OK) while(1);
	
	__HAL_RCC_PWR_CLK_ENABLE();											//ʹ��PWRʱ��
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);				//���õ�ѹ�������ѹ����, �Ա�������δ�����Ƶ�ʹ���
	while (__HAL_PWR_GET_FLAG(PWR_FLAG_VOS) != RESET) {};

	/* ѡ��PLL��Ϊϵͳʱ��Դ��������HCLK, PCLK1 �� PCLK2 */
	RCC_ClkInitStructure.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStructure.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;				//����ϵͳʱ��ʱ��ԴΪPLL
	RCC_ClkInitStructure.AHBCLKDivider = RCC_SYSCLK_DIV1;						//AHB��Ƶϵ��Ϊ1
	RCC_ClkInitStructure.APB1CLKDivider = RCC_HCLK_DIV1;						//APB1��Ƶϵ��Ϊ1
	RCC_ClkInitStructure.APB2CLKDivider = RCC_HCLK_DIV1;						//APB2��Ƶϵ��Ϊ1

	ret = HAL_RCC_ClockConfig(&RCC_ClkInitStructure, FLASH_LATENCY_1);			//ͬʱ����FLASH��ʱ����Ϊ1WS, Ҳ����2��CPU����
	if (ret != HAL_OK) while(1);
}

/**********************************************************************************************************
 @Function			void Stm32_MSIClock_Init(u32 msiClockRange)
 @Description			ʱ�����ú���(MAX32MHz)
 @Input				msiClockRange : 
 @attention			RCC_MSIRANGE_0				!< MSI = 65.536  KHz
					RCC_MSIRANGE_1				!< MSI = 131.072 KHz
					RCC_MSIRANGE_2				!< MSI = 262.144 KHz
					RCC_MSIRANGE_3				!< MSI = 524.288 KHz
					RCC_MSIRANGE_4				!< MSI = 1.048   MHz
					RCC_MSIRANGE_5				!< MSI = 2.097   MHz
					RCC_MSIRANGE_6				!< MSI = 4.194   MHz
  @Return				void
**********************************************************************************************************/
void Stm32_MSIClock_Init(u32 MsiClockRange)
{
	HAL_StatusTypeDef ret = HAL_OK;
	RCC_OscInitTypeDef RCC_OscInitStructure;
	RCC_ClkInitTypeDef RCC_ClkInitStructure;
	
	HAL_RCC_DeInit();
	
	RCC_OscInitStructure.OscillatorType = RCC_OSCILLATORTYPE_MSI;				//ʱ��ԴΪMSI
	RCC_OscInitStructure.MSIState = RCC_MSI_ON;								//��MSI
	RCC_OscInitStructure.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;		//MSIĬ��У׼
	RCC_OscInitStructure.MSIClockRange = MsiClockRange;						//MSIValue
	RCC_OscInitStructure.PLL.PLLState = RCC_PLL_OFF;							//�ر�PLL
	ret = HAL_RCC_OscConfig(&RCC_OscInitStructure);							//��ʼ��
	if (ret != HAL_OK) while(1);
	
	RCC_OscInitStructure.OscillatorType = RCC_OSCILLATORTYPE_HSI;				//ʱ��ԴΪHSI --> ADC
	RCC_OscInitStructure.HSIState = RCC_HSI_ON;								//��HSI
	RCC_OscInitStructure.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;		//HSIĬ��У׼
	RCC_OscInitStructure.PLL.PLLState = RCC_PLL_OFF;							//�ر�PLL
	ret = HAL_RCC_OscConfig(&RCC_OscInitStructure);							//��ʼ��
	if (ret != HAL_OK) while(1);
	
	__HAL_RCC_PWR_CLK_ENABLE();											//ʹ��PWRʱ��
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);				//���õ�ѹ�������ѹ����, �Ա�������δ�����Ƶ�ʹ���
	while (__HAL_PWR_GET_FLAG(PWR_FLAG_VOS) != RESET) {};
	
	/* ѡ��MSI��Ϊϵͳʱ��Դ��������HCLK, PCLK1 �� PCLK2 */
	RCC_ClkInitStructure.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStructure.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;					//����ϵͳʱ��ʱ��ԴΪMSI
	RCC_ClkInitStructure.AHBCLKDivider = RCC_SYSCLK_DIV1;						//AHB��Ƶϵ��Ϊ1
	RCC_ClkInitStructure.APB1CLKDivider = RCC_HCLK_DIV1;						//APB1��Ƶϵ��Ϊ1
	RCC_ClkInitStructure.APB2CLKDivider = RCC_HCLK_DIV1;						//APB2��Ƶϵ��Ϊ1
	
	ret = HAL_RCC_ClockConfig(&RCC_ClkInitStructure, FLASH_LATENCY_1);			//ͬʱ����FLASH��ʱ����Ϊ1WS, Ҳ����2��CPU����
	if (ret != HAL_OK) while(1);
}

/**********************************************************************************************************
 @Function			void Stm32_IncSecondTick(void)
 @Description			Stm32_IncSecondTick : ����ʱ���1��
 @Input				void
 @Return				void
**********************************************************************************************************/
void Stm32_IncSecondTick(void)
{
	static u32 SecondTickPre;
	u32 SecondTickNow;
	
	SecondTickNow = HAL_GetTick();
	if ((SecondTickNow > SecondTickPre) && ((SecondTickNow - SecondTickPre) >= 1000)) {
		SystemRunningTimesSecond++;
		SecondTickPre += 1000;
	}
	else if ((0xFFFFFFFF - SecondTickPre + SecondTickNow + 1) >= 1000) {
		SystemRunningTimesSecond++;
		SecondTickPre += 1000;
	}
}

/**********************************************************************************************************
 @Function			u32 Stm32_GetSecondTick(void)
 @Description			Stm32_GetSecondTick 	: ��ȡ��������
 @Input				void
 @Return				SystemRunningTimesSecond : ��������
**********************************************************************************************************/
u32 Stm32_GetSecondTick(void)
{
	return SystemRunningTimesSecond;
}

/**********************************************************************************************************
 @Function			void Stm32_Calculagraph_Init(Stm32_CalculagraphTypeDef* timer)
 @Description			Stm32_Calculagraph_Init 			: ��ʼ����ʱ��
 @Input				Stm32_CalculagraphTypeDef*		: ��ʱ���ṹ��ָ��
 @Return				void
**********************************************************************************************************/
void Stm32_Calculagraph_Init(Stm32_CalculagraphTypeDef* timer)
{
	timer->xTicksToWait = 0;
	timer->xTimeOut = 0;
}

/**********************************************************************************************************
 @Function			void Stm32_Calculagraph_CountdownMS(Stm32_CalculagraphTypeDef* timer, u32 timeout_ms)
 @Description			Stm32_Calculagraph_CountdownMS	: ���ü�ʱ������ʱ��(MS)
 @Input				Stm32_CalculagraphTypeDef*		: ��ʱ���ṹ��ָ��
					timeout_ms					: ����ʱʱ��(MS)
 @Return				void
**********************************************************************************************************/
void Stm32_Calculagraph_CountdownMS(Stm32_CalculagraphTypeDef* timer, u32 timeout_ms)
{
	timer->xTicksToWait = timeout_ms;
	timer->xTimeOut = HAL_GetTick() + timeout_ms;
}

/**********************************************************************************************************
 @Function			bool Stm32_Calculagraph_IsExpiredMS(Stm32_CalculagraphTypeDef* timer)
 @Description			Stm32_Calculagraph_IsExpiredMS	: ��ѯ�Ƿ񵽴��ʱ����ʱʱ��(MS)
 @Input				Stm32_CalculagraphTypeDef*		: ��ʱ���ṹ��ָ��
 @Return				true							: ����
					false						: δ����
**********************************************************************************************************/
bool Stm32_Calculagraph_IsExpiredMS(Stm32_CalculagraphTypeDef* timer)
{
	u32 MilliSecondTickNow;
	
	MilliSecondTickNow = HAL_GetTick();
	
	if ((MilliSecondTickNow > (timer->xTimeOut - timer->xTicksToWait)) && ((MilliSecondTickNow - (timer->xTimeOut - timer->xTicksToWait)) >= timer->xTicksToWait)) {
		return true;
	}
	else if ((0xFFFFFFFF - (timer->xTimeOut - timer->xTicksToWait) + MilliSecondTickNow + 1) >= timer->xTicksToWait) {
		return true;
	}
	else {
		return false;
	}
}

/**********************************************************************************************************
 @Function			void Stm32_Calculagraph_CountdownSec(Stm32_CalculagraphTypeDef* timer, u32 timeout_sec)
 @Description			Stm32_Calculagraph_CountdownSec	: ���ü�ʱ������ʱ��(S)
 @Input				Stm32_CalculagraphTypeDef*		: ��ʱ���ṹ��ָ��
					timeout_sec					: ����ʱʱ��(S)
 @Return				void
**********************************************************************************************************/
void Stm32_Calculagraph_CountdownSec(Stm32_CalculagraphTypeDef* timer, u32 timeout_sec)
{
	timer->xTicksToWait = timeout_sec;
	timer->xTimeOut = Stm32_GetSecondTick() + timeout_sec;
}

/**********************************************************************************************************
 @Function			bool Stm32_Calculagraph_IsExpiredSec(Stm32_CalculagraphTypeDef* timer)
 @Description			Stm32_Calculagraph_IsExpiredSec	: ��ѯ�Ƿ񵽴��ʱ����ʱʱ��(S)
 @Input				Stm32_CalculagraphTypeDef*		: ��ʱ���ṹ��ָ��
 @Return				true							: ����
					false						: δ����
**********************************************************************************************************/
bool Stm32_Calculagraph_IsExpiredSec(Stm32_CalculagraphTypeDef* timer)
{
	u32 SecondTickNow;
	
	SecondTickNow = Stm32_GetSecondTick();
	
	if ((SecondTickNow > (timer->xTimeOut - timer->xTicksToWait)) && ((SecondTickNow - (timer->xTimeOut - timer->xTicksToWait)) >= timer->xTicksToWait)) {
		return true;
	}
	else if ((0xFFFFFFFF - (timer->xTimeOut - timer->xTicksToWait) + SecondTickNow + 1) >= timer->xTicksToWait) {
		return true;
	}
	else {
		return false;
	}
}


#ifdef  USE_FULL_ASSERT
/**********************************************************************************************************
 @Function			void assert_failed(uint8_t* file, uint32_t line)
 @Description			��������ʾ�����ʱ��˺����������������ļ���������
 @Input				file		: ָ��Դ�ļ�
					line		: ָ�����ļ��е�����
 @Return				void
**********************************************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{
	while (1)
	{
	}
}
#endif

//THUMBָ�֧�ֻ������
//�������·���ʵ��ִ�л��ָ��WFI  
__asm void WFI_SET(void)
{
	WFI;		  
}
//�ر������ж�(���ǲ�����fault��NMI�ж�)
__asm void INTX_DISABLE(void)
{
	CPSID   I
	BX      LR	  
}
//���������ж�
__asm void INTX_ENABLE(void)
{
	CPSIE   I
	BX      LR  
}
//����ջ����ַ
//addr:ջ����ַ
__asm void MSR_MSP(u32 addr) 
{
	MSR MSP, r0 			//set Main Stack value
	BX r14
}

/********************************************** END OF FLEE **********************************************/
