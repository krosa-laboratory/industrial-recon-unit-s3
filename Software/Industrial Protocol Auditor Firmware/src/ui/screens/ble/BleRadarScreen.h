#pragma once
#include "../../core/IScreen.h"
#include <lvgl.h>

class BleRadarScreen : public IScreen
{

    private:

        unsigned long _entry_time = 0;
        bool _is_scanning = false;

        lv_obj_t* _header_label;
        lv_obj_t* _ble_list;

        void _startScan();
        void _addDeviceItem(const char* name, const char* mac, int rssi);

    public:

        void onEnter() override;
        void onLoop() override;
        void onExit() override;
        void onButtonPress(int gpio) override;

};