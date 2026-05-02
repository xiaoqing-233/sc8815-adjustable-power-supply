/**
 * @file lv_port_indev_templ.c
 *
 */

/*Copy this file as "lv_port_indev.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_indev.h"
#include "../../lvgl.h"
#include "main.h"
#include "tim.h"
#include "GUI.h"
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

//static void touchpad_init(void);
//static void touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
//static bool touchpad_is_pressed(void);
//static void touchpad_get_xy(lv_coord_t * x, lv_coord_t * y);

//static void mouse_init(void);
//static void mouse_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
//static bool mouse_is_pressed(void);
//static void mouse_get_xy(lv_coord_t * x, lv_coord_t * y);

static void keypad_init(void);
static void keypad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static uint32_t keypad_get_key(void);

static void encoder_init(void);
static void encoder_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static void encoder_handler(void);

static void button_init(void);
static void button_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static int8_t button_get_pressed_id(void);
static bool button_is_pressed(uint8_t id);



/**********************
 *  STATIC VARIABLES
 **********************/
//lv_indev_t * indev_touchpad;
//lv_indev_t * indev_mouse;
lv_indev_t * indev_keypad;
lv_indev_t * indev_encoder;
//lv_indev_t * indev_button;

int16_t encoder_diff;
lv_indev_state_t encoder_state;
// 用于记录上一次的计数值
uint16_t last_counter_value = 0;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_group_t * group; 
void lv_port_indev_init(void)
{
    /**
     * Here you will find example implementation of input devices supported by LittelvGL:
     *  - Touchpad
     *  - Mouse (with cursor support)
     *  - Keypad (supports GUI usage only with key)
     *  - Encoder (supports GUI usage only with: left, right, push)
     *  - Button (external buttons to press points on the screen)
     *
     *  The `..._read()` function are only examples.
     *  You should shape them according to your hardware
     */

    static lv_indev_drv_t keypad_drv;
		static lv_indev_drv_t encoder_drv;
//    /*------------------
//     * Touchpad
//     * -----------------*/

//    /*Initialize your touchpad if you have*/
//    touchpad_init();

//    /*Register a touchpad input device*/
//    lv_indev_drv_init(&indev_drv);
//    indev_drv.type = LV_INDEV_TYPE_POINTER;
//    indev_drv.read_cb = touchpad_read;
//    indev_touchpad = lv_indev_drv_register(&indev_drv);

//    /*------------------
//     * Mouse
//     * -----------------*/

//    /*Initialize your mouse if you have*/
//    mouse_init();

//    /*Register a mouse input device*/
//    lv_indev_drv_init(&indev_drv);
//    indev_drv.type = LV_INDEV_TYPE_POINTER;
//    indev_drv.read_cb = mouse_read;
//    indev_mouse = lv_indev_drv_register(&indev_drv);

//    /*Set cursor. For simplicity set a HOME symbol now.*/
//    lv_obj_t * mouse_cursor = lv_img_create(lv_scr_act());
//    lv_img_set_src(mouse_cursor, LV_SYMBOL_HOME);
//    lv_indev_set_cursor(indev_mouse, mouse_cursor);

    /*------------------
     * Keypad
     * -----------------*/

    /*Initialize your keypad or keyboard if you have*/
    keypad_init();

    /*Register a keypad input device*/
    lv_indev_drv_init(&keypad_drv);
    keypad_drv.type = LV_INDEV_TYPE_KEYPAD;
    keypad_drv.read_cb = keypad_read;
		indev_keypad = lv_indev_drv_register(&keypad_drv);
		
		
		group = lv_group_create();
		lv_indev_set_group(indev_keypad, group); 
		lv_group_set_default(group);
//		indev_drv.long_press_time = 1000; 
//    
    // 连发间隔时间 (默认 100ms，这里设为 200ms)
//    indev_drv.long_press_repeat_time = 200;
//    indev_keypad = lv_indev_drv_register(&indev_drv);

    /*Later you should create group(s) with `lv_group_t * group = lv_group_create()`,
     *add objects to the group with `lv_group_add_obj(group, obj)`
     *and assign this input device to group to navigate in it:
     *`lv_indev_set_group(indev_keypad, group);`*/

    /*------------------
     * Encoder
     * -----------------*/

    /*Initialize your encoder if you have*/
    encoder_init();

    /*Register a encoder input device*/
    lv_indev_drv_init(&encoder_drv);
    encoder_drv.type = LV_INDEV_TYPE_ENCODER;
    encoder_drv.read_cb = encoder_read;
    indev_encoder = lv_indev_drv_register(&encoder_drv);

		lv_indev_set_group(indev_encoder, group);
//    /*------------------
//     * Button
//     * -----------------*/

//    /*Initialize your button if you have*/
//    button_init();

//    /*Register a button input device*/
//    lv_indev_drv_init(&indev_drv);
//    indev_drv.type = LV_INDEV_TYPE_BUTTON;
//    indev_drv.read_cb = button_read;
//    indev_button = lv_indev_drv_register(&indev_drv);

//    /*Assign buttons to points on the screen*/
//    static const lv_point_t btn_points[2] = {
//        {10, 10},   /*Button 0 -> x:10; y:10*/
//        {40, 100},  /*Button 1 -> x:40; y:100*/
//    };
//    lv_indev_set_button_points(indev_button, btn_points);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*------------------
 * Keypad
 * -----------------*/

/*Initialize your keypad*/
static void keypad_init(void)
{
    /*Your code comes here*/
}


static uint8_t key_read(void)
{
    uint8_t act_key = 0; // 默认返回0，表示没有给 LVGL 的按键

    // ==========================================================
    // 1. 导航按键逻辑 (给 LVGL 的菜单操作用的)
    // ==========================================================
    if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) == GPIO_PIN_RESET) {
        act_key = 1; 
    }
    else if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6) == GPIO_PIN_RESET) {
        act_key = 2; 
    }
		else if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7) == GPIO_PIN_RESET) {
        act_key = 3; 
    }

		else if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8) == GPIO_PIN_RESET) {
        act_key = 4;
    }


    // ==========================================================
    // 2. 独立电源键逻辑 (直接拦截，不返回给 LVGL)
    // ==========================================================
    static uint8_t last_pwr_key_state = 1; // 1:松开, 0:按下
    uint8_t curr_pwr_key_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15); 

    // 检测下降沿 (上一次是1，这一次是0 -> 说明刚按下)
    if(last_pwr_key_state == 1 && curr_pwr_key_state == 0)
    {
        // 简单消抖：如果需要更强的消抖，可以加 HAL_Delay(10) 或者软件计数
        // 这里假设 LVGL 轮询间隔自带了一定的消抖效果
		  	BUZZER_ShortBeep();
        // === 核心动作：翻转电源状态 ===
        // 如果当前是开(1)，就传0；如果当前是关(0)，就传1
        PSU_Set_Output_State(!PSU.Is_Output_On);
    }
    
    // 更新历史状态，供下一次比较
    last_pwr_key_state = curr_pwr_key_state;

    // ==========================================================
    // 3. 返回导航键结果
    // ==========================================================
    return act_key; 
}


