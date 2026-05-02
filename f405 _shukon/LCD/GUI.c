#include "GUI.h"
#include "ui.h"
#include "tim.h"





void PSU_Set_Output_State(uint8_t state) //电源开关总指挥
{
    // 1. 修改核心变量 (Model)
    PSU.Is_Output_On = state;



    // 3. 刷新 UI (View)
    // 只有在状态改变确立后，才叫 UI 去更新显示
    Update_All_Power_UI();
}

void Update_All_Power_UI(void)
{
    // 获取当前的真实状态
    uint8_t target_state = PSU.Is_Output_On; 

    // ================= Screen 1 更新 =================
    if (ui_PowerOpen != NULL && ui_Power != NULL) {
        
        // 1. 处理开关状态 (关键修改！)
        // 只有当 UI 当前状态 和 目标状态 不一致时，才去强制修改
        // 这样如果你刚点击了开关，UI状态已经对了，就不会被打断动画
        if (target_state == 1) {
            if (!lv_obj_has_state(ui_PowerOpen, LV_STATE_CHECKED)) {
                lv_obj_add_state(ui_PowerOpen, LV_STATE_CHECKED);
            }
        } else {
            if (lv_obj_has_state(ui_PowerOpen, LV_STATE_CHECKED)) {
                lv_obj_clear_state(ui_PowerOpen, LV_STATE_CHECKED);
            }
        }

        // 2. 处理文字和颜色 (这部分无论如何都要刷，因为文字没有动画冲突)
        if (target_state == 1) {
            lv_label_set_text(ui_Power, "ON");
            lv_obj_set_style_text_color(ui_Power, lv_color_hex(0x00FF00), LV_PART_MAIN);
        } else {
            lv_label_set_text(ui_Power, "OFF");
            lv_obj_set_style_text_color(ui_Power, lv_color_hex(0x000000), LV_PART_MAIN);
        }
    }

    // ================= Screen 2 更新 =================
    // 逻辑同上
    if (ui_PowerOpen2 != NULL && ui_Power2 != NULL) {
        if (target_state == 1) {
            if (!lv_obj_has_state(ui_PowerOpen2, LV_STATE_CHECKED)) {
                lv_obj_add_state(ui_PowerOpen2, LV_STATE_CHECKED);
            }
        } else {
            if (lv_obj_has_state(ui_PowerOpen2, LV_STATE_CHECKED)) {
                lv_obj_clear_state(ui_PowerOpen2, LV_STATE_CHECKED);
            }
        }

        // 文字部分
        if (target_state == 1) {
            lv_label_set_text(ui_Power2, "ON");
            lv_obj_set_style_text_color(ui_Power2, lv_color_hex(0x00FF00), LV_PART_MAIN);
        } else {
            lv_label_set_text(ui_Power2, "OFF");
            lv_obj_set_style_text_color(ui_Power2, lv_color_hex(0x000000), LV_PART_MAIN);
        }
    }
}

void Switch_State_Changed(lv_event_t * e)
{
    lv_obj_t * sw = lv_event_get_target(e);

    // 1. 获取用户把开关拨到了哪里 (1:开, 0:关)
    uint8_t user_target = lv_obj_has_state(sw, LV_STATE_CHECKED) ? 1 : 0;

    // 2. 只有当用户意图 和 当前系统状态 不一致时，才去执行
    // (这行判断非常重要，防止 Update_All_Power_UI 刷新开关时反过来触发回调，导致死循环)
    if (user_target != PSU.Is_Output_On) {
        // 调用总控函数
        PSU_Set_Output_State(user_target);
    }


}

// Screen 2 的开关回调
void Switch2_State_Changed(lv_event_t * e)
{
    lv_obj_t * sw = lv_event_get_target(e);

    // 1. 获取用户把开关拨到了哪里 (1:开, 0:关)
    uint8_t user_target = lv_obj_has_state(sw, LV_STATE_CHECKED) ? 1 : 0;

    // 2. 只有当用户意图 和 当前系统状态 不一致时，才去执行
    // (这行判断非常重要，防止 Update_All_Power_UI 刷新开关时反过来触发回调，导致死循环)
    if (user_target != PSU.Is_Output_On) {
        // 调用总控函数
        PSU_Set_Output_State(user_target);
    }
}

