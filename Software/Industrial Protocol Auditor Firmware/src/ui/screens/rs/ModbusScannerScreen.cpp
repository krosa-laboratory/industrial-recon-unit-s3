#include <Arduino.h>
#include "ModbusScannerScreen.h"
#include "../../core/UIManager.h"
#include "../menus/RsMenuScreen.h"

extern lv_group_t* navigation_group;

LV_FONT_DECLARE(lv_font_montserrat_10);
LV_FONT_DECLARE(lv_font_unscii_8);

void ModbusScannerScreen::onEnter()
{

    _entry_time = millis();
    
    lv_obj_t* screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);

    // Header
    _header_label = lv_label_create(screen);
    lv_label_set_text(_header_label, "RS485: MODBUS SCAN");
    lv_obj_set_style_text_color(_header_label, lv_color_white(), 0);
    lv_obj_set_style_bg_color(_header_label, lv_color_hex(0x333333), 0);
    lv_obj_set_style_bg_opa(_header_label, LV_OPA_COVER, 0);
    lv_obj_set_width(_header_label, 128);
    lv_obj_set_style_text_align(_header_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(_header_label, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_style_text_font(_header_label, &lv_font_montserrat_10, 0);

    // State label
    _status_label = lv_label_create(screen);
    lv_label_set_text(_status_label, "ID: 1 | FOUND: 0");
    lv_obj_set_style_text_color(_status_label, lv_color_hex(0x00AAFF), 0);
    lv_obj_align(_status_label, LV_ALIGN_TOP_MID, 0, 22);
    lv_obj_set_style_text_font(_status_label, &lv_font_unscii_8, 0);

    // Progress bar
    _progress_bar = lv_bar_create(screen);
    lv_obj_set_size(_progress_bar, 118, 8);
    lv_obj_align(_progress_bar, LV_ALIGN_TOP_MID, 0, 35);
    lv_bar_set_range(_progress_bar, 1, 247);
    lv_bar_set_value(_progress_bar, 1, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(_progress_bar, lv_color_hex(0x00AAFF), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(_progress_bar, lv_color_hex(0x222222), LV_PART_MAIN);

    // Slaves founded list
    _slave_list = lv_list_create(screen);
    lv_obj_set_size(_slave_list, 128, 75);
    lv_obj_align(_slave_list, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(_slave_list, lv_color_black(), 0);
    lv_obj_set_style_border_width(_slave_list, 0, 0);

    _startScan();

    lv_obj_update_layout(screen);
    lv_obj_invalidate(screen);

}

void ModbusScannerScreen::_startScan()
{

    _is_scanning = true;
    _current_slave_id = 1;
    _devices_found = 0;
    _last_scan_time = millis();

    lv_bar_set_value(_progress_bar, 1, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(_progress_bar, lv_color_hex(0x00AAFF), LV_PART_INDICATOR);
    
    lv_obj_clean(_slave_list);
    if (navigation_group != NULL) lv_group_remove_all_objs(navigation_group);

}

void ModbusScannerScreen::onLoop()
{

    if (_is_scanning && (millis() - _last_scan_time > 15))
    {
        _last_scan_time = millis();

        if (esp_random() % 100 < 3 && _devices_found < 10) 
        {
            _addFoundSlave(_current_slave_id);
            _devices_found++;
        }

        lv_label_set_text_fmt(_status_label, "ID: %d | FOUND: %d", _current_slave_id, _devices_found);
        lv_bar_set_value(_progress_bar, _current_slave_id, LV_ANIM_ON);

        _current_slave_id++;

        if (_current_slave_id > 247)
        {
            _is_scanning = false;
            lv_label_set_text_fmt(_status_label, "DONE | FOUND: %d", _devices_found);
            lv_obj_set_style_bg_color(_progress_bar, lv_color_hex(0x00FF00), LV_PART_INDICATOR);
            
            if (navigation_group != NULL && lv_obj_get_child_cnt(_slave_list) > 0) lv_group_focus_obj(lv_obj_get_child(_slave_list, 0));
        }
    }

}

void ModbusScannerScreen::_addFoundSlave(int slave_id)
{

    char buf[64];

    int fake_register_value = esp_random() % 9999;
    snprintf(buf, sizeof(buf), "SLAVE ID: %02d\nReg[0]: %d", slave_id, fake_register_value);

    lv_obj_t* btn = lv_list_add_btn(_slave_list, 0, buf);

    lv_obj_set_style_bg_color(btn, lv_color_hex(0x111111), 0);
    lv_obj_set_style_text_color(btn, lv_color_white(), 0);
    lv_obj_set_style_border_side(btn, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_color(btn, lv_color_hex(0x00AAFF), 0);
    lv_obj_set_style_text_font(btn, &lv_font_montserrat_10, 0);

    if (navigation_group != NULL) lv_group_add_obj(navigation_group, btn);

}

void ModbusScannerScreen::onButtonPress(int gpio)
{

    if (millis() - _entry_time < 500) return;

    if (gpio == 27) UIManager::getInstance()->switchScreen(new RsMenuScreen());
    else if (gpio == 14) { if (!_is_scanning) _startScan(); }

}

void ModbusScannerScreen::onExit() { if (navigation_group != NULL) lv_group_remove_all_objs(navigation_group); }
