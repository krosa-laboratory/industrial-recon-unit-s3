#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEClient.h>
#include "BleGattScreen.h"
#include "../../core/UIManager.h"
#include "../menus/BleMenuScreen.h"

extern lv_group_t* navigation_group;

LV_FONT_DECLARE(lv_font_montserrat_10);
LV_FONT_DECLARE(lv_font_unscii_8);

// Global variables for BLE connection
static BLEScan* pGattScan = nullptr;
static volatile bool gatt_scan_done = false;
static std::vector<BLEAdvertisedDevice> found_devices;

// Callback triggered by the hardware when the scan finishes
static void gattScanCompleteCB(BLEScanResults results) { gatt_scan_done = true; }

void BleGattScreen::onEnter()
{

    _entry_time = millis();
    _state = GATT_IDLE;
    found_devices.clear();

    lv_obj_t* screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);

    // Header
    _header_label = lv_label_create(screen);
    lv_label_set_text(_header_label, "BLE: GATT EXPLORER");
    lv_obj_set_style_text_color(_header_label, lv_color_white(), 0);
    lv_obj_set_style_bg_color(_header_label, lv_color_hex(0x333333), 0);
    lv_obj_set_style_bg_opa(_header_label, LV_OPA_COVER, 0);
    lv_obj_set_width(_header_label, 128);
    lv_obj_set_style_text_align(_header_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(_header_label, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_style_text_font(_header_label, &lv_font_montserrat_10, 0);

    // Target list
    _list_obj = lv_list_create(screen);
    lv_obj_set_size(_list_obj, 128, 105);
    lv_obj_align(_list_obj, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(_list_obj, lv_color_black(), 0);
    lv_obj_set_style_border_width(_list_obj, 0, 0);
    lv_obj_add_flag(_list_obj, LV_OBJ_FLAG_HIDDEN);

    // Command console
    _console_ta = lv_textarea_create(screen);
    lv_obj_set_size(_console_ta, 128, 105);
    lv_obj_align(_console_ta, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(_console_ta, lv_color_black(), 0);
    lv_obj_set_style_text_color(_console_ta, lv_color_hex(0x00FF00), 0);
    lv_obj_set_style_border_width(_console_ta, 0, 0);
    lv_textarea_set_cursor_click_pos(_console_ta, false);
    lv_obj_set_style_text_font(_console_ta, &lv_font_unscii_8, 0);

    _logMessage("GATT EXPLORER READY.\nPress ENTER to scan\nfor targets...");

    // Initialize BLE hardware
    static bool ble_initialized = false;
    if (!ble_initialized)
    {
        BLEDevice::init("RECON-UNIT");
        ble_initialized = true;
    }
    
    pGattScan = BLEDevice::getScan();
    pGattScan->setActiveScan(true);
    pGattScan->setInterval(100);
    pGattScan->setWindow(99);

    lv_obj_update_layout(screen);
    lv_obj_invalidate(screen);

}

void BleGattScreen::_logMessage(const char* msg)
{

    lv_textarea_add_text(_console_ta, msg);
    lv_textarea_add_text(_console_ta, "\n");
    // Auto-scroll to the bottom
    lv_textarea_set_cursor_pos(_console_ta, LV_TEXTAREA_CURSOR_LAST);

}

void BleGattScreen::_startScan()
{

    _state = GATT_SCANNING;
    gatt_scan_done = false;
    found_devices.clear();

    lv_obj_add_flag(_list_obj, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(_console_ta, LV_OBJ_FLAG_HIDDEN);
    
    lv_textarea_set_text(_console_ta, ""); // Clear console
    _logMessage("[*] Scanning 3s...");
    lv_label_set_text(_header_label, "BLE: SELECT TARGET");

    pGattScan->clearResults();
    pGattScan->start(3, gattScanCompleteCB, false);

}

void BleGattScreen::onLoop()
{

    // State Transition: Scanning finished -> Show device list
    if (_state == GATT_SCANNING && gatt_scan_done)
    {
        _state = GATT_SHOW_LIST;
        gatt_scan_done = false;
        _populateList();
    }

    // State Transition: User selected a device -> Connecting
    if (_state == GATT_CONNECTING)
    {
        // Force LVGL to draw the "Connecting..." message before blocking the processor
        lv_timer_handler(); 
        delay(50);
        
        _connectAndExplore(); // Blocking call to connect and fetch GATT data
        
        _state = GATT_IDLE; // Process finished, return to idle
    }

}

void BleGattScreen::_populateList()
{

    lv_obj_add_flag(_console_ta, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(_list_obj, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clean(_list_obj);
    if (navigation_group != NULL) lv_group_remove_all_objs(navigation_group);

    BLEScanResults results = pGattScan->getResults();
    int count = results.getCount();
    
    // Save discovered devices
    for (int i = 0; i < count; i++)
    {
        BLEAdvertisedDevice dev = results.getDevice(i);
        found_devices.push_back(dev);
    }

    // Limit to 10 items to prevent RAM exhaustion
    int displayCount = (found_devices.size() > 10) ? 10 : found_devices.size();

    if (displayCount == 0)
    {
        lv_obj_add_flag(_list_obj, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(_console_ta, LV_OBJ_FLAG_HIDDEN);
        _logMessage("[-] No devices found.");
        _state = GATT_IDLE;
        return;
    }

    for (int i = 0; i < displayCount; i++) 
    {
        String name = found_devices[i].haveName() ? found_devices[i].getName().c_str() : "Unknown";
        char buf[64];
        snprintf(buf, sizeof(buf), "[%d] %s", i, name.c_str());

        lv_obj_t* btn = lv_list_add_btn(_list_obj, LV_SYMBOL_BLUETOOTH, buf);
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x111111), 0);
        lv_obj_set_style_text_color(btn, lv_color_white(), 0);
        lv_obj_set_style_border_color(btn, lv_color_hex(0x00AAFF), 0);
        
        #ifdef LV_FONT_MONTSERRAT_10
        lv_obj_set_style_text_font(btn, &lv_font_montserrat_10, 0);
        #endif

        // Pass the index (i) as user data to the event handler
        lv_obj_add_event_cb(btn, _event_handler, LV_EVENT_CLICKED, (void*)(intptr_t)i);
        
        if (navigation_group != NULL) lv_group_add_obj(navigation_group, btn);
    }

    if (navigation_group != NULL) lv_group_focus_obj(lv_obj_get_child(_list_obj, 0));
    pGattScan->clearResults();

}

void BleGattScreen::_event_handler(lv_event_t * e)
{

    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED)
    {
        int index = (int)(intptr_t)lv_event_get_user_data(e);
        // Retrieve the current screen instance
        BleGattScreen* screen = (BleGattScreen*)UIManager::getInstance()->getCurrentScreen();
        
        screen->_selected_device_index = index;
        screen->_state = GATT_CONNECTING;
        
        // Prepare UI for the connection phase
        lv_obj_add_flag(screen->_list_obj, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(screen->_console_ta, LV_OBJ_FLAG_HIDDEN);
        lv_textarea_set_text(screen->_console_ta, "");
        screen->_logMessage("[*] Connecting to:");
        screen->_logMessage(found_devices[index].getAddress().toString().c_str());
        screen->_logMessage("Please wait...");
    }

}

void BleGattScreen::_connectAndExplore()
{

    BLEClient* pClient = BLEDevice::createClient();
    
    // Attempt to connect to the selected device
    if (!pClient->connect(&found_devices[_selected_device_index])) {
        _logMessage("[-] Connection FAILED.");
        return;
    }

    _logMessage("[+] CONNECTED!");
    _logMessage("[*] Enum Services:");

    // Extract the remote services map
    std::map<std::string, BLERemoteService*>* pServices = pClient->getServices();
    
    if (pServices->empty()) _logMessage("[-] No services found.");
    else {
        // Iterate through the services and print their UUIDs
        for (auto const& entry : *pServices)
        {
            String uuidStr = entry.second->getUUID().toString().c_str();
            
            char buf[64];
            snprintf(buf, sizeof(buf), "-> %s", uuidStr.c_str());
            _logMessage(buf);
            delay(50); // Short pause to avoid saturating LVGL drawing tasks
        }
    }

    _logMessage("[*] Disconnecting...");
    pClient->disconnect();
    _logMessage("[+] Disconnected. Done.");

}

void BleGattScreen::onButtonPress(int gpio)
{

    if (millis() - _entry_time < 500) return;

    if (gpio == 27)
    {
        if (_state == GATT_SCANNING) pGattScan->stop();
        UIManager::getInstance()->switchScreen(new BleMenuScreen());
    } else if (gpio == 14)
    {
        if (_state == GATT_IDLE) _startScan();
    }
    else if (gpio == 13 && _state == GATT_IDLE) lv_textarea_cursor_up(_console_ta);
    else if (gpio == 12 && _state == GATT_IDLE) lv_textarea_cursor_down(_console_ta);

}

void BleGattScreen::onExit()
{

    if (_state == GATT_SCANNING) pGattScan->stop();
    if (navigation_group != NULL) lv_group_remove_all_objs(navigation_group);

}
