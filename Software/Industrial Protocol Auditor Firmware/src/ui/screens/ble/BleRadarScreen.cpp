#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include "BleRadarScreen.h"
#include "../../core/UIManager.h"
#include "../menus/BleMenuScreen.h"

extern lv_group_t* navigation_group;

LV_FONT_DECLARE(lv_font_montserrat_10);

// Global variables for asyncronous BLE comms
static BLEScan* pBLEScan = nullptr;
static volatile bool scan_completed = false;

// Callback for hardware when finish the scanning
static void scanCompleteCB(BLEScanResults foundDevices) { scan_completed = true; }

void BleRadarScreen::onEnter()
{

    _entry_time = millis();
    _is_scanning = false;
    scan_completed = false;

    lv_obj_t* screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);

    // Header
    _header_label = lv_label_create(screen);
    lv_label_set_text(_header_label, "BLE: RADAR SCAN");
    lv_obj_set_style_text_color(_header_label, lv_color_white(), 0);
    lv_obj_set_style_bg_color(_header_label, lv_color_hex(0x333333), 0);
    lv_obj_set_style_bg_opa(_header_label, LV_OPA_COVER, 0);
    lv_obj_set_width(_header_label, 128);
    lv_obj_set_style_text_align(_header_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(_header_label, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_style_text_font(_header_label, &lv_font_montserrat_10, 0);

    // List container
    _ble_list = lv_list_create(screen);
    lv_obj_set_size(_ble_list, 128, 105);
    lv_obj_align(_ble_list, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(_ble_list, lv_color_black(), 0);
    lv_obj_set_style_border_width(_ble_list, 0, 0);
    lv_obj_set_style_radius(_ble_list, 0, 0);

    // Initialize BLE Hardware
    static bool ble_initialized = false;
    if (!ble_initialized)
    {
        BLEDevice::init("RECON-UNIT");
        ble_initialized = true;
    }

    pBLEScan = BLEDevice::getScan();
    pBLEScan->setActiveScan(true);
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);

    _startScan();

    lv_obj_update_layout(screen);
    lv_obj_invalidate(screen);

}

void BleRadarScreen::_startScan()
{

    if (_is_scanning) return;

    _is_scanning = true;
    scan_completed = false;
    lv_label_set_text(_header_label, "BLE: SCANNING...");

    lv_obj_clean(_ble_list);
    if (navigation_group != NULL) lv_group_remove_all_objs(navigation_group);

    pBLEScan->clearResults();
    pBLEScan->start(3, scanCompleteCB, false);

}

void BleRadarScreen::onLoop()
{

    if (_is_scanning && scan_completed)
    {
        _is_scanning = false;
        scan_completed = false;
        
        BLEScanResults results = pBLEScan->getResults();
        int count = results.getCount();
        
        lv_label_set_text_fmt(_header_label, "BLE: %d FOUND", count);

        // Seccurity limit for RAM management
        int displayCount = (count > 15) ? 15 : count;

        for (int i = 0; i < displayCount; i++) 
        {
            BLEAdvertisedDevice device = results.getDevice(i);
            String name = device.haveName() ? device.getName().c_str() : "<Unknown>";
            String mac = device.getAddress().toString().c_str();
            mac.toUpperCase();
            int rssi = device.getRSSI();
            _addDeviceItem(name.c_str(), mac.c_str(), rssi);
        }

        // Free RAM space
        pBLEScan->clearResults();
        // Focus in the first element
        if (navigation_group != NULL && lv_obj_get_child_cnt(_ble_list) > 0) lv_group_focus_obj(lv_obj_get_child(_ble_list, 0));

    }

}

void BleRadarScreen::_addDeviceItem(const char* name, const char* mac, int rssi)
{

    char buf[64];
    snprintf(buf, sizeof(buf), "%s\n%s", name, mac);

    lv_obj_t* btn = lv_list_add_btn(_ble_list, 0, buf);

    lv_obj_set_style_bg_color(btn, lv_color_hex(0x111111), 0);
    lv_obj_set_style_text_color(btn, lv_color_white(), 0);
    lv_obj_set_style_border_side(btn, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_color(btn, lv_color_hex(0x0055FF), 0);
    lv_obj_set_style_text_font(btn, &lv_font_montserrat_10, 0);

    // RSSI label to the right
    lv_obj_t* rssi_label = lv_label_create(btn);
    lv_label_set_text_fmt(rssi_label, "%d", rssi);
    lv_obj_align(rssi_label, LV_ALIGN_RIGHT_MID, 0, 0);
    
    // Color code for BLE
    if (rssi > -65) lv_obj_set_style_text_color(rssi_label, lv_color_hex(0x00FF00), 0);
    else if (rssi > -85) lv_obj_set_style_text_color(rssi_label, lv_color_hex(0xFFAA00), 0);
    else lv_obj_set_style_text_color(rssi_label, lv_color_hex(0xFF0000), 0);
    
    lv_obj_set_style_text_font(rssi_label, &lv_font_montserrat_10, 0);

    if (navigation_group != NULL) lv_group_add_obj(navigation_group, btn);

}

void BleRadarScreen::onButtonPress(int gpio)
{

    if (millis() - _entry_time < 500) return;

    if (gpio == 27)
    {
        if (_is_scanning) pBLEScan->stop();
            UIManager::getInstance()->switchScreen(new BleMenuScreen());
    } else if (gpio == 14) _startScan();

}

void BleRadarScreen::onExit()
{

    if (_is_scanning) pBLEScan->stop();
    if (navigation_group != NULL) lv_group_remove_all_objs(navigation_group);

}