void Sync_Hardware_To_UI(void)
{
    // 读取 SC8815 寄存器，看它到底有没有输出
    // 假设 SC8815_IsOutputOn() 返回 true 表示正在输出
//    bool is_really_on = SC8815_IsOutputOn(); 

    // 只有当 UI 显示的状态和硬件不一致时才刷新，避免闪烁
		
//    if (is_really_on) {
//        if (!lv_obj_has_state(ui_PowerOpen, LV_STATE_CHECKED)) {
//            lv_obj_add_state(ui_PowerOpen, LV_STATE_CHECKED); // 强制把开关推上去
//            lv_label_set_text(ui_Power, "ON");
//        }
//    } else {
//        if (lv_obj_has_state(ui_PowerOpen, LV_STATE_CHECKED)) {
//            lv_obj_clear_state(ui_PowerOpen, LV_STATE_CHECKED); // 强制把开关拉下来
//            lv_label_set_text(ui_Power, "OFF");
//        }
//    }
}





extern lv_indev_t * indev_keypad; 



// 【必须加】引用你在 main.c 或 GUI.c 里定义的全局组
extern lv_group_t * group; 

// SLS 会自动调用这个函数
void Focus_On_Screen1(lv_event_t * e)
{
    // 先清空
    lv_group_remove_all_objs(group);
    
    // 再添加当前页面的按钮
    lv_group_add_obj(group, ui_Button1);
    
    lv_group_add_obj(group, ui_SpinboxVolt);
	  lv_group_add_obj(group, ui_SpinboxCurr);
	
//	  lv_group_add_obj(group, ui_PowerOpen);
    // 最后聚焦
    lv_group_focus_obj(ui_Button1);
	
		Sync_Struct_To_UI();
}

void Focus_On_Screen2(lv_event_t * e)
{
    lv_group_remove_all_objs(group);
    lv_group_add_obj(group, ui_Button2);
	  lv_group_add_obj(group, ui_SpinboxVolt2);
	  lv_group_add_obj(group, ui_SpinboxCurr2);
//		lv_group_add_obj(group, ui_PowerOpen2);
    lv_group_focus_obj(ui_Button2);
		
		Sync_Struct_To_UI();
}

//lv_obj_add_event_cb(ui_Screen1, Focus_On_Screen1, LV_EVENT_SCREEN_LOADED, NULL); 加到screen12中
lv_chart_series_t * ui_Series_Volt; // 电压线（红色）
lv_chart_series_t * ui_Series_Curr; // 电流线（建议用青色或黄色）

lv_obj_t * ui_Label_Tip_Volt = NULL; //跟随标签
lv_obj_t * ui_Label_Tip_Curr = NULL;

