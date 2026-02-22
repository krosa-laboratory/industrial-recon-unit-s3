#include <Arduino.h>
#include "CanSnifferScreen.h"
#include "../../core/UIManager.h"
#include "../menus/CanMenuScreen.h"

// Font declaration
LV_FONT_DECLARE(lv_font_unscii_8);
LV_FONT_DECLARE(lv_font_montserrat_10);

#define COLOR_MATRIX lv_color_hex(0x00FF00)
#define DATA_REFRESH_RATE 150 // ms

void CanSnifferScreen::onEnter()
{

    _entry_time = millis();
    _is_paused = false;
    _last_update = millis();

    lv_obj_t* screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);

    // Header
    _header_label = lv_label_create(screen);
    lv_label_set_text(_header_label, "CAN: SNIFFER LOG");
    lv_obj_set_style_text_color(_header_label, lv_color_white(), 0);
    lv_obj_set_style_bg_color(_header_label, lv_color_hex(0x333333), 0);
    lv_obj_set_style_bg_opa(_header_label, LV_OPA_COVER, 0);
    lv_obj_set_width(_header_label, 128);
    lv_obj_set_style_text_align(_header_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(_header_label, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_style_text_font(_header_label, &lv_font_montserrat_10, 0);

    // Terminal
    _terminal_text = lv_textarea_create(screen);
    lv_obj_set_size(_terminal_text, 128, 110);
    lv_obj_align(_terminal_text, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(_terminal_text, lv_color_black(), 0);
    lv_obj_set_style_text_color(_terminal_text, COLOR_MATRIX, 0);
    lv_obj_set_style_border_width(_terminal_text, 0, 0);
    lv_textarea_set_cursor_click_pos(_terminal_text, false);
    lv_obj_set_style_text_font(_terminal_text, &lv_font_unscii_8, 0);

    lv_textarea_set_text(_terminal_text, ">> LISTENING...\n");
    lv_obj_update_layout(screen);
    lv_obj_invalidate(screen);

}

void CanSnifferScreen::onLoop()
{

    if (!_is_paused && (millis() - _last_update > DATA_REFRESH_RATE))
    {
        _last_update = millis();
        _updateSniffer();
    }

}

void CanSnifferScreen::_updateSniffer()
{

    int id = (esp_random() % (0x400 - 0x100)) + 0x100;
    int data_1 = esp_random() % 256;
    int data_2 = esp_random() % 256;

    char buffer[64];
    sprintf(buffer, "%03X [%02X %02X..]\n", id, data_1, data_2);
    lv_textarea_add_text(_terminal_text, buffer);

    if (strlen(lv_textarea_get_text(_terminal_text)) > 500)
    {
        lv_textarea_set_text(_terminal_text, ""); 
        lv_textarea_add_text(_terminal_text, ">> BUFFER CLEARED\n");
    }

}

void CanSnifferScreen::onButtonPress(int gpio)
{

    if (millis() - _entry_time < 500) return;

    if (gpio == 27) UIManager::getInstance()->switchScreen(new CanMenuScreen());
    else if (gpio == 14) // ENTER pauses/resumes
    {
        _is_paused = !_is_paused;
        if (_is_paused) lv_label_set_text(_header_label, "CAN: PAUSED");
        else lv_label_set_text(_header_label, "CAN: SNIFFER LOG");
    }
    else if (gpio == 13) lv_textarea_cursor_up(_terminal_text);
    else if (gpio == 12) lv_textarea_cursor_down(_terminal_text);

}

void CanSnifferScreen::onExit() {}
