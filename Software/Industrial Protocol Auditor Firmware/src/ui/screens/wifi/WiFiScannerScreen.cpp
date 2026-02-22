#include <Arduino.h>
#include <WiFi.h>
#include "WiFiScannerScreen.h"
#include "../../core/UIManager.h"
#include "../menus/WiFiMenuScreen.h"

extern lv_group_t* navigation_group;

LV_FONT_DECLARE(lv_font_montserrat_10);

void WiFiScannerScreen::onEnter()
{

    _entry_time = millis();
    _is_scanning = false;

    lv_obj_t* screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);

    // Header
    _header_label = lv_label_create(screen);
    lv_obj_set_style_text_color(_header_label, lv_color_white(), 0);
    lv_obj_set_style_bg_color(_header_label, lv_color_hex(0x333333), 0);
    lv_obj_set_style_bg_opa(_header_label, LV_OPA_COVER, 0);
    lv_obj_set_width(_header_label, 128);
    lv_obj_set_style_text_align(_header_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(_header_label, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_style_text_font(_header_label, &lv_font_montserrat_10, 0);

    // List container
    _wifi_list = lv_list_create(screen);
    lv_obj_set_size(_wifi_list, 128, 110);
    lv_obj_align(_wifi_list, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(_wifi_list, lv_color_black(), 0);
    lv_obj_set_style_border_width(_wifi_list, 0, 0);
    lv_obj_set_style_radius(_wifi_list, 0, 0);

    // Configure WiFi hardware in station mode and start scanning
    WiFi.mode(WIFI_OFF);
    delay(100);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true);
    delay(100);

    _startScan();

    lv_obj_update_layout(screen);
    lv_obj_invalidate(screen);

}

void WiFiScannerScreen::_startScan()
{

    if (_is_scanning) return; // Never start two scannings at the same time

    _is_scanning = true;
    lv_label_set_text(_header_label, "WIFI: SCANNING...");

    // Clear actual view and navigation group
    lv_obj_clean(_wifi_list);
    if (navigation_group != NULL) lv_group_remove_all_objs(navigation_group);

    // Launch real scan asyncronously to avoid blocking the display
    WiFi.scanDelete();
    delay(50);
    WiFi.scanNetworks(true);

}

void WiFiScannerScreen::onLoop()
{

    if (_is_scanning)
    {
        int n = WiFi.scanComplete();
        if (n >= 0)
        {
            _processScanResults(n);
            _is_scanning = false;
        } else if (n == WIFI_SCAN_FAILED)
        {
            WiFi.scanDelete();
            delay(50);
            WiFi.scanNetworks(true);
        }
    }

}

void WiFiScannerScreen::_processScanResults(int networksFound)
{

    lv_label_set_text_fmt(_header_label, "WIFI: %d FOUND", networksFound);

    // Limit of 15 networks to avoid using too much RAM
    int displayCount = (networksFound > 15) ? 15 : networksFound;

    for (int i = 0; i < displayCount; ++i)
    {
        // Get data from the network
        String ssid = WiFi.SSID(i);
        int rssi = WiFi.RSSI(i);
        bool encrypted = (WiFi.encryptionType(i) != WIFI_AUTH_OPEN);
        // Placeholder for hidden SSID
        if (ssid.length() == 0) ssid = "<HIDDEN>";
        _addNetworkItem(ssid.c_str(), rssi, encrypted);
    }

    // Free RAM from scanning
    WiFi.scanDelete();
    // Focus in the first network in the list
    if (navigation_group != NULL && lv_obj_get_child_cnt(_wifi_list) > 0)
        lv_group_focus_obj(lv_obj_get_child(_wifi_list, 0));

}

void WiFiScannerScreen::_addNetworkItem(const char* ssid, int rssi, bool encrypted)
{

    // WiFi button with the info
    lv_obj_t* btn = lv_list_add_btn(_wifi_list, 0, ssid);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x111111), 0);
    lv_obj_set_style_text_color(btn, lv_color_white(), 0);
    lv_obj_set_style_border_side(btn, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_color(btn, lv_color_hex(0x444444), 0);
    lv_obj_set_style_text_font(btn, &lv_font_montserrat_10, 0);

    // Label for RSSI
    lv_obj_t* rssi_label = lv_label_create(btn);
    lv_label_set_text_fmt(rssi_label, "%d", rssi);
    lv_obj_align(rssi_label, LV_ALIGN_RIGHT_MID, 0, 0);
    
    // Change color for RSSI value (Green: Good, Orange: Medium, Red: Bad)
    if (rssi > -60) lv_obj_set_style_text_color(rssi_label, lv_color_hex(0x00FF00), 0);
    else if (rssi > -80) lv_obj_set_style_text_color(rssi_label, lv_color_hex(0xFFAA00), 0);
    else lv_obj_set_style_text_color(rssi_label, lv_color_hex(0xFF0000), 0);
   
    lv_obj_set_style_text_font(rssi_label, &lv_font_montserrat_10, 0);
    // Add to the group so user can navigate with the buttons
    if (navigation_group != NULL) lv_group_add_obj(navigation_group, btn);

}

void WiFiScannerScreen::onButtonPress(int gpio)
{

    if (millis() - _entry_time < 500) return;

    if (gpio == 27)
    {
        if (_is_scanning) WiFi.scanDelete(); 
        UIManager::getInstance()->switchScreen(new WiFiMenuScreen());
    } else if (gpio == 14) _startScan();

}

void WiFiScannerScreen::onExit()
{

    if (_is_scanning) WiFi.scanDelete();
    WiFi.mode(WIFI_OFF);
    if (navigation_group != NULL) lv_group_remove_all_objs(navigation_group);

}