void My_Waveform_Init(void)
{
	
		lv_chart_set_point_count(ui_Chart1, 40);
			// 设置主 Y 轴  范围 0 ~ 30.00V (对应整数 3000)
		lv_chart_set_range(ui_Chart1, LV_CHART_AXIS_PRIMARY_Y, 0, 3600);

		// 设置副 Y 轴 
		lv_chart_set_range(ui_Chart1, LV_CHART_AXIS_SECONDARY_Y, 0, 3200);
    // 1. 电压线 (红色) - 接在主 Y 轴
    ui_Series_Volt = lv_chart_add_series(ui_Chart1, lv_color_hex(0xFF0000), LV_CHART_AXIS_PRIMARY_Y);
    
    // 2. 【新增】电流线 (青色) - 接在主 Y 轴
    // 如果你想让颜色对比明显，推荐 Cyan (0x00FFFF) 或 Yellow (0xFFFF00)
    ui_Series_Curr = lv_chart_add_series(ui_Chart1, lv_color_hex(0x00FFFF), LV_CHART_AXIS_PRIMARY_Y);
    
    // 3. 设置刷新模式
    lv_chart_set_update_mode(ui_Chart1, LV_CHART_UPDATE_MODE_SHIFT);

    // 【建议】设置 Y 轴范围 (Range)
    // 假设你在 UI 设计器里已经设好了 0~3000 (对应 0~30.00V)，这里就不用动
    // 如果没设，可以用代码强制设一下：
    // lv_chart_set_range(ui_Chart1, LV_CHART_AXIS_PRIMARY_Y, 0, 3000); 
		
		
		// 1. 创建电压跟随标签 (透明背景)
    ui_Label_Tip_Volt = lv_label_create(ui_Chart1);
    lv_obj_set_style_bg_opa(ui_Label_Tip_Volt, 0, 0); 
    lv_obj_set_style_text_color(ui_Label_Tip_Volt, lv_color_hex(0xFF0000), 0); 
    
    // 【可选】为了防干扰，给文字加一圈黑色描边 (如果在网格线上会更清楚)
    // lv_obj_set_style_text_outline_width(ui_Label_Tip_Volt, 2, 0);
    // lv_obj_set_style_text_outline_color(ui_Label_Tip_Volt, lv_color_hex(0x000000), 0);

    lv_label_set_text(ui_Label_Tip_Volt, "0.0V");
    lv_obj_align(ui_Label_Tip_Volt, LV_ALIGN_BOTTOM_RIGHT, -5, 0); // 稍微靠左一点
    lv_obj_add_flag(ui_Label_Tip_Volt, LV_OBJ_FLAG_FLOATING); 


    // 2. 创建电流跟随标签 (透明背景)
    ui_Label_Tip_Curr = lv_label_create(ui_Chart1);
    lv_obj_set_style_bg_opa(ui_Label_Tip_Curr, 0, 0);
    lv_obj_set_style_text_color(ui_Label_Tip_Curr, lv_color_hex(0x00FFFF), 0);
    
    lv_label_set_text(ui_Label_Tip_Curr, "0.0A");
    lv_obj_align(ui_Label_Tip_Curr, LV_ALIGN_BOTTOM_RIGHT, -5, 0);
    lv_obj_add_flag(ui_Label_Tip_Curr, LV_OBJ_FLAG_FLOATING);
		
		
		
				// 1. 禁止主屏幕滚动
		if (ui_Screen2 != NULL) {
			lv_obj_clear_flag(ui_Screen2, LV_OBJ_FLAG_SCROLLABLE);
		}

		// 2. 如果 Spinbox 放在某个 Panel 里（比如 ui_Panel10），也要禁止那个 Panel 滚动
		if (ui_Panel10 != NULL) {
			lv_obj_clear_flag(ui_Panel10, LV_OBJ_FLAG_SCROLLABLE);
			}
}

