#include <Arduino.h>
#include "RsSnifferScreen.h"
#include "../../core/UIManager.h"
#include "../menus/RsMenuScreen.h"

LV_FONT_DECLARE(lv_font_unscii_8);
LV_FONT_DECLARE(lv_font_montserrat_10);

#define COLOR_MATRIX lv_color_hex(0x00FF00)
#define COLOR_TX     lv_color_hex(0x00AAFF)
#define DATA_REFRESH_RATE 300 // ms

void RsSnifferScreen::onEnter()
{

    _entry_time = millis();
    _is_paused = false;
    _last_update = millis();

    lv_obj_t* screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);

    // Header
    _header_label = lv_label_create(screen);
    lv_label_set_text(_header_label, "RS485: RAW SNIFFER");
    lv_obj_set_style_text_color(_header_label, lv_color_white(), 0);
    lv_obj_set_style_bg_color(_header_label, lv_color_hex(0x333333), 0);
    lv_obj_set_style_bg_opa(_header_label, LV_OPA_COVER, 0);
    lv_obj_set_width(_header_label, 128);
    lv_obj_set_style_text_align(_header_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(_header_label, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_style_text_font(_header_label, &lv_font_montserrat_10, 0);

    // Terminal
    _terminal_text = lv_textarea_create(screen);
    lv_obj_set_size(_terminal_text, 128, 105);
    lv_obj_align(_terminal_text, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(_terminal_text, lv_color_black(), 0);
    lv_obj_set_style_text_color(_terminal_text, COLOR_MATRIX, 0);
    lv_obj_set_style_border_width(_terminal_text, 0, 0);
    lv_textarea_set_cursor_click_pos(_terminal_text, false);
    lv_obj_set_style_text_font(_terminal_text, &lv_font_unscii_8, 0);

    lv_textarea_set_text(_terminal_text, "> BAUD: 9600 8N1\n> LISTENING...\n");

    lv_obj_update_layout(screen);
    lv_obj_invalidate(screen);

}

void RsSnifferScreen::onLoop()
{

    if (!_is_paused && (millis() - _last_update > DATA_REFRESH_RATE))
    {
        _last_update = millis();
        _updateSniffer();
    }

}

// Simulación simple de un cálculo CRC para que los bytes finales parezcan reales
uint16_t RsSnifferScreen::_calculateFakeCRC(uint8_t* buffer, int length)
{

    uint16_t crc = 0xFFFF;
    for (int pos = 0; pos < length; pos++)
    {
        crc ^= (uint16_t)buffer[pos];
        for (int i = 8; i != 0; i--)
        {
            if ((crc & 0x0001) != 0)
            {
                crc >>= 1;
                crc ^= 0xA001;
            } else crc >>= 1;
        }
    }
    return crc;

}

void RsSnifferScreen::_updateSniffer()
{

    char line_buffer[64];
    uint8_t frame[10];
    
    static bool is_tx = true;
    
    if (is_tx)
    {
        frame[0] = 0x01;
        frame[1] = 0x03;
        frame[2] = 0x00;
        frame[3] = 0x00;
        frame[4] = 0x00;
        frame[5] = 0x02;
        uint16_t crc = _calculateFakeCRC(frame, 6);
        sprintf(line_buffer, "TX:010300000002%02X%02X\n\n", (crc & 0xFF), (crc >> 8));
    } else {
        frame[0] = 0x01;
        frame[1] = 0x03;
        frame[2] = 0x04;
        frame[3] = esp_random() % 256;
        frame[4] = esp_random() % 256;
        frame[5] = esp_random() % 256;
        frame[6] = esp_random() % 256;
        uint16_t crc = _calculateFakeCRC(frame, 7);
        sprintf(line_buffer, "RX:010304%02X%02X%02X%02X%02X%02X\n\n", frame[3], frame[4], frame[5], frame[6], (crc & 0xFF), (crc >> 8));
    }
    
    is_tx = !is_tx;

    lv_textarea_add_text(_terminal_text, line_buffer);

    if (strlen(lv_textarea_get_text(_terminal_text)) > 400)
    {
        lv_textarea_set_text(_terminal_text, ""); 
        lv_textarea_add_text(_terminal_text, "> BUFFER CLEARED\n");
    }

}

void RsSnifferScreen::onButtonPress(int gpio)
{

    if (millis() - _entry_time < 500) return;

    if (gpio == 27) UIManager::getInstance()->switchScreen(new RsMenuScreen());
    else if (gpio == 14)
    {
        _is_paused = !_is_paused;
        if (_is_paused) lv_label_set_text(_header_label, "RS485: PAUSED");
        else lv_label_set_text(_header_label, "RS485: RAW SNIFFER");
    } else if (gpio == 13) lv_textarea_cursor_up(_terminal_text);
    else if (gpio == 12) lv_textarea_cursor_down(_terminal_text);

}

void RsSnifferScreen::onExit() {}