/*Will be called by the library to read the mouse*/
static void keypad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    static uint32_t last_key = 0;
		static bool is_pressed_last = false; // 【新增】记录上一次是否按下
    uint32_t act_key = key_read();
    if(act_key != 0) {
        data->state = LV_INDEV_STATE_PR;
			
				if(is_pressed_last == false)
        {
            BUZZER_ShortBeep();
            is_pressed_last = true;
        }
				
        switch(act_key)
        {
	        case 1:
//	            act_key = LV_KEY_NEXT;
	            break;
	        case 2:
	            act_key = LV_KEY_NEXT;
	            break;
	        case 3:
	            act_key = LV_KEY_ENTER;
	            break;
	        case 4:
//	        	act_key = LV_KEY_ENTER;
	        default:
	        	break;
        }
        last_key = act_key;
    } else {
        data->state = LV_INDEV_STATE_REL;
				is_pressed_last = false;
    }
    data->key = last_key;
}


/*Get the currently being pressed key.  0 if no key is pressed*/
static uint32_t keypad_get_key(void)
{
    /*Your code comes here*/

    return 0;
}
/*------------------
 * Encoder
 * -----------------*/

/*Initialize your keypad*/
static void encoder_init(void)
{

}

/*Will be called by the library to read the encoder*/
static void encoder_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    // 0. 读取系统时间
    uint32_t curr_time = HAL_GetTick();
    
    // 1. 定义配置参数 (可以根据手感调整)
    const uint32_t LONG_PRESS_DELAY = 400; // 按住 400ms 后开始连发
    const uint32_t REPEAT_INTERVAL  = 50;  // 连发速度：每 50ms 跳一次 (一秒跳20下)

    // 2. 静态变量 (记录状态)
    // state: 0=松开, 1=刚按下等待长按, 2=正在连发
    static uint8_t left_state = 0;
    static uint8_t right_state = 0;
    static uint32_t left_timer = 0;
    static uint32_t right_timer = 0;

    // 3. 读取硬件引脚
    bool is_left  = (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_11) == GPIO_PIN_RESET);
    bool is_right = (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_2)  == GPIO_PIN_RESET);
    bool is_enter = (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_12) == GPIO_PIN_RESET);

    // 4. 初始化输出
    data->enc_diff = 0;

    // ============================================================
    // 左拨处理 (PC11) - 向下/减小
    // ============================================================
    if (is_left) {
        if (left_state == 0) {
            // [阶段1] 刚按下：触发一次，并启动长按计时
            data->enc_diff = -1;
					  BUZZER_ShortBeep();
            left_state = 1;
            left_timer = curr_time + LONG_PRESS_DELAY; 
        } 
        else if (left_state == 1) {
            // [阶段2] 按住中：等待达到长按时间
            if (curr_time > left_timer) {
                left_state = 2; // 时间到，切换为连发模式
            }
        }
        
        if (left_state == 2) {
            // [阶段3] 连发模式：每隔 REPEAT_INTERVAL 触发一次
            if (curr_time > left_timer) {
                data->enc_diff = -1;
							  BUZZER_ShortBeep();
                left_timer = curr_time + REPEAT_INTERVAL; // 设定下一次触发时间
            }
        }
    } else {
        left_state = 0; // 松开复位
    }

    // ============================================================
    // 右拨处理 (PD2) - 向上/增加
    // ============================================================
    // 加个 else 是为了防止左右互搏，优先响应左边（或者你可以不加else允许同时按）
    if (is_right) {
        if (right_state == 0) {
            data->enc_diff = 1;
					  BUZZER_ShortBeep();
            right_state = 1;
            right_timer = curr_time + LONG_PRESS_DELAY;
        } 
        else if (right_state == 1) {
            if (curr_time > right_timer) {
                right_state = 2;
            }
        }
        
        if (right_state == 2) {
            if (curr_time > right_timer) {
                data->enc_diff = 1;
								BUZZER_ShortBeep();
                right_timer = curr_time + REPEAT_INTERVAL;
            }
        }
    } else {
        right_state = 0; // 松开复位
    }

    // ============================================================
    // 确认键处理
    // ============================================================
    static bool last_enter_state = false; // 【新增】用于检测边沿

    if(is_enter) {
        data->state = LV_INDEV_STATE_PR;
        
        // 【新增】按下瞬间响
        if(last_enter_state == false) {
             BUZZER_ShortBeep();
             last_enter_state = true;
        }
    } else {
        data->state = LV_INDEV_STATE_REL;
        last_enter_state = false;
    }
}


static void encoder_handler(void)
{
}

/*------------------
 * Button
 * -----------------*/

/*Initialize your buttons*/
static void button_init(void)
{
    /*Your code comes here*/
}

/*Will be called by the library to read the button*/
static void button_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{

    static uint8_t last_btn = 0;

    /*Get the pressed button's ID*/
    int8_t btn_act = button_get_pressed_id();

    if(btn_act >= 0) {
        data->state = LV_INDEV_STATE_PR;
        last_btn = btn_act;
    }
    else {
        data->state = LV_INDEV_STATE_REL;
    }

    /*Save the last pressed button's ID*/
    data->btn_id = last_btn;
}

/*Get ID  (0, 1, 2 ..) of the pressed button*/
static int8_t button_get_pressed_id(void)
{
    uint8_t i;

    /*Check to buttons see which is being pressed (assume there are 2 buttons)*/
    for(i = 0; i < 2; i++) {
        /*Return the pressed button's ID*/
        if(button_is_pressed(i)) {
            return i;
        }
    }

    /*No button pressed*/
    return -1;
}

/*Test if `id` button is pressed or not*/
static bool button_is_pressed(uint8_t id)
{

    /*Your code comes here*/

    return false;
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
