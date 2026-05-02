#include "main.h"
#include "tim.h"
#include "lvgl.h"
#include "usart.h"
#include "PID.h"
#include <stdlib.h>

volatile uint8_t tim_num = 0;  //lvgl任务时间戳
volatile uint8_t PID_tim = 0;  //pid计算时间戳
volatile uint16_t Update_lcd_tim = 0;  //pid计算时间戳

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM12)
	{
		lv_tick_inc(1); // 告诉LVGL：时间过了1ms
		tim_num++;
		Update_lcd_tim++;
		
	}
	if(htim->Instance == TIM11)
	{
		PID_tim++;
		
	}
	
}


#define RX_BUFFER_SIZE 32  
uint8_t rx_one;
uint8_t Rx_state = 0;
uint8_t RxIndex = 0;

uint8_t RxBuffer[RX_BUFFER_SIZE]; 


extern float V_OFFSET ;

void Parse_PID_Command(char* cmd)
{
    PID_Type *target_pid = NULL;
    char loop_char = cmd[1];  // 'V' or 'C'
    char param_char = cmd[2]; // 'P', 'I', 'D', etc.
    float value = atof(&cmd[3]); // 从第3个字符开始转换浮点数

    // 1. 确定是哪个环
    if (loop_char == 'V') {
        target_pid = &Voltage_PID;
    } else if (loop_char == 'C') {
        target_pid = &Current_PID;
    } else {
        return; // 无效回路
    }

    // 2. 更新参数
    switch (param_char) {
        case 'P': // Kp
            target_pid->Kp = value;
						HAL_UART_Transmit(&huart4, (uint8_t*)"OK!/r/n", 6, 100);
            break;
        case 'I': // Ki
            target_pid->Ki = value;
						HAL_UART_Transmit(&huart4, (uint8_t*)"OK!/r/n", 6, 100);
            break;
        case 'D': // DeadBand
            target_pid->DeadBand = value;
						HAL_UART_Transmit(&huart4, (uint8_t*)"OK!/r/n", 6, 100);
            break;
        case 'M': // Out_Max
            target_pid->Out_Max = value;
						HAL_UART_Transmit(&huart4, (uint8_t*)"OK!/r/n", 6, 100);
            break;
        case 'N': // Out_Min
            target_pid->Out_Min = value;
						HAL_UART_Transmit(&huart4, (uint8_t*)"OK!/r/n", 6, 100);
            break;
        case 'T': // Target (目标值调试)
             // 注意：如果是Target，可能需要更新 PSU.Set_Volts 或 Set_Amps，
             // 这里仅直接改PID结构体，实际逻辑中建议改 PSU 结构体
            target_pid->Target = value;
            if(loop_char == 'V') PSU.Set_Volts = value;
            if(loop_char == 'C') PSU.Set_Amps = value;
            // 触发电压改变事件逻辑(如果需要)
            if(loop_char == 'V') On_Target_Voltage_Changed(value);
						HAL_UART_Transmit(&huart4, (uint8_t*)"OK!/r/n", 6, 100);
            break;
				case 'O': 
            if (loop_char == 'V') {
                V_OFFSET = value; // 更新全局变量
								HAL_UART_Transmit(&huart4, (uint8_t*)"OK!/r/n", 6, 100);
            }
            break;
        default:
            return; // 无效参数
    }

    // 3. (可选) 回显确认，方便知道修改成功了
    // 注意：不要在中断里做耗时打印，这里为了简单直接写了，
    // 实际工程建议置标志位在主循环打印。
    // printf("Set %c-%c to %.4f\r\n", loop_char, param_char, value);
}

/**
 * @brief 串口中断接收处理状态机
 */
void ReceiveData_proc(uint8_t RxData)
{
    // 协议格式: # [V/C] [P/I/D] [Value] !
    // 示例: #VP15.0!
    
    if (Rx_state == 0) // 等待包头
    {
        if (RxData == '#') 
        {
            RxIndex = 0;
            memset(RxBuffer, 0, RX_BUFFER_SIZE); // 清空缓存
            RxBuffer[RxIndex++] = RxData;
            Rx_state = 1;
        }
    }
    else if (Rx_state == 1) // 接收数据
    {
        // 防止缓冲区溢出
        if (RxIndex >= RX_BUFFER_SIZE - 1) {
            Rx_state = 0;
            RxIndex = 0;
            return;
        }

        RxBuffer[RxIndex++] = RxData;

        if (RxData == '!') // 检测包尾
        {
            // 收到完整包，进行解析
            RxBuffer[RxIndex] = '\0'; // 添加字符串结束符
            Parse_PID_Command((char*)RxBuffer);
            
            // 复位状态
            Rx_state = 0;
            RxIndex = 0;
        }
    }
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == UART4) // 确保是串口1
    {
        // 处理接收到的这一个字节
        ReceiveData_proc(rx_one);
        
        // 重新开启中断接收下一个字节
        HAL_UART_Receive_IT(&huart4, &rx_one, 1);
    }
}
