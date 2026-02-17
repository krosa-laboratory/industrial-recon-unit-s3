#include "MainMenuScreen.h"
#include "../UIManager.h"

// Here include all screens available
#include "CanMonitorScreen.h"
#include "WiFiScanScreen.h"

extern lv_group_t* navigation_group; 

void MainMenuScreen::onEnter()
{

    lv_obj_t* screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);
    // Main header label
    lv_obj_t* header = lv_label_create(screen);
    lv_label_set_text(header, "RECON-UNIT");
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_style_text_color(header, lv_color_hex(0x00FF00), 0);
    lv_obj_set_style_text_font(header, &lv_font_montserrat_14, 0);
    // List menu
    lv_obj_t* list = lv_list_create(screen);
    lv_obj_set_size(list, 128, 105);
    lv_obj_align(list, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(list, lv_color_black(), 0);
    lv_obj_set_style_border_width(list, 0, 0);
    // Add options to the menu
    const char * options[] = { "CAN Bus", "WiFi", "RS485", "Bluetooth", "Settings" };
    // Create each option with static styles and event handling
    for(int i = 0; i < 5; i++)
    {
        lv_obj_t* btn = lv_list_add_btn(list, NULL, options[i]);
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x222222), 0);
        lv_obj_set_style_text_color(btn, lv_color_white(), 0);
        lv_obj_set_style_text_font(btn, &lv_font_montserrat_10, 0);
        lv_obj_set_style_border_width(btn, 0, 0);
        lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
        if (navigation_group != NULL) lv_group_add_obj(navigation_group, btn);
    }
    
}

void MainMenuScreen::event_handler(lv_event_t* e)
{

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* obj = lv_event_get_target(e);
    // Handle a click in a button from the screen
    if(code == LV_EVENT_CLICKED)
    {

        const char * text = lv_list_get_btn_text(lv_obj_get_parent(obj), obj);
        // Navigation logic
        if (strcmp(text, "CAN Bus") == 0) UIManager::getInstance()->switchScreen(new CanMonitorScreen());
        else if (strcmp(text, "WiFi") == 0) UIManager::getInstance()->switchScreen(new WiFiScanScreen());

    }

}

void MainMenuScreen::onLoop() {}
void MainMenuScreen::onExit() {}
void MainMenuScreen::onButtonPress(int gpio) {}