void Update_Main_Screen_Sensors(float voltage, float current, float power, float input_volts)
{
    char buffer[32]; // 定义一个临时缓冲区用于存放字符串

    // 1. 刷新电压 (ui_VoltVal)
    // "%.3fV" 表示保留3位小数，例如 12.000V
    sprintf(buffer, "%.3fV", voltage);
    lv_label_set_text(ui_VoltVal, buffer);

    // 2. 刷新电流 (ui_CurrVal)
    // "%.3fA" 表示保留3位小数
    sprintf(buffer, "%.3fA", current);
    lv_label_set_text(ui_CurrVal, buffer);

    // 3. 刷新功率 (ui_PowerVal)
    // "%.2fW" 表示保留2位小数
    sprintf(buffer, "%.2fW", power);
    lv_label_set_text(ui_PowerVal, buffer);
    
   
    // 4. 刷新输入电压 (ui_VinVal) - 假设你也读了输入电压
    sprintf(buffer, "%.0fV", input_volts);
    lv_label_set_text(ui_VinVal, buffer);
		lv_label_set_text(ui_VinVal2, buffer);
		
		
		
		// ================= 刷新波形图 & 跟随标签 =================
    if (ui_Chart1 != NULL && ui_Series_Volt != NULL && ui_Series_Curr != NULL) 
		{
        
        // 1. 定义量程 (必须和你 Chart 设置的一致！)
        const int MAX_Y_RANGE = 3200; // 对应 35.00V
        const int CHART_HEIGHT = lv_obj_get_height(ui_Chart1); // 动态获取高度(170)

        // 2. 准备绘图数据
        int v_plot = (int)(voltage * 100); 
        int i_plot = (int)(current * 100); // 假设电流也放大100倍
				
        // 3. 推送数据进图表
        lv_chart_set_next_value(ui_Chart1, ui_Series_Volt, v_plot);
        lv_chart_set_next_value(ui_Chart1, ui_Series_Curr, i_plot);
        
        // -------------------------------------------------
        // 4. 更新跟随标签的位置 (关键步骤)
        // -------------------------------------------------
        int v_height_px = (v_plot * CHART_HEIGHT) / MAX_Y_RANGE;
        // --- 电压标签处理 ---
        if (ui_Label_Tip_Volt != NULL) {
            // 更新文字
            lv_label_set_text_fmt(ui_Label_Tip_Volt, "%.2fV", voltage);
            
            // 计算高度像素 (限制在 0 ~ 100% 之间，防止跑出屏幕)
            if(v_height_px > CHART_HEIGHT) v_height_px = CHART_HEIGHT;
            
            // 设置位置：X轴微调(-50)是为了不挡住最新的线，Y轴必须是负数(向上)
            // 减去 10 是为了让标签中心对齐线条，而不是标签底部对齐线条
            lv_obj_set_y(ui_Label_Tip_Volt, -v_height_px - 10); 
            lv_obj_set_x(ui_Label_Tip_Volt, -5); // 紧贴右边框
        }

        // --- 电流标签处理 ---
        if (ui_Label_Tip_Curr != NULL) {
            lv_label_set_text_fmt(ui_Label_Tip_Curr, "%.2fA", current);
            
            // 注意：如果你的电流用了副Y轴，量程可能不一样，这里假设共用主Y轴
            int i_height_px = (i_plot * CHART_HEIGHT) / MAX_Y_RANGE;
             if(i_height_px > CHART_HEIGHT) i_height_px = CHART_HEIGHT;

            // 为了防止两个标签重叠，如果电压电流很接近，可以给电流标签X轴多移一点
            lv_obj_set_y(ui_Label_Tip_Curr, -i_height_px - 10);
            
            // 如果两个高度差不多(差值小于20像素)，把电流标签往左挪挪，防止重叠
            if (abs(v_height_px - i_height_px) < 20) {
                lv_obj_set_x(ui_Label_Tip_Curr, -60); // 错开显示
            } else {
                lv_obj_set_x(ui_Label_Tip_Curr, -5);
            }
        }
    }
}

///////////////////////////////spinbox控件//////////////////////////////
// 电压调节回调
void On_SpinboxVolt_Changed(lv_event_t * e)
{
    lv_obj_t * spinbox = lv_event_get_target(e);
    
    // 1. 获取 Spinbox 的整数值 (例如 500 代表 5.00V)
    int32_t val_int = lv_spinbox_get_value(spinbox);
    
    // 2. 转换为浮点数
    float new_volts = (float)val_int / 100.0f; // 如果是3位小数就除以1000
    
    // 3. 更新全局变量
    PSU.Set_Volts = new_volts;
    
    // 4. 立即通知硬件修改电压 (调用你之前写的函数)
    // 这一步非常重要，否则界面变了，实际输出没变
    On_Target_Voltage_Changed(new_volts); 
}

// 电流调节回调
void On_SpinboxCurr_Changed(lv_event_t * e)
{
    lv_obj_t * spinbox = lv_event_get_target(e);
    
    int32_t val_int = lv_spinbox_get_value(spinbox);
    float new_amps = (float)val_int / 100.0f; 
    
    PSU.Set_Amps = new_amps;
    
    // 這裡通常不需要像电压那样清除PID，直接更新目标值即可
    Current_PID.Target = new_amps;
}

void Update_Temp_UI(float temp)
{
    // 更新文字
    lv_label_set_text_fmt(ui_TemVal, "%.0f", temp);
		lv_label_set_text_fmt(ui_TemVal2, "%.0f", temp);
    // 变色逻辑：超过 60度 变红色，否则 绿色
    if (temp >= 60.0f) {
        lv_obj_set_style_text_color(ui_TemVal, lv_color_hex(0xFF0000), LV_PART_MAIN); // 红
    } else {
        // 恢复原来的颜色 (假设原来是浅绿色)
        lv_obj_set_style_text_color(ui_TemVal, lv_color_hex(0x10EE10), LV_PART_MAIN); 
    }
}




//////////////////////////////////* 定义蜂鸣器相关变量 *///////////////////////////////////////
static uint32_t buzzer_off_tick = 0; // 记录预计关闭的时间点
static uint8_t  buzzer_active = 0;   // 蜂鸣器运行状态标记
/**
  * @brief  蜂鸣器初始化函数
  * @note  
  */
void BUZZER_Init(void)
{
	__HAL_TIM_SET_AUTORELOAD(&htim3,250-1);
	__HAL_TIM_SET_PRESCALER(&htim3,200-1);
	__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,50-1);
	
	
	
	HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_3);
}



void Sync_Struct_To_UI(void)
{
    // --- 1. 同步电压设定值 (Float -> Int) ---
    // 假设 Spinbox 设置了 2 位小数，所以要 * 100
    int32_t val_v = (int32_t)(PSU.Set_Volts * 100.0f);
    
    if(ui_Screen1 && ui_SpinboxVolt)  lv_spinbox_set_value(ui_SpinboxVolt, val_v);
    if(ui_Screen2 && ui_SpinboxVolt2) lv_spinbox_set_value(ui_SpinboxVolt2, val_v);

    // --- 2. 同步电流设定值 ---
    int32_t val_i = (int32_t)(PSU.Set_Amps * 100.0f);
    
    if(ui_Screen1 && ui_SpinboxCurr)  lv_spinbox_set_value(ui_SpinboxCurr, val_i);
    if(ui_Screen2 && ui_SpinboxCurr2) lv_spinbox_set_value(ui_SpinboxCurr2, val_i);
    
    // --- 3. (可选) 同步开关状态 ---
    if(ui_PowerOpen) {
        if(PSU.Is_Output_On) lv_obj_add_state(ui_PowerOpen, LV_STATE_CHECKED);
        else lv_obj_clear_state(ui_PowerOpen, LV_STATE_CHECKED);
    }
}
void On_Spinbox_Changed(lv_event_t * e)
{
    lv_obj_t * target = lv_event_get_target(e);
    int32_t val = lv_spinbox_get_value(target);

    // --- 电压调节 ---
    if(target == ui_SpinboxVolt || target == ui_SpinboxVolt2) 
    {
        // 1. 更新全局结构体 (Int -> Float)
        PSU.Set_Volts = (float)val / 100.0f; 
        
        // 2. 立即把新电压告诉 PID 控制器 / SC8815
        // SC8815_SetOutputVoltage(val * 10); // 如果你的底层函数要 mv
        // PID_Set_Target_V(g_PSU.Set_Volts);
    }
    // --- 电流调节 ---
    else if(target == ui_SpinboxCurr || target == ui_SpinboxCurr2) 
    {
        PSU.Set_Amps = (float)val / 100.0f;
        // PID_Set_Target_I(g_PSU.Set_Amps);
    }
}











/**
  * @brief  触发蜂鸣器短响
  * @note   调用此函数后，蜂鸣器会立即响起，并在指定时间后自动关闭
  * @param  None
  */
void BUZZER_ShortBeep(void)
{
    // 1. 开启 PWM 输出
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
    
    // 2. 标记状态为正在响
    buzzer_active = 1;
    
    // 3. 设定关闭时间 = 当前系统滴答 + 100ms
    buzzer_off_tick = HAL_GetTick() + 100;
}

/**
  * @brief  蜂鸣器任务处理函数
  * @note   请务必在 main函数的 while(1) 循环中调用此函数
  */
void BUZZER_Task_Handler(void)
{
    // 如果蜂鸣器当前没有在响，直接返回，不浪费CPU
    if (buzzer_active == 0)
    {
        return;
    }

    // 检查当前系统时间是否已经超过了设定的关闭时间
    if (HAL_GetTick() >= buzzer_off_tick)
    {
        // 时间到了，关闭 PWM
        HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);
        
        // 清除状态标记
        buzzer_active = 0;
    }
}



