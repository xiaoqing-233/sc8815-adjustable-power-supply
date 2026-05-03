#include "GUI.h"
#include "ui.h"
#include "tim.h"
#include "AT24C16.h"
#include "string.h"

extern SoftI2C_Bus Bus_AT;





void PSU_Set_Output_State(uint8_t state) //��Դ������ָ��
{
    // 1. �޸ĺ��ı��� (Model)
    PSU.Is_Output_On = state;



    // 3. ˢ�� UI (View)
    // ֻ����״̬�ı�ȷ���󣬲Ž� UI ȥ������ʾ
    Update_All_Power_UI();
}

void Update_All_Power_UI(void)
{
    // ��ȡ��ǰ����ʵ״̬
    uint8_t target_state = PSU.Is_Output_On; 

    // ================= Screen 1 ���� =================
    if (ui_PowerOpen != NULL && ui_Power != NULL) {
        
        // 1. ��������״̬ (�ؼ��޸ģ�)
        // ֻ�е� UI ��ǰ״̬ �� Ŀ��״̬ ��һ��ʱ����ȥǿ���޸�
        // ���������յ���˿��أ�UI״̬�Ѿ����ˣ��Ͳ��ᱻ��϶���
        if (target_state == 1) {
            if (!lv_obj_has_state(ui_PowerOpen, LV_STATE_CHECKED)) {
                lv_obj_add_state(ui_PowerOpen, LV_STATE_CHECKED);
            }
        } else {
            if (lv_obj_has_state(ui_PowerOpen, LV_STATE_CHECKED)) {
                lv_obj_clear_state(ui_PowerOpen, LV_STATE_CHECKED);
            }
        }

        // 2. �������ֺ���ɫ (�ⲿ��������ζ�Ҫˢ����Ϊ����û�ж�����ͻ)
        if (target_state == 1) {
            lv_label_set_text(ui_Power, "ON");
            lv_obj_set_style_text_color(ui_Power, lv_color_hex(0x00FF00), LV_PART_MAIN);
        } else {
            lv_label_set_text(ui_Power, "OFF");
            lv_obj_set_style_text_color(ui_Power, lv_color_hex(0x000000), LV_PART_MAIN);
        }
    }

    // ================= Screen 2 ���� =================
    // �߼�ͬ��
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

        // ���ֲ���
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

    // 1. ��ȡ�û��ѿ��ز��������� (1:��, 0:��)
    uint8_t user_target = lv_obj_has_state(sw, LV_STATE_CHECKED) ? 1 : 0;

    // 2. ֻ�е��û���ͼ �� ��ǰϵͳ״̬ ��һ��ʱ����ȥִ��
    // (�����жϷǳ���Ҫ����ֹ Update_All_Power_UI ˢ�¿���ʱ�����������ص���������ѭ��)
    if (user_target != PSU.Is_Output_On) {
        // �����ܿغ���
        PSU_Set_Output_State(user_target);
    }


}

// Screen 2 �Ŀ��ػص�
void Switch2_State_Changed(lv_event_t * e)
{
    lv_obj_t * sw = lv_event_get_target(e);

    // 1. ��ȡ�û��ѿ��ز��������� (1:��, 0:��)
    uint8_t user_target = lv_obj_has_state(sw, LV_STATE_CHECKED) ? 1 : 0;

    // 2. ֻ�е��û���ͼ �� ��ǰϵͳ״̬ ��һ��ʱ����ȥִ��
    // (�����жϷǳ���Ҫ����ֹ Update_All_Power_UI ˢ�¿���ʱ�����������ص���������ѭ��)
    if (user_target != PSU.Is_Output_On) {
        // �����ܿغ���
        PSU_Set_Output_State(user_target);
    }
}

