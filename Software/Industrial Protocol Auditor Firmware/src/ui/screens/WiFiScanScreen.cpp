#include <Arduino.h>
#include "WiFiScanScreen.h"
#include "../UIManager.h"
#include "MainMenuScreen.h"

// Reference to the global navigation group defined in main.cpp
extern lv_group_t* navigation_group; 

void WiFiScanScreen::onEnter()
{

    _entry_time = millis();
    lv_obj_t* screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);
    // Prepare header
    _header_label = lv_label_create(screen);
    lv_label_set_text(_header_label, "SCANNING");
    lv_obj_set_style_text_color(_header_label, lv_color_white(), 0);
    lv_obj_set_style_bg_color(_header_label, lv_color_hex(0x333333), 0);
    lv_obj_set_style_bg_opa(_header_label, LV_OPA_COVER, 0);
    lv_obj_set_width(_header_label, 128);
    lv_obj_set_style_text_align(_header_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(_header_label, LV_ALIGN_TOP_MID, 0, 0);
    // Prepare list container
    _wifi_list = lv_list_create(screen);
    lv_obj_set_size(_wifi_list, 128, 110);
    lv_obj_align(_wifi_list, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(_wifi_list, lv_color_black(), 0);
    lv_obj_set_style_border_width(_wifi_list, 0, 0);
    lv_obj_set_style_radius(_wifi_list, 0, 0);
    // Generate Mock Data
    simulateScan();
    // Force Redraw
    lv_obj_update_layout(screen);
    lv_obj_invalidate(screen);

}

void WiFiScanScreen::simulateScan()
{

    // Clear list and navigation group
    lv_obj_clean(_wifi_list);
    if (navigation_group != NULL) lv_group_remove_all_objs(navigation_group);
    // Update Header
    lv_label_set_text(_header_label, "WIFI: 5 FOUND");
    lv_obj_set_style_text_font(_header_label, &lv_font_montserrat_10, 0);
    // Add Fake Networks
    addNetworkItem("Skynet_Uplink", -45, true);
    addNetworkItem("Free_Virus", -60, false);
    addNetworkItem("FBI_Surveillance", -75, true);
    addNetworkItem("Neighbor_WiFi", -80, true);
    addNetworkItem("Starlink_Dish", -90, true);
    // Focus the first item
    if (navigation_group != NULL) lv_group_focus_obj(lv_obj_get_child(_wifi_list, 0));

}

void WiFiScanScreen::addNetworkItem(const char* ssid, int rssi, bool encrypted)
{

    // Create button in list
    lv_obj_t* btn = lv_list_add_btn(_wifi_list, 0, ssid);
    // Custom Style for the Item
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x111111), 0);
    lv_obj_set_style_text_color(btn, lv_color_white(), 0);
    lv_obj_set_style_border_side(btn, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_color(btn, lv_color_hex(0x444444), 0);
    lv_obj_set_style_text_font(btn, &lv_font_unscii_8, 0);
    // Create a label for RSSI
    lv_obj_t* rssi_label = lv_label_create(btn);
    lv_label_set_text_fmt(rssi_label, "%d", rssi);
    lv_obj_align(rssi_label, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_text_color(rssi_label, (rssi > -60) ? lv_color_hex(0x00FF00) : lv_color_hex(0xFFAA00), 0);
    // Add to navigation group for UP/DOWN buttons
    if (navigation_group != NULL) lv_group_add_obj(navigation_group, btn);

}

void WiFiScanScreen::onLoop() {}

void WiFiScanScreen::onExit() 
{
    if (navigation_group != NULL) lv_group_remove_all_objs(navigation_group);
}

void WiFiScanScreen::onButtonPress(int gpio)
{
    // Anti-bounce
    if (millis() - _entry_time < 500) return;

    if (gpio == 27) // BACK
    {
        UIManager::getInstance()->switchScreen(new MainMenuScreen());
        return;
    }

    if (gpio == 14) // ENTER (Rescan)
    {
        lv_label_set_text(_header_label, "SCANNING...");
        lv_obj_update_layout(_header_label);
        delay(100);
        simulateScan();
    }
    
}
