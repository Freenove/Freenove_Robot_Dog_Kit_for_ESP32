/**
 * @file TouchPad.cpp
 * @author suhayl@freenove (support@freenove.com)
 * @brief Touchpad driver and service.
 * @version v1.0.0
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022. Freenove corporation.
 * 
 */

#include "TouchPad.h"

static uint32_t s_pad_init_val;
const char *TAG = "TPD";
static void tp_example_set_thresholds(void)
{
    uint16_t touch_value;
    // read filtered value
    touch_pad_read_filtered(PIN_TOUCH_PAD, &touch_value);
    s_pad_init_val = touch_value;
    ESP_LOGI(TAG, "test init: touch pad [%d] val is %d", PIN_TOUCH_PAD, touch_value);
    // set interrupt threshold.
    touch_pad_set_thresh(PIN_TOUCH_PAD, touch_value * 4 / 5);
}

void setupTouchPad(void)
{
    // Initialize touch pad peripheral, it will start a timer to run a filter
    ESP_LOGI(TAG, "Initializing touch pad");
    ESP_ERROR_CHECK(touch_pad_init());
    // If use interrupt trigger mode, should set touch sensor FSM mode at 'TOUCH_FSM_MODE_TIMER'.
    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
    // Set reference voltage for charging/discharging
    // For most usage scenarios, we recommend using the following combination:
    // the high reference valtage will be 2.7V - 1V = 1.7V, The low reference voltage will be 0.5V.
    touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);
    // Init touch pad IO
    touch_pad_config(PIN_TOUCH_PAD, TOUCH_THRESH_NO_USE);
    // Initialize and start a software filter to detect slight change of capacitance.
    touch_pad_filter_start(TOUCHPAD_FILTER_TOUCH_PERIOD);
    // Set thresh hold
    tp_example_set_thresholds();
    // set isr trigger mode
    touch_pad_set_trigger_mode(TOUCH_TRIGGER_BELOW);
    // Register touch interrupt ISR
    touch_pad_isr_register(isr_touchpad, NULL);
    // enable isr
    touch_pad_intr_enable();
}

// TaskHandle_t taskHandle_TouchPad;
bool touchPadVal = false;
void isr_touchpad(void *arg)
{
    // uint16_t touchVal = 0;
    uint32_t touchStatus = 0;
    touchStatus = touch_pad_get_status(); // TOUCHPAD3 : 0000 1000 = 0x08
    // touch_pad_read_filtered(PIN_TOUCH_PAD, &touchVal);
    //  touch_pad_read(PIN_TOUCH_PAD, &touchVal);
    // Serial.printf("touch isr:%lu, %d, %d\n", millis(), touchVal, touchStatus);
    if (touchStatus == 0x08) // TOUCHPAD3
    {
        touchPadVal = true;
    }
    touch_pad_clear_status();
}

uint8_t touchMechineStatus = 0;
uint32_t t0 = millis(), // start time point
    t1 = 0,             // update time point
    t2 = 0,             // release time
    t3 = 0;             // pressed time
uint8_t sayHelloIndex = 0;
void task_TouchPad(void *pvParameters)
{
    switch (touchMechineStatus)
    {
    case 0: // Idle , Wait for the button to press.
        if (touchPadVal)
        {
            t0 = millis();
            t1 = t0;
            touchPadVal = false;
            touchMechineStatus = 1;
        }
        break;
    case 1: // short press or long press ?
        if (touchPadVal)
        {
            t1 = millis();
            touchPadVal = false;
        }
        t2 = millis() - t1; // Time since the last press signal was detected
        t3 = t1 - t0;       // calculate pressed time.
        if (t3 > 700)       //  long pressed
        {
            // Serial.printf("long press ,%d\n", t3);
            setMelodyToQueue(MELODY_BEEP_1);
            enterMessageQueue(String(ACTION_UP_DOWN) + "#0#\n");
            touchMechineStatus = 3; // wait release
        }
        if (t2 > 50) // release time > 50, press time < 700, short press, enter short pressed state
        {
            touchMechineStatus = 2;
        }
        break;
    case 2: // short press
        // Serial.printf("short press ,%d\n", t3);
        // setMelodyToQueue(MELODY_DHTG);
        setMelodyToQueue(MELODY_BEEP_2);
        // mqMotion.enterForced(String(ACTION_DANCING) + "#" + String(sayHelloIndex) + "#\n");
        sayHelloIndex++;
        if (sayHelloIndex >= 5)
            sayHelloIndex = 0;
        touchMechineStatus = 0;
        break;
    case 3: // Wait for release after long press.
        if (touchPadVal)
        {
            t1 = millis();
            touchPadVal = false;
        }
        t2 = millis() - t1;
        if (t2 > 50) // release time > 50
        {
            touchMechineStatus = 0;
        }
        // touchMechineStatus = 0;
        break;
    default:
        break;
    }
}

void task_Touch(void *pvParameters)
{
	Serial.printf("task_Touch is running...\r\n");
	while(1)
	{
		switch (touchMechineStatus)
		{
			case 0: // Idle , Wait for the button to press.
				if (touchPadVal)
				{
					t0 = millis();
					t1 = t0;
					touchPadVal = false;
					touchMechineStatus = 1;
				}
				break;
			case 1: // short press or long press ?
				if (touchPadVal)
				{
					t1 = millis();
					touchPadVal = false;
				}
				t2 = millis() - t1; // Time since the last press signal was detected
				t3 = t1 - t0;       // calculate pressed time.
				if (t3 > 700)       //  long pressed
				{
					touchMechineStatus = 3; // wait release
				}
				if (t2 > 50) // release time > 50, press time < 700, short press, enter short pressed state
				{
					touchMechineStatus = 2;
				}
				break;
			case 2: // short press
				sayHelloIndex++;
				if (sayHelloIndex >= 5)
					sayHelloIndex = 0;
				touchMechineStatus = 0;
				break;
			case 3: // Wait for release after long press.
				if (touchPadVal)
				{
					t1 = millis();
					touchPadVal = false;
				}
				t2 = millis() - t1;
				if (t2 > 50) // release time > 50
				{
					touchMechineStatus = 0;
				}
				break;
			default:
				break;
		}
	}
}

int getTouch(void)
{
	return touchMechineStatus;
}