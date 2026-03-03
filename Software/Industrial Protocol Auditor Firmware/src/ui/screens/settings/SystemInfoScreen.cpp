#include <Arduino.h>
#include "SystemInfoScreen.h"
#include "../../core/UIManager.h"
#include "../menus/SettingsMenuScreen.h"

LV_FONT_DECLARE(lv_font_montserrat_10);
LV_FONT_DECLARE(lv_font_unscii_8);

void SystemInfoScreen::onEnter()
{

    _entry_time = millis();
    _last_update = 0;

    lv_obj_t* screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);

    // Header
    _header_label = lv_label_create(screen);
    lv_label_set_text(_header_label, "SYS: HARDWARE INFO");
    lv_obj_set_style_text_color(_header_label, lv_color_white(), 0);
    lv_obj_set_style_bg_color(_header_label, lv_color_hex(0x333333), 0);
    lv_obj_set_style_bg_opa(_header_label, LV_OPA_COVER, 0);
    lv_obj_set_width(_header_label, 128);
    lv_obj_set_style_text_align(_header_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(_header_label, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_style_text_font(_header_label, &lv_font_montserrat_10, 0);

    // Information textarea
    _info_ta = lv_textarea_create(screen);
    lv_obj_set_size(_info_ta, 128, 105);
    lv_obj_align(_info_ta, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(_info_ta, lv_color_black(), 0);
    lv_obj_set_style_text_color(_info_ta, lv_color_hex(0x00FF00), 0);
    lv_obj_set_style_border_width(_info_ta, 0, 0);
    lv_textarea_set_cursor_click_pos(_info_ta, false);
    lv_obj_set_style_text_font(_info_ta, &lv_font_unscii_8, 0);

    lv_obj_update_layout(screen);
    lv_obj_invalidate(screen);

}

void SystemInfoScreen::_updateInfo()
{

    char buf[256];
    
    uint32_t free_heap = ESP.getFreeHeap() / 1024;
    uint32_t total_heap = ESP.getHeapSize() / 1024;
    uint8_t cpu_freq = ESP.getCpuFreqMHz();
    const char* chip_model = ESP.getChipModel();
    uint32_t uptime_sec = millis() / 1000;

    snprintf(buf, sizeof(buf), 
        "MODEL: %s\n"
        "CPU:   %d MHz\n"
        "CORES: %d\n"
        "----------------\n"
        "RAM FREE: %d KB\n"
        "RAM TOTL: %d KB\n"
        "----------------\n"
        "UPTIME: %d s\n"
        "FW VER: v1.0.0",
        chip_model, cpu_freq, ESP.getChipCores(), 
        free_heap, total_heap, uptime_sec
    );

    lv_textarea_set_text(_info_ta, buf);

}

void SystemInfoScreen::onLoop()
{

    if (millis() - _last_update > 1000)
    {
        _last_update = millis();
        _updateInfo();
    }

}

void SystemInfoScreen::onButtonPress(int gpio)
{

    if (millis() - _entry_time < 500) return;

    if (gpio == 27) UIManager::getInstance()->switchScreen(new SettingsMenuScreen());
    else if (gpio == 13) lv_textarea_cursor_up(_info_ta);
    else if (gpio == 12) lv_textarea_cursor_down(_info_ta);

}

void SystemInfoScreen::onExit() {}
