#ifndef __INSPECT_FUNC_H
#define   __INSPECT_FUNC_H

#include "sys.h"
#include "inspectconfig.h"

Inspect_Qmc5883l_StatusTypeDef Inspect_Qmc5883l_Pass_Detect(int16_t x_mag, int16_t y_mag, int16_t z_mag);		//车辆检测
Inspect_Qmc5883l_StatusTypeDef Inspect_Qmc5883l_CheckStatus(unsigned char index);							//检测状态
unsigned char Inspect_Qmc5883l_KmeansAllocate(void);													//质心分配
unsigned char Inspect_Qmc5883l_BackgroundCalculate(void);												//背景计算










#endif
