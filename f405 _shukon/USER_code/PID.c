#include "PID.h"
#include "sc8815.h"
#include "main.h"
#include "math.h"
#include "string.h"
#include "stdio.h"
#include "usart.h"


extern float OutputVoltage; 
extern float OutputCurrent;
extern float OutputPower  ;





void PID_Init(void) {
    // --- 电压环参数 ---
    // 电压需要响应快，但 SC8815 自身很快，所以这里主要是消除误差
    Voltage_PID.Kp = 0.005f; //0.005
    Voltage_PID.Ki = 0.01f;  //0.01
    Voltage_PID.DeadBand = 0.015f; // 20mV 死区
    Voltage_PID.Out_Max = 2.0f;  // 补偿范围限制在 +/- 2V 即可
    Voltage_PID.Out_Min = -2.0f; //  必须允许负向补偿
    // --- 电流环参数 ---
    // 电流环通常要反应慢一点，防止和电压环打架震荡
    Current_PID.Kp = 0.5f; // 0.5
    Current_PID.Ki = 0.5f; // 0.5
    Current_PID.DeadBand = 0.002f; // 2mA 死区
		Current_PID.Out_Max = 36.5f;   // 
    Current_PID.Out_Min = -10.0f;  // SC8815 最低输出  -10.0
	
	// 这样在 CV 模式下，电流环输出 36V，电压环输出 12V。
    // min(12, 36) = 12V。电压瞬间到位，没有任何阻碍。
	  Current_PID.Integral = Current_PID.Out_Max;
    Current_PID.Output   = Current_PID.Out_Max;
}

void PID_Calculate_Incremental(PID_Type *pid) {
    // 1. 计算误差
    float error = pid->Target - pid->Measured;

    // 2. 死区控制
    if (fabsf(error) < pid->DeadBand) {
        error = 0.0f;
    }

        pid->Integral += (pid->Ki * error);

    if (pid->Integral > pid->Out_Max) pid->Integral = pid->Out_Max;
    if (pid->Integral < pid->Out_Min) pid->Integral = pid->Out_Min;

    // 4. 计算 P 项
    float p_term = pid->Kp * error;

    pid->Output = p_term + pid->Integral;

    // 总输出限幅
    if (pid->Output > pid->Out_Max) pid->Output = pid->Out_Max;
    if (pid->Output < pid->Out_Min) pid->Output = pid->Out_Min;

    // 7. 记录误差 (位置式其实不需要 Last_Error，除非你要加 D 项)
    pid->Last_Error = error;
}




float V_OFFSET = 0.005f; // 一点偏移量，稍微好看一些

void PSU_Control_Loop(void) {
   
		
    Voltage_PID.Target = PSU.Set_Volts + V_OFFSET;
    Current_PID.Target = PSU.Set_Amps;
	
	
	
		Voltage_PID.Measured = PSU.Real_Volts; 
    Current_PID.Measured = PSU.Real_Curr;
	
	// 用于记录上一次的状态，防止重复发送硬件指令
    static uint8_t last_output_state = 0;
    
    // ============================================================
    // 3. 核心修改：前馈 + PID 计算
    // ============================================================
    
    if (PSU.Is_Output_On) {
			
			
			 if(last_output_state == 0) 
			 {
             SC8815_EnableOutput(1); // 你的硬件开启函数
             last_output_state = 1;
            
       }
        
        // --- A. 正常计算 PID (计算误差补偿量) ---
        // 注意：这里的 PID 输出限幅要改小！
        // 因为 PID 现在只负责修补误差，不需要输出 30V
        // 建议限幅在 +/- 2.0V 左右
        PID_Calculate_Incremental(&Voltage_PID); // 计算出的 Output 是补偿值
        PID_Calculate_Incremental(&Current_PID);
        
        // --- B. 叠加前馈量 (Feedforward) ---
        // 理论输出 = 设定电压 + 电压环PID补偿
        float Volts_With_Feedforward = PSU.Set_Volts + Voltage_PID.Output ;
        
        // --- C. 竞争仲裁 ---
        float Final_Volts_Target;

        // 这里的 Current_PID.Output 依然代表恒流所需的电压
        // 但恒流环通常不需要前馈，或者以前一时刻电压为基准
        
        if (Volts_With_Feedforward < Current_PID.Output) {
            // CV 模式
            Final_Volts_Target = Volts_With_Feedforward;
            PSU.Work_Mode = MODE_CV;
            
           Current_PID.Integral = Final_Volts_Target + 2.0f; 
            
            // 防止超出物理极限
            if(Current_PID.Integral > Current_PID.Out_Max) Current_PID.Integral = Current_PID.Out_Max;
            
            // 同步修改 Output
           Current_PID.Output = Current_PID.Integral;
					
        } else {
            // CC 模式
            Final_Volts_Target = Current_PID.Output;
            PSU.Work_Mode = MODE_CC;
            
            Voltage_PID.Integral = Final_Volts_Target - PSU.Set_Volts;
            
           
            if(Voltage_PID.Integral > Voltage_PID.Out_Max) Voltage_PID.Integral = Voltage_PID.Out_Max;
            if(Voltage_PID.Integral < Voltage_PID.Out_Min) Voltage_PID.Integral = Voltage_PID.Out_Min;
					
            Voltage_PID.Output = Final_Volts_Target - PSU.Set_Volts;
        }

        // --- D. 硬件执行 ---
        uint16_t target_mv = (uint16_t)(Final_Volts_Target * 1000.0f);
        SC8815_SetOutputVoltage(target_mv);
        
    }
		else // 关断输出
		{
			if(last_output_state == 1) 
        {

            SC8815_EnableOutput(0); 
            
            Voltage_PID.Integral = 0;
            Voltage_PID.Output = 0;
            

            Current_PID.Integral = Current_PID.Out_Max;
						Current_PID.Output   = Current_PID.Out_Max;
           
            last_output_state = 0;
            
			
				}
		}
		
		
}






// 当用户改变 Set_Volts 时调用此函数
void On_Target_Voltage_Changed(float new_volts)
{
    PSU.Set_Volts = new_volts;
    
    // 关键：清空 PID 的积分项和输出
    // 因为我们要靠前馈直接跳过去，不需要 PID 存留的历史数据
    Voltage_PID.Output = 0.0f;     // 补偿值归零
    Voltage_PID.Integral = 0.0f;   // 积分归零
    Voltage_PID.Last_Error = 0.0f; // 误差归零
    
    // 如果想要极致响应，可以在这里直接调用一次硬件设置
    // 这样不用等下一个 PID 周期，电压立马就变
//    uint16_t target_mv = (uint16_t)(new_volts * 1000.0f);
//    SC8815_SetOutputVoltage(target_mv);
}



















