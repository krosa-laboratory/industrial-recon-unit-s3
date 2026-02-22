#include <Arduino.h>
#include "CanDashboardScreen.h"
#include "../../core/UIManager.h"
#include "../menus/CanMenuScreen.h"

LV_FONT_DECLARE(lv_font_montserrat_10);

#define COLOR_MATRIX   lv_color_hex(0x00FF00)
#define COLOR_WARNING  lv_color_hex(0xFFAA00)
#define COLOR_ERROR    lv_color_hex(0xFF0000)
#define DATA_REFRESH_RATE 150 // ms

void CanDashboardScreen::onEnter()
{

    _entry_time = millis();
    _last_update = millis();

    lv_obj_t* screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);

    // Header
    _header_label = lv_label_create(screen);
    lv_label_set_text(_header_label, "CAN: DASHBOARD");
    lv_obj_set_style_text_color(_header_label, lv_color_white(), 0);
    lv_obj_set_style_bg_color(_header_label, lv_color_hex(0x333333), 0);
    lv_obj_set_style_bg_opa(_header_label, LV_OPA_COVER, 0);
    lv_obj_set_width(_header_label, 128);
    lv_obj_set_style_text_align(_header_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(_header_label, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_style_text_font(_header_label, &lv_font_montserrat_10, 0);

    // Main container
    lv_obj_t* stats_view = lv_obj_create(screen);
    lv_obj_set_size(stats_view, 128, 110);
    lv_obj_align(stats_view, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(stats_view, lv_color_black(), 0);
    lv_obj_set_style_border_width(stats_view, 0, 0);
    lv_obj_set_style_pad_all(stats_view, 2, 0);

    // Bus Load Label
    lv_obj_t* labelLoad = lv_label_create(stats_view);
    lv_label_set_text(labelLoad, "BUS LOAD:");
    lv_obj_align(labelLoad, LV_ALIGN_TOP_LEFT, 2, 5);
    lv_obj_set_style_text_color(labelLoad, lv_color_white(), 0);

    // Bar
    _bar_bus_load = lv_bar_create(stats_view);
    lv_obj_set_size(_bar_bus_load, 118, 10);
    lv_obj_align(_bar_bus_load, LV_ALIGN_TOP_MID, 0, 25);
    lv_bar_set_range(_bar_bus_load, 0, 100);
    lv_bar_set_value(_bar_bus_load, 0, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(_bar_bus_load, COLOR_MATRIX, LV_PART_INDICATOR);

    // Error counters label
    _label_error_count = lv_label_create(stats_view);
    lv_label_set_text(_label_error_count, "ERRORS: 0");
    lv_obj_align(_label_error_count, LV_ALIGN_LEFT_MID, 2, 10);
    lv_obj_set_style_text_color(_label_error_count, COLOR_ERROR, 0);

    // Unique IDs label
    _label_unique_ids = lv_label_create(stats_view);
    lv_label_set_text(_label_unique_ids, "UNIQUE IDs: 0");
    lv_obj_align(_label_unique_ids, LV_ALIGN_LEFT_MID, 2, 30);
    lv_obj_set_style_text_color(_label_unique_ids, lv_color_hex(0x00AAFF), 0);

    // Font text for labels
    lv_obj_set_style_text_font(labelLoad, &lv_font_montserrat_10, 0);
    lv_obj_set_style_text_font(_label_error_count, &lv_font_montserrat_10, 0);
    lv_obj_set_style_text_font(_label_unique_ids, &lv_font_montserrat_10, 0);

    lv_obj_update_layout(screen);
    lv_obj_invalidate(screen);

}

void CanDashboardScreen::onLoop()
{

    if (millis() - _last_update > DATA_REFRESH_RATE)
    {
        _last_update = millis();
        _updateStats();
    }

}

void CanDashboardScreen::_updateStats()
{

    static int load = 0;
    load = (load + (esp_random() % 15) - 5);
    if (load < 0) load = 0;
    if (load > 100) load = 100;

    lv_bar_set_value(_bar_bus_load, load, LV_ANIM_ON);

    if (load > 80) lv_obj_set_style_bg_color(_bar_bus_load, COLOR_ERROR, LV_PART_INDICATOR);
    else if (load > 50) lv_obj_set_style_bg_color(_bar_bus_load, COLOR_WARNING, LV_PART_INDICATOR);
    else lv_obj_set_style_bg_color(_bar_bus_load, COLOR_MATRIX, LV_PART_INDICATOR);

    static int errors = 0;
    static int ids_num = 0;
    if ((esp_random() % 100) > 95) errors++;
    if ((esp_random() % 100) > 80) ids_num++;

    lv_label_set_text_fmt(_label_error_count, "ERRORS: %d", errors);
    lv_label_set_text_fmt(_label_unique_ids, "UNIQUE IDs: %d", ids_num);

}

void CanDashboardScreen::onButtonPress(int gpio)
{

    if (millis() - _entry_time < 500) return;
    if (gpio == 27) UIManager::getInstance()->switchScreen(new CanMenuScreen());

}

void CanDashboardScreen::onExit() {}
