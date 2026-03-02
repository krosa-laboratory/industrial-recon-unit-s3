#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include "WiFiWidsScreen.h"
#include "../../core/UIManager.h"
#include "../menus/WiFiMenuScreen.h"

LV_FONT_DECLARE(lv_font_montserrat_10);
LV_FONT_DECLARE(lv_font_unscii_8);

static volatile unsigned long deauth_count = 0;
static char last_target_mac[18] = "00:00:00:00:00:00";
static char last_source_mac[18] = "00:00:00:00:00:00";
static volatile bool new_attack_detected = false;

static void wids_rx_cb(void *buf, wifi_promiscuous_pkt_type_t type)
{

    if (type != WIFI_PKT_MGMT) return;

    wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t *)buf;
    uint8_t *payload = pkt->payload;

    // payload[0] contains Frame Control
    // 0xC0 is Deauthentication, 0xA0 is Disassociation
    if (payload[0] == 0xC0 || payload[0] == 0xA0)
    {
        deauth_count++;
        new_attack_detected = true;
        // Extract MAC directions (Objetive: bytes 4-9, Origin: bytes 10-15)
        sprintf(last_target_mac, "%02X:%02X:%02X:%02X:%02X:%02X", payload[4], payload[5], payload[6], payload[7], payload[8], payload[9]);
        sprintf(last_source_mac, "%02X:%02X:%02X:%02X:%02X:%02X", payload[10], payload[11], payload[12], payload[13], payload[14], payload[15]);
    }

}

void WiFiWidsScreen::onEnter()
{

    _entry_time = millis();
    _last_update = millis();
    _last_hop_time = millis();
    _current_channel = 1;
    _is_under_attack = false;
    
    deauth_count = 0;
    new_attack_detected = false;

    lv_obj_t* screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);

    // Header
    _header_label = lv_label_create(screen);
    lv_label_set_text(_header_label, "WIDS (DEAUTH DET.)");
    lv_obj_set_style_text_color(_header_label, lv_color_white(), 0);
    lv_obj_set_style_bg_color(_header_label, lv_color_hex(0x333333), 0);
    lv_obj_set_style_bg_opa(_header_label, LV_OPA_COVER, 0);
    lv_obj_set_width(_header_label, 128);
    lv_obj_set_style_text_align(_header_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(_header_label, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_style_text_font(_header_label, &lv_font_montserrat_10, 0);

    // State dashboard
    _status_panel = lv_obj_create(screen);
    lv_obj_set_size(_status_panel, 128, 25);
    lv_obj_align(_status_panel, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_set_style_bg_color(_status_panel, lv_color_hex(0x005500), 0);
    lv_obj_set_style_border_width(_status_panel, 0, 0);
    lv_obj_set_style_pad_all(_status_panel, 2, 0);

    _status_label = lv_label_create(_status_panel);
    lv_label_set_text(_status_label, "STATUS: SECURE");
    lv_obj_set_style_text_color(_status_label, lv_color_white(), 0);
    lv_obj_align(_status_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_font(_status_label, &lv_font_montserrat_10, 0);

    // Logs terminal
    _log_ta = lv_textarea_create(screen);
    lv_obj_set_size(_log_ta, 128, 60);
    lv_obj_align(_log_ta, LV_ALIGN_BOTTOM_MID, 0, -15);
    lv_obj_set_style_bg_color(_log_ta, lv_color_black(), 0);
    lv_obj_set_style_text_color(_log_ta, lv_color_hex(0xFF5555), 0);
    lv_obj_set_style_border_color(_log_ta, lv_color_hex(0x444444), 0);
    lv_textarea_set_cursor_click_pos(_log_ta, false);
    lv_obj_set_style_text_font(_log_ta, &lv_font_unscii_8, 0);
    lv_textarea_set_text(_log_ta, "> Listening for Deauths\n");

    // Stadistics & channel
    _stats_label = lv_label_create(screen);
    lv_label_set_text(_stats_label, "CH: 1 | DEAUTHS: 0");
    lv_obj_set_style_text_color(_stats_label, lv_color_hex(0x00AAFF), 0);
    lv_obj_align(_stats_label, LV_ALIGN_BOTTOM_LEFT, 2, -2);
    lv_obj_set_style_text_font(_stats_label, &lv_font_montserrat_10, 0);

    // Turn on promiscuous mode with WIDS filter
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(&wids_rx_cb);
    esp_wifi_set_channel(_current_channel, WIFI_SECOND_CHAN_NONE);

    lv_obj_update_layout(screen);
    lv_obj_invalidate(screen);

}

void WiFiWidsScreen::onLoop()
{

    // Continuous scanning every 250ms
    if (millis() - _last_hop_time > 250)
    {
        _last_hop_time = millis();
        _performChannelHopping();
    }
    // Update UI every 100ms
    if (millis() - _last_update > 100)
    {
        _last_update = millis();
        _updateUI();
    }

}

void WiFiWidsScreen::_performChannelHopping()
{

    _current_channel++;
    if (_current_channel > 13) _current_channel = 1;
    esp_wifi_set_channel(_current_channel, WIFI_SECOND_CHAN_NONE);

}

void WiFiWidsScreen::_updateUI()
{

    // Show new attacks in the log
    if (new_attack_detected) 
    {
        new_attack_detected = false;
        _is_under_attack = true;
        _last_attack_time = millis();
        // Turn dashboard color red alert
        lv_obj_set_style_bg_color(_status_panel, lv_color_hex(0xFF0000), 0);
        lv_label_set_text(_status_label, "CRITICAL: ATTACK!");
        // Add log to the terminal
        char log_buf[128];
        sprintf(log_buf, "[!] DEAUTH DETECTED\n TGT: %s\n SRC: %s\n", last_target_mac, last_source_mac);
        lv_textarea_add_text(_log_ta, log_buf);
        // Clean log if it's full
        if (strlen(lv_textarea_get_text(_log_ta)) > 400) lv_textarea_set_text(_log_ta, "> Log cleared.\n");
    }
    // If 3 seconds occurs without attacks, turn seccure state again
    if (_is_under_attack && (millis() - _last_attack_time > 3000)) 
    {
        _is_under_attack = false;
        lv_obj_set_style_bg_color(_status_panel, lv_color_hex(0x005500), 0);
        lv_label_set_text(_status_label, "STATUS: SECURE");
    }

    // Update counters
    lv_label_set_text_fmt(_stats_label, "CH: %d | DEAUTHS: %lu", _current_channel, deauth_count);

}

void WiFiWidsScreen::onButtonPress(int gpio)
{

    if (millis() - _entry_time < 500) return;

    if (gpio == 27) UIManager::getInstance()->switchScreen(new WiFiMenuScreen());
    else if (gpio == 13) lv_textarea_cursor_up(_log_ta);
    else if (gpio == 12) lv_textarea_cursor_down(_log_ta);

}

void WiFiWidsScreen::onExit()
{

    esp_wifi_set_promiscuous(false);
    esp_wifi_set_promiscuous_rx_cb(NULL);
    WiFi.mode(WIFI_OFF);

}