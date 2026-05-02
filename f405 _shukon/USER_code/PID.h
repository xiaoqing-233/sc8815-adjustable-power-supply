#ifndef SC8815_H
#define SC8815_H

#include "main.h"
#include "sc8815.h"
#include <math.h>
#include <stdint.h>

// --- PID 结构体 (增量式) ---
typedef struct {
    float Kp;           // 比例系数
    float Ki;           // 积分系数
    float Target;       // 目标值 (用户设定的 V 或 I)
    float Measured;     // 实际值 (INA226 读回来的)
	  float Integral;
    float Last_Error;   // 上次误差
    float Output;       // PID 计算出的输出值 (这里统一对应 SC8815 的电压 DAC)
    float Out_Max;      // 输出上限
    float Out_Min;      // 输出下限
    float DeadBand;     // 死区 (防抖动)
} PID_Type;

// --- 电源全局状态 ---
typedef struct {
    float Set_Volts;    // 用户设定电压
    float Set_Amps;     // 用户设定电流
    
    float Real_Volts;   // INA226 读到的电压
    float Real_Curr;    // INA226 读到的电流
    
    uint8_t Is_Output_On; // 输出开关状态
    enum { MODE_CV, MODE_CC } Work_Mode; // 当前工作模式
} PSU_Status_Type;

// 全局变量声明
extern PID_Type Voltage_PID;
extern PID_Type Current_PID;
extern PSU_Status_Type PSU;

void PID_Init(void);
void PID_Calculate_Incremental(PID_Type *pid);
void PSU_Control_Loop(void);
void On_Target_Voltage_Changed(float new_volts);







#ifdef __cplusplus

#endif // __cplusplus
#endif // !SC8815_H





