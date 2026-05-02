#ifndef NTC_TEMP_H
#define NTC_TEMP_H


#include "main.h"

// ================== 用户配置区 ==================
// 硬件参数
#define NTC_NOMINAL_R    100000.0f  // 100k NTC
#define NTC_NOMINAL_T    25.0f      // 25度
#define NTC_B_VALUE      3950.0f    // B值
#define SERIES_RESISTOR  20000.0f   // 分压电阻 20k
#define ADC_RESOLUTION   4095.0f    // 12位ADC

// 采样设置
#define NTC_SAMPLE_COUNT 20         // 连续采样20次取平均值 (防抖)

// ================== 函数声明 ==================
float NTC_Read_Temperature(ADC_HandleTypeDef *hadc);

#endif

