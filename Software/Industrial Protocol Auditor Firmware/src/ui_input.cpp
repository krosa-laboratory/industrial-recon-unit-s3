#include <Arduino.h>
#include <lvgl.h>

// GPIOs definition
#define BTN_UP    13
#define BTN_DOWN  12
#define BTN_ENTER 14
#define BTN_BACK  27

// Global variable for navigation group
lv_group_t* navigation_group;

// Function to read the buttons
void myKeypadRead (lv_indev_drv_t* indev_drv, lv_indev_data_t* data)
{

    data->state = LV_INDEV_STATE_RELEASED;

    if (digitalRead(BTN_UP) == LOW)
    {
        data->key = LV_KEY_PREV;
        data->state = LV_INDEV_STATE_PRESSED;
    } else if (digitalRead(BTN_DOWN) == LOW)
    {
        data->key = LV_KEY_NEXT;
        data->state = LV_INDEV_STATE_PRESSED;
    } else if (digitalRead(BTN_ENTER) == LOW)
    {
        data->key = LV_KEY_ENTER;
        data->state = LV_INDEV_STATE_PRESSED;
    } else if (digitalRead(BTN_BACK) == LOW)
    {
        data->key = LV_KEY_ESC;
        data->state = LV_INDEV_STATE_PRESSED;
    }

}

// Function to configure the inputs and the UI
void uiInitInput ()
{

    // Set pull ups into all user input buttons
    pinMode(BTN_BACK, INPUT_PULLUP);
    pinMode(BTN_DOWN, INPUT_PULLUP);
    pinMode(BTN_ENTER, INPUT_PULLUP);
    pinMode(BTN_UP, INPUT_PULLUP);

    // Register the LVGL driver
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init (&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_KEYPAD;
    indev_drv.read_cb = myKeypadRead;
    lv_indev_t* my_indev = lv_indev_drv_register (&indev_drv);

    // Create the Navigation Group for the UI control
    navigation_group = lv_group_create();
    lv_group_set_default(navigation_group);
    lv_indev_set_group(my_indev, navigation_group);

}