void Sync_Hardware_To_UI(void)
{
    // ��ȡ SC8815 �Ĵ���������������û�����
    // ���� SC8815_IsOutputOn() ���� true ��ʾ�������
//    bool is_really_on = SC8815_IsOutputOn(); 

    // ֻ�е� UI ��ʾ��״̬��Ӳ����һ��ʱ��ˢ�£�������˸
		
//    if (is_really_on) {
//        if (!lv_obj_has_state(ui_PowerOpen, LV_STATE_CHECKED)) {
//            lv_obj_add_state(ui_PowerOpen, LV_STATE_CHECKED); // ǿ�ưѿ�������ȥ
//            lv_label_set_text(ui_Power, "ON");
//        }
//    } else {
//        if (lv_obj_has_state(ui_PowerOpen, LV_STATE_CHECKED)) {
//            lv_obj_clear_state(ui_PowerOpen, LV_STATE_CHECKED); // ǿ�ưѿ���������
//            lv_label_set_text(ui_Power, "OFF");
//        }
//    }
}





extern lv_indev_t * indev_keypad; 

// 保存电压电流配置到 AT24C16
static void Save_Config_To_EEPROM(void)
{
	uint8_t buf[4];
	memcpy(buf, &PSU.Set_Volts, 4);
	AT24C16_Write(&Bus_AT, EE_ADDR_SET_VOLTS, buf, 4);
	memcpy(buf, &PSU.Set_Amps, 4);
	AT24C16_Write(&Bus_AT, EE_ADDR_SET_AMPS, buf, 4);
	AT24C16_WriteByte(&Bus_AT, EE_ADDR_MAGIC, EE_MAGIC_VALUE);
}



// ������ӡ��������� main.c �� GUI.c �ﶨ���ȫ����
extern lv_group_t * group; 

