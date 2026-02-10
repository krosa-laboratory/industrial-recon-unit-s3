#include <lvgl.h>

// Styles
static lv_style_t style_base;
static lv_style_t style_btn_def;
static lv_style_t style_btn_sel;

// Function to configure the different styles needed
void uiInitStyles ()
{

    // Black background
    lv_style_init(&style_base);
    lv_style_set_bg_color(&style_base, lv_color_hex(0x000000));
    lv_style_set_text_color(&style_base, lv_color_hex(0xFFFFFF));
    lv_style_set_border_width(&style_base, 0);

    // Default Button
    lv_style_init(&style_btn_def);
    lv_style_set_bg_color(&style_btn_def, lv_color_hex(0x111111));
    lv_style_set_text_color(&style_btn_def, lv_color_hex(0x888888));
    lv_style_set_radius(&style_btn_def, 0);

    // Selected Button
    lv_style_init(&style_btn_sel);
    lv_style_set_bg_color(&style_btn_sel, lv_color_hex(0xFF6600));
    lv_style_set_text_color(&style_btn_sel, lv_color_hex(0x000000));

}

void uiCreateMenu ()
{

    uiInitStyles();

    lv_obj_t* screen = lv_scr_act();
    lv_obj_add_style(screen, &style_base, 0);

    // Header
    lv_obj_t* header = lv_label_create(screen);
    lv_label_set_text(header, "RECON-UNIT");
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_style_text_color(header, lv_color_hex(0x00FF00), 0);
    lv_obj_set_style_text_font(header, &lv_font_montserrat_14, 0);

    // Options list
    lv_obj_t* list = lv_list_create(screen);
    lv_obj_set_size(list, 128, 105);
    lv_obj_align(list, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(list, lv_color_black(), 0);
    lv_obj_set_style_border_width(list, 0, 0);

    // Add buttons
    const char* options[] = { "CAN Monitor", "RS485 Audit", "WiFi Scan", "Settings", "Reboot" };

    for (int i = 0 ; i < 5 ; i++)
    {
        lv_obj_t* button = lv_list_add_btn(list, NULL, options[i]);
        lv_obj_add_style(button, &style_btn_def, 0);
        lv_obj_add_style(button, &style_btn_sel, LV_STATE_FOCUS_KEY);
    }

}
