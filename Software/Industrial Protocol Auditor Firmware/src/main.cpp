#include <Arduino.h>
#include <TFT_eSPI.h>
#include <lvgl.h>

// UI Architecture
#include "ui/core/UIManager.h"
#include "ui/screens/menus/MainMenuScreen.h"

// GPIOs definition
#define BTN_UP    13
#define BTN_DOWN  12
#define BTN_ENTER 14
#define BTN_BACK  27

// Global variable for navigation group
lv_group_t* navigation_group;

TFT_eSPI tft = TFT_eSPI();
static lv_disp_draw_buf_t draw_buffer;
static lv_color_t buffer[128 * 20];

void myDisplayFlush(lv_disp_drv_t *display, const lv_area_t *area, lv_color_t *color_p)
{

    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)&color_p->full, w * h, true);
    tft.endWrite();
    lv_disp_flush_ready(display);

}

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

void setup()
{

    Serial.begin(115200);
    // Init inputs
    pinMode(BTN_UP, INPUT_PULLUP);
    pinMode(BTN_DOWN, INPUT_PULLUP);
    pinMode(BTN_ENTER, INPUT_PULLUP);
    pinMode(BTN_BACK, INPUT_PULLUP);
    // Init Display
    tft.begin();
    tft.setRotation(0); 
    tft.fillScreen(TFT_BLACK);
    // Init LVGL
    lv_init();
    lv_disp_draw_buf_init(&draw_buffer, buffer, NULL, 128 * 20);
    // Register Display Driver
    static lv_disp_drv_t display_drv;
    lv_disp_drv_init(&display_drv);
    display_drv.hor_res = 132;
    display_drv.ver_res = 132;
    display_drv.flush_cb = myDisplayFlush;
    display_drv.draw_buf = &draw_buffer;
    lv_disp_drv_register(&display_drv);
    // Register Input Driver
    static lv_indev_drv_t indev_driver;
    lv_indev_drv_init(&indev_driver);
    indev_driver.type = LV_INDEV_TYPE_KEYPAD;
    indev_driver.read_cb = myKeypadRead;
    lv_indev_t* my_indev = lv_indev_drv_register(&indev_driver);
    // Create navigation group
    navigation_group = lv_group_create();
    lv_group_set_default(navigation_group);
    lv_indev_set_group(my_indev, navigation_group);
    // Boot display
    UIManager::getInstance()->setup();
    Serial.print("Loading main menu...");
    UIManager::getInstance()->switchScreen(new MainMenuScreen());
    Serial.println("System ready!");

}

void loop()
{

    // Move graphic motor
    lv_timer_handler();
    // Update active screen logic
    UIManager::getInstance()->update();
    // Manual button management
    // Detect BACK button activation
    static bool back_pressed = false;
    if (digitalRead(BTN_BACK) == LOW)
    {
        if (!back_pressed)
        {
            back_pressed = true;
            UIManager::getInstance()->handleInput(BTN_BACK);
        }
    } else back_pressed = false;
    // Same for ENTER button activation
    static bool enter_pressed = false;
    if (digitalRead(BTN_ENTER) == LOW)
    {
        if (!enter_pressed)
        {
            enter_pressed = true;
            UIManager::getInstance()->handleInput(BTN_ENTER);
        }
    } else enter_pressed = false;
    // Detect UP button activation
    static bool up_pressed = false;
    if (digitalRead(BTN_UP) == LOW)
    {
        if (!up_pressed)
        {
            up_pressed = true;
            UIManager::getInstance()->handleInput(BTN_UP);
        }
    } else up_pressed = false;
    // Detect DOWN button activation
    static bool down_pressed = false;
    if (digitalRead(BTN_DOWN) == LOW)
    {
        if (!down_pressed)
        {
            down_pressed = true;
            UIManager::getInstance()->handleInput(BTN_DOWN);
        }
    } else down_pressed = false;
    // Little pause
    delay(5);

}

