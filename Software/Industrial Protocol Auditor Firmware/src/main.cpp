#include <Arduino.h>
#include <TFT_eSPI.h>
#include <lvgl.h>

// External functions
void uiInitInput();
void uiCreateMenu();

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

void setup()
{

    Serial.begin(115200);

    // Init Display
    tft.begin();
    tft.setRotation(0); 
    tft.fillScreen(TFT_BLACK);

    // Init LVGL
    lv_init();
    lv_disp_draw_buf_init(&draw_buffer, buffer, NULL, 128 * 20);

    static lv_disp_drv_t display_drv;
    lv_disp_drv_init(&display_drv);
    display_drv.hor_res = 132;
    display_drv.ver_res = 132;
    display_drv.flush_cb = myDisplayFlush;
    display_drv.draw_buf = &draw_buffer;
    lv_disp_drv_register(&display_drv);

    // Init UI systems
    uiInitInput();
    uiCreateMenu();

    Serial.println("System ready!");

}

void loop()
{

    lv_timer_handler();
    delay(5);

}

