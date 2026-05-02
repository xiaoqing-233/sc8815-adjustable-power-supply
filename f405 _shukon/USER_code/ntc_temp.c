#include "ntc_temp.h"
#include <math.h>
#include "adc.h" // 确保能引用到 ADC 的相关定义

/**
 * @brief  内部使用的数学计算函数 (Beta方程)
 * @param  adc_val: 滤波后的ADC平均值
 * @return 温度(摄氏度)
 */
static float NTC_Calculate_Celsius(uint16_t adc_val)
{
    float R_ntc;
    float temperature;

    // 1. 异常处理 (开路或短路)
    if (adc_val == 0 || adc_val >= ADC_RESOLUTION) {
        return -999.0f; 
    }

    // 2. 计算电阻 (电路: VCC --[20k]--o--[NTC]-- GND)
    // 如果你的电路反了，请修改这里
    R_ntc = SERIES_RESISTOR * ((float)adc_val / (ADC_RESOLUTION - (float)adc_val));

    // 3. 计算温度 (Kelvin -> Celsius)
    temperature = 1.0f / ( (1.0f / (NTC_NOMINAL_T + 273.15f)) + 
                           (1.0f / NTC_B_VALUE) * log(R_ntc / NTC_NOMINAL_R) );
    temperature -= 273.15f;

    return temperature;
}

/**
 * @brief  【核心功能】读取温度（包含采样、滤波、转换）
 * @param  hadc: ADC句柄指针，例如 &hadc1
 * @return float: 最终温度值
 */
float NTC_Read_Temperature(ADC_HandleTypeDef *hadc)
{
    uint32_t sum = 0;
    uint16_t avg_adc = 0;

    // 1. 连续采样 N 次取平均 (软件滤波)
    for(int i = 0; i < NTC_SAMPLE_COUNT; i++)
    {
        HAL_ADC_Start(hadc);
        
        // 等待转换完成 (超时时间 10ms)
        if (HAL_ADC_PollForConversion(hadc, 10) == HAL_OK)
        {
            sum += HAL_ADC_GetValue(hadc);
        }
        
        // 稍微延时一点点，避免采样太快读到同样的噪声 (可选)
        // HAL_Delay(1); 
    }

    // 2. 计算平均值
    avg_adc = (uint16_t)(sum / NTC_SAMPLE_COUNT);

    // 3. 调用数学公式转换
    return NTC_Calculate_Celsius(avg_adc);
}