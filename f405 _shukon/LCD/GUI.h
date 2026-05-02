#include "main.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "PID.h"
/* USER CODE BEGIN 0 */




void my_keypad_read(lv_indev_drv_t * drv, lv_indev_data_t * data);
void My_Group_Init(void);


void PSU_Set_Output_State(uint8_t state) ;//电源开关总指挥
void Update_All_Power_UI(void);
void Switch_State_Changed(lv_event_t * e); //switch回调函数
void Switch2_State_Changed(lv_event_t * e);//switch 2回调函数

void Focus_On_Screen1(lv_event_t * e);//焦点处理
void Focus_On_Screen2(lv_event_t * e);

void My_Waveform_Init(void);


void Update_Main_Screen_Sensors(float voltage, float current, float power, float input_volts);//更新屏幕上的数据
void On_SpinboxVolt_Changed(lv_event_t * e);
void On_SpinboxCurr_Changed(lv_event_t * e);
void Update_Temp_UI(float temp);

void Sync_Struct_To_UI(void);
void On_Spinbox_Changed(lv_event_t * e);

void BUZZER_Init(void);
void BUZZER_Task_Handler(void);
void BUZZER_ShortBeep(void);