// SLS ���Զ������������
void Focus_On_Screen1(lv_event_t * e)
{
    // �����
    lv_group_remove_all_objs(group);
    
    // �����ӵ�ǰҳ��İ�ť
    lv_group_add_obj(group, ui_Button1);
    
    lv_group_add_obj(group, ui_SpinboxVolt);
	  lv_group_add_obj(group, ui_SpinboxCurr);
	
//	  lv_group_add_obj(group, ui_PowerOpen);
    // ���۽�
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

//lv_obj_add_event_cb(ui_Screen1, Focus_On_Screen1, LV_EVENT_SCREEN_LOADED, NULL); �ӵ�screen12��
lv_chart_series_t * ui_Series_Volt; // ��ѹ�ߣ���ɫ��
lv_chart_series_t * ui_Series_Curr; // �����ߣ���������ɫ���ɫ��

lv_obj_t * ui_Label_Tip_Volt = NULL; //�����ǩ
lv_obj_t * ui_Label_Tip_Curr = NULL;

void My_Waveform_Init(void)
{
	
		lv_chart_set_point_count(ui_Chart1, 40);
			// ������ Y ��  ��Χ 0 ~ 30.00V (��Ӧ���� 3000)
		lv_chart_set_range(ui_Chart1, LV_CHART_AXIS_PRIMARY_Y, 0, 3600);

		// ���ø� Y �� 
		lv_chart_set_range(ui_Chart1, LV_CHART_AXIS_SECONDARY_Y, 0, 3200);
    // 1. ��ѹ�� (��ɫ) - ������ Y ��
    ui_Series_Volt = lv_chart_add_series(ui_Chart1, lv_color_hex(0xFF0000), LV_CHART_AXIS_PRIMARY_Y);
    
    // 2. �������������� (��ɫ) - ������ Y ��
    // �����������ɫ�Ա����ԣ��Ƽ� Cyan (0x00FFFF) �� Yellow (0xFFFF00)
    ui_Series_Curr = lv_chart_add_series(ui_Chart1, lv_color_hex(0x00FFFF), LV_CHART_AXIS_PRIMARY_Y);
    
    // 3. ����ˢ��ģʽ
    lv_chart_set_update_mode(ui_Chart1, LV_CHART_UPDATE_MODE_SHIFT);

    // �����顿���� Y �᷶Χ (Range)
    // �������� UI ��������Ѿ������ 0~3000 (��Ӧ 0~30.00V)������Ͳ��ö�
    // ���û�裬�����ô���ǿ����һ�£�
    // lv_chart_set_range(ui_Chart1, LV_CHART_AXIS_PRIMARY_Y, 0, 3000); 
		
		
		// 1. ������ѹ�����ǩ (͸������)
    ui_Label_Tip_Volt = lv_label_create(ui_Chart1);
    lv_obj_set_style_bg_opa(ui_Label_Tip_Volt, 0, 0); 
    lv_obj_set_style_text_color(ui_Label_Tip_Volt, lv_color_hex(0xFF0000), 0); 
    
    // ����ѡ��Ϊ�˷����ţ������ּ�һȦ��ɫ��� (������������ϻ�����)
    // lv_obj_set_style_text_outline_width(ui_Label_Tip_Volt, 2, 0);
    // lv_obj_set_style_text_outline_color(ui_Label_Tip_Volt, lv_color_hex(0x000000), 0);

    lv_label_set_text(ui_Label_Tip_Volt, "0.0V");
    lv_obj_align(ui_Label_Tip_Volt, LV_ALIGN_BOTTOM_RIGHT, -5, 0); // ��΢����һ��
    lv_obj_add_flag(ui_Label_Tip_Volt, LV_OBJ_FLAG_FLOATING); 


    // 2. �������������ǩ (͸������)
    ui_Label_Tip_Curr = lv_label_create(ui_Chart1);
    lv_obj_set_style_bg_opa(ui_Label_Tip_Curr, 0, 0);
    lv_obj_set_style_text_color(ui_Label_Tip_Curr, lv_color_hex(0x00FFFF), 0);
    
    lv_label_set_text(ui_Label_Tip_Curr, "0.0A");
    lv_obj_align(ui_Label_Tip_Curr, LV_ALIGN_BOTTOM_RIGHT, -5, 0);
    lv_obj_add_flag(ui_Label_Tip_Curr, LV_OBJ_FLAG_FLOATING);
		
		
		
				// 1. ��ֹ����Ļ����
		if (ui_Screen2 != NULL) {
			lv_obj_clear_flag(ui_Screen2, LV_OBJ_FLAG_SCROLLABLE);
		}

		// 2. ��� Spinbox ����ĳ�� Panel ����� ui_Panel10����ҲҪ��ֹ�Ǹ� Panel ����
		if (ui_Panel10 != NULL) {
			lv_obj_clear_flag(ui_Panel10, LV_OBJ_FLAG_SCROLLABLE);
			}
}

void Update_Main_Screen_Sensors(float voltage, float current, float power, float input_volts)
{
    char buffer[32]; // ����һ����ʱ���������ڴ���ַ���

    // 1. ˢ�µ�ѹ (ui_VoltVal)
    // "%.3fV" ��ʾ����3λС�������� 12.000V
    sprintf(buffer, "%.3fV", voltage);
    lv_label_set_text(ui_VoltVal, buffer);

    // 2. ˢ�µ��� (ui_CurrVal)
    // "%.3fA" ��ʾ����3λС��
    sprintf(buffer, "%.3fA", current);
    lv_label_set_text(ui_CurrVal, buffer);

    // 3. ˢ�¹��� (ui_PowerVal)
    // "%.2fW" ��ʾ����2λС��
    sprintf(buffer, "%.2fW", power);
    lv_label_set_text(ui_PowerVal, buffer);
    
   
    // 4. ˢ�������ѹ (ui_VinVal) - ������Ҳ���������ѹ
    sprintf(buffer, "%.0fV", input_volts);
    lv_label_set_text(ui_VinVal, buffer);
		lv_label_set_text(ui_VinVal2, buffer);
		
		
		
		// ================= ˢ�²���ͼ & �����ǩ =================
    if (ui_Chart1 != NULL && ui_Series_Volt != NULL && ui_Series_Curr != NULL) 
		{
        
        // 1. �������� (������� Chart ���õ�һ�£�)
        const int MAX_Y_RANGE = 3200; // ��Ӧ 35.00V
        const int CHART_HEIGHT = lv_obj_get_height(ui_Chart1); // ��̬��ȡ�߶�(170)

        // 2. ׼����ͼ����
        int v_plot = (int)(voltage * 100); 
        int i_plot = (int)(current * 100); // �������Ҳ�Ŵ�100��
				
        // 3. �������ݽ�ͼ��
        lv_chart_set_next_value(ui_Chart1, ui_Series_Volt, v_plot);
        lv_chart_set_next_value(ui_Chart1, ui_Series_Curr, i_plot);
        
        // -------------------------------------------------
        // 4. ���¸����ǩ��λ�� (�ؼ�����)
        // -------------------------------------------------
        int v_height_px = (v_plot * CHART_HEIGHT) / MAX_Y_RANGE;
        // --- ��ѹ��ǩ���� ---
        if (ui_Label_Tip_Volt != NULL) {
            // ��������
            lv_label_set_text_fmt(ui_Label_Tip_Volt, "%.2fV", voltage);
            
            // ����߶����� (������ 0 ~ 100% ֮�䣬��ֹ�ܳ���Ļ)
            if(v_height_px > CHART_HEIGHT) v_height_px = CHART_HEIGHT;
            
            // ����λ�ã�X��΢��(-50)��Ϊ�˲���ס���µ��ߣ�Y������Ǹ���(����)
            // ��ȥ 10 ��Ϊ���ñ�ǩ���Ķ��������������Ǳ�ǩ�ײ���������
            lv_obj_set_y(ui_Label_Tip_Volt, -v_height_px - 10); 
            lv_obj_set_x(ui_Label_Tip_Volt, -5); // �����ұ߿�
        }

        // --- ������ǩ���� ---
        if (ui_Label_Tip_Curr != NULL) {
            lv_label_set_text_fmt(ui_Label_Tip_Curr, "%.2fA", current);
            
            // ע�⣺�����ĵ������˸�Y�ᣬ���̿��ܲ�һ����������蹲����Y��
            int i_height_px = (i_plot * CHART_HEIGHT) / MAX_Y_RANGE;
             if(i_height_px > CHART_HEIGHT) i_height_px = CHART_HEIGHT;

            // Ϊ�˷�ֹ������ǩ�ص��������ѹ�����ܽӽ������Ը�������ǩX�����һ��
            lv_obj_set_y(ui_Label_Tip_Curr, -i_height_px - 10);
            
            // ��������߶Ȳ��(��ֵС��20����)���ѵ�����ǩ����ŲŲ����ֹ�ص�
            if (abs(v_height_px - i_height_px) < 20) {
                lv_obj_set_x(ui_Label_Tip_Curr, -60); // ������ʾ
            } else {
                lv_obj_set_x(ui_Label_Tip_Curr, -5);
            }
        }
    }
}

///////////////////////////////spinbox�ؼ�//////////////////////////////
// ��ѹ���ڻص�
void On_SpinboxVolt_Changed(lv_event_t * e)
{
    lv_obj_t * spinbox = lv_event_get_target(e);
    
    // 1. ��ȡ Spinbox ������ֵ (���� 500 ���� 5.00V)
    int32_t val_int = lv_spinbox_get_value(spinbox);
    
    // 2. ת��Ϊ������
    float new_volts = (float)val_int / 100.0f; // �����3λС���ͳ���1000
    
    // 3. ����ȫ�ֱ���
    PSU.Set_Volts = new_volts;
    
    // 流���浽 EEPROM
    Save_Config_To_EEPROM();
    
    On_Target_Voltage_Changed(new_volts); 
}

// �������ڻص�
void On_SpinboxCurr_Changed(lv_event_t * e)
{
    lv_obj_t * spinbox = lv_event_get_target(e);
    
    int32_t val_int = lv_spinbox_get_value(spinbox);
    float new_amps = (float)val_int / 100.0f; 
    
    PSU.Set_Amps = new_amps;
    
    // 流���浽 EEPROM
    Save_Config_To_EEPROM();
    
    Current_PID.Target = new_amps;
}

void Update_Temp_UI(float temp)
{
    // ��������
    lv_label_set_text_fmt(ui_TemVal, "%.0f", temp);
		lv_label_set_text_fmt(ui_TemVal2, "%.0f", temp);
    // ��ɫ�߼������� 60�� ���ɫ������ ��ɫ
    if (temp >= 60.0f) {
        lv_obj_set_style_text_color(ui_TemVal, lv_color_hex(0xFF0000), LV_PART_MAIN); // ��
    } else {
        // �ָ�ԭ������ɫ (����ԭ����ǳ��ɫ)
        lv_obj_set_style_text_color(ui_TemVal, lv_color_hex(0x10EE10), LV_PART_MAIN); 
    }
}




//////////////////////////////////* �����������ر��� *///////////////////////////////////////
static uint32_t buzzer_off_tick = 0; // ��¼Ԥ�ƹرյ�ʱ���
static uint8_t  buzzer_active = 0;   // ����������״̬���
/**
  * @brief  ��������ʼ������
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
    // --- 1. ͬ����ѹ�趨ֵ (Float -> Int) ---
    // ���� Spinbox ������ 2 λС��������Ҫ * 100
    int32_t val_v = (int32_t)(PSU.Set_Volts * 100.0f);
    
    if(ui_Screen1 && ui_SpinboxVolt)  lv_spinbox_set_value(ui_SpinboxVolt, val_v);
    if(ui_Screen2 && ui_SpinboxVolt2) lv_spinbox_set_value(ui_SpinboxVolt2, val_v);

    // --- 2. ͬ�������趨ֵ ---
    int32_t val_i = (int32_t)(PSU.Set_Amps * 100.0f);
    
    if(ui_Screen1 && ui_SpinboxCurr)  lv_spinbox_set_value(ui_SpinboxCurr, val_i);
    if(ui_Screen2 && ui_SpinboxCurr2) lv_spinbox_set_value(ui_SpinboxCurr2, val_i);
    
    // --- 3. (��ѡ) ͬ������״̬ ---
    if(ui_PowerOpen) {
        if(PSU.Is_Output_On) lv_obj_add_state(ui_PowerOpen, LV_STATE_CHECKED);
        else lv_obj_clear_state(ui_PowerOpen, LV_STATE_CHECKED);
    }
}
void On_Spinbox_Changed(lv_event_t * e)
{
    lv_obj_t * target = lv_event_get_target(e);
    int32_t val = lv_spinbox_get_value(target);

    // --- ��ѹ���� ---
    if(target == ui_SpinboxVolt || target == ui_SpinboxVolt2) 
    {
        // 1. ����ȫ�ֽṹ�� (Int -> Float)
        PSU.Set_Volts = (float)val / 100.0f; 
        
        // 2. �������µ�ѹ���� PID ������ / SC8815
        // SC8815_SetOutputVoltage(val * 10); // �����ĵײ㺯��Ҫ mv
        // PID_Set_Target_V(g_PSU.Set_Volts);
    }
    // --- �������� ---
    else if(target == ui_SpinboxCurr || target == ui_SpinboxCurr2) 
    {
        PSU.Set_Amps = (float)val / 100.0f;
    }
    
    // 流���浽 EEPROM
    Save_Config_To_EEPROM();
}











/**
  * @brief  ��������������
  * @note   ���ô˺����󣬷��������������𣬲���ָ��ʱ����Զ��ر�
  * @param  None
  */
void BUZZER_ShortBeep(void)
{
    // 1. ���� PWM ���
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
    
    // 2. ���״̬Ϊ������
    buzzer_active = 1;
    
    // 3. �趨�ر�ʱ�� = ��ǰϵͳ�δ� + 100ms
    buzzer_off_tick = HAL_GetTick() + 100;
}

/**
  * @brief  ����������������
  * @note   ������� main������ while(1) ѭ���е��ô˺���
  */
void BUZZER_Task_Handler(void)
{
    // �����������ǰû�����죬ֱ�ӷ��أ����˷�CPU
    if (buzzer_active == 0)
    {
        return;
    }

    // ��鵱ǰϵͳʱ���Ƿ��Ѿ��������趨�Ĺر�ʱ��
    if (HAL_GetTick() >= buzzer_off_tick)
    {
        // ʱ�䵽�ˣ��ر� PWM
        HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);
        
        // ���״̬���
        buzzer_active = 0;
    }
}



