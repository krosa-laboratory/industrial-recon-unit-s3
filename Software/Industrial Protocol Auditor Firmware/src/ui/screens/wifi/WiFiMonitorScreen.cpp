#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include "WiFiMonitorScreen.h"
#include "../../core/UIManager.h"
#include "../menus/WiFiMenuScreen.h"

#define COLOR_MGMT   lv_color_hex(0x00FF00)
#define COLOR_DATA   lv_color_hex(0x0099FF)

LV_FONT_DECLARE(lv_font_montserrat_10);
LV_FONT_DECLARE(lv_font_unscii_8);


// Promiscuous mode callback variables and method
static volatile unsigned long pkt_mgmt = 0;
static volatile unsigned long pkt_ctrl = 0;
static volatile unsigned long pkt_data = 0;
static volatile unsigned long pkt_total = 0;
static void promiscuous_rx_cb(void *buf, wifi_promiscuous_pkt_type_t type)
{

    pkt_total++;
    switch (type)
    {
        case WIFI_PKT_MGMT: pkt_mgmt++; break;
        case WIFI_PKT_CTRL: pkt_ctrl++; break;
        case WIFI_PKT_DATA: pkt_data++; break;
        default: break;
    }

}

void WiFiMonitorScreen::onEnter()
{

    _entry_time = millis();
    _last_update = millis();
    _last_hop_time = millis();
    _current_channel = 1;

    // Reset counters
    pkt_mgmt = 0;
    pkt_ctrl = 0;
    pkt_data = 0;
    pkt_total = 0;
    _last_mgmt_count = 0;
    _last_data_count = 0;

    lv_obj_t* screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);

    // Header
    _header_label = lv_label_create(screen);
    lv_label_set_text(_header_label, "WIFI: PROMISCUOUS");
    lv_obj_set_style_text_color(_header_label, lv_color_white(), 0);
    lv_obj_set_style_bg_color(_header_label, lv_color_hex(0x333333), 0);
    lv_obj_set_width(_header_label, 128);
    lv_obj_align(_header_label, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_style_text_font(_header_label, &lv_font_montserrat_10, 0);

    // Graphic
    _chart_traffic = lv_chart_create(screen);
    lv_obj_set_size(_chart_traffic, 128, 60);
    lv_obj_align(_chart_traffic, LV_ALIGN_TOP_MID, 0, 20);
    lv_chart_set_type(_chart_traffic, LV_CHART_TYPE_LINE);
    lv_obj_set_style_bg_color(_chart_traffic, lv_color_hex(0x101010), 0);
    lv_obj_set_style_border_width(_chart_traffic, 0, 0);
    lv_chart_set_point_count(_chart_traffic, 20);
    lv_chart_set_div_line_count(_chart_traffic, 3, 0);
    lv_chart_set_range(_chart_traffic, LV_CHART_AXIS_PRIMARY_Y, 0, 50);

    _series_mgmt = lv_chart_add_series(_chart_traffic, COLOR_MGMT, LV_CHART_AXIS_PRIMARY_Y);
    _series_data = lv_chart_add_series(_chart_traffic, COLOR_DATA, LV_CHART_AXIS_PRIMARY_Y);

    // Dashboard
    lv_obj_t* panel = lv_obj_create(screen);
    lv_obj_set_size(panel, 128, 48);
    lv_obj_align(panel, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(panel, lv_color_black(), 0);
    lv_obj_set_style_border_color(panel, lv_color_hex(0x444444), 0);
    lv_obj_set_style_pad_all(panel, 2, 0);

    _label_channel = lv_label_create(panel);
    lv_label_set_text(_label_channel, "CH: 1");
    lv_obj_set_style_text_color(_label_channel, lv_color_hex(0xFFCC00), 0);
    lv_obj_align(_label_channel, LV_ALIGN_TOP_LEFT, 2, 2);

    _label_stats = lv_label_create(panel);
    lv_label_set_text(_label_stats, "PKT: 0\nMGMT: 0\nDATA: 0");
    lv_obj_set_style_text_color(_label_stats, lv_color_white(), 0);
    lv_obj_align(_label_stats, LV_ALIGN_TOP_RIGHT, -2, 2);
    lv_obj_set_style_text_align(_label_stats, LV_TEXT_ALIGN_RIGHT, 0);

    lv_obj_set_style_text_font(_label_channel, &lv_font_unscii_8, 0);
    lv_obj_set_style_text_font(_label_stats, &lv_font_unscii_8, 0);

    // Starts the promiscuous mode
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(&promiscuous_rx_cb);
    esp_wifi_set_channel(_current_channel, WIFI_SECOND_CHAN_NONE);

    lv_obj_update_layout(screen);
    lv_obj_invalidate(screen);

}

void WiFiMonitorScreen::onLoop()
{

    // Channel Hopping
    if (millis() - _last_hop_time > 300) 
    {
        _last_hop_time = millis();
        _performChannelHopping();
    }
    // Update UI
    if (millis() - _last_update > 200) 
    {
        _last_update = millis();
        _updateUI();
    }

}

void WiFiMonitorScreen::_performChannelHopping()
{

    _current_channel++;
    if (_current_channel > 13) _current_channel = 1;
    
    esp_wifi_set_channel(_current_channel, WIFI_SECOND_CHAN_NONE);
    lv_label_set_text_fmt(_label_channel, "CH: %d", _current_channel);

}

void WiFiMonitorScreen::_updateUI()
{

    // Capture volatile variables
    unsigned long current_mgmt = pkt_mgmt;
    unsigned long current_data = pkt_data;
    // Calculate how many packets have arrived since last tick
    unsigned long diff_mgmt = current_mgmt - _last_mgmt_count;
    unsigned long diff_data = current_data - _last_data_count;

    _last_mgmt_count = current_mgmt;
    _last_data_count = current_data;

    // Update chart
    lv_chart_set_next_value(_chart_traffic, _series_mgmt, diff_mgmt);
    lv_chart_set_next_value(_chart_traffic, _series_data, diff_data);

    // Update texts
    lv_label_set_text_fmt(_label_stats, "ALL: %lu\nMGMT: %lu\nDATA: %lu", pkt_total, current_mgmt, current_data);

}

void WiFiMonitorScreen::onButtonPress(int gpio)
{

    if (millis() - _entry_time < 500) return;
    if (gpio == 27) UIManager::getInstance()->switchScreen(new WiFiMenuScreen());

}

void WiFiMonitorScreen::onExit()
{

    esp_wifi_set_promiscuous(false);
    esp_wifi_set_promiscuous_rx_cb(NULL);
    WiFi.mode(WIFI_OFF);

}
