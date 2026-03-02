#pragma once
#include "../../core/IScreen.h"
#include <lvgl.h>
#include <vector>
#include <BLEAdvertisedDevice.h>

enum GattState
{
    GATT_IDLE,
    GATT_SCANNING,
    GATT_SHOW_LIST,
    GATT_CONNECTING
};

class BleGattScreen : public IScreen
{

    private:

        unsigned long _entry_time = 0;
        GattState _state = GATT_IDLE;
        int _selected_device_index = -1;

        lv_obj_t* _header_label;
        lv_obj_t* _list_obj;
        lv_obj_t* _console_ta;

        void _startScan();
        void _populateList();
        void _connectAndExplore();
        void _logMessage(const char* msg);

        static void _event_handler(lv_event_t * e);

    public:

        void onEnter() override;
        void onLoop() override;
        void onExit() override;
        void onButtonPress(int gpio) override;

};