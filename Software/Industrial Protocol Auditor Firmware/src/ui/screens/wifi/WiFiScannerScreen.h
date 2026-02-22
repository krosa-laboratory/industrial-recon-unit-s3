#pragma once
#include "../../core/IScreen.h"
#include <lvgl.h>

class WiFiScannerScreen : public IScreen
{

    private:

        unsigned long _entry_time = 0;
        bool _is_scanning = false;

        lv_obj_t* _header_label;
        lv_obj_t* _wifi_list;

        void _startScan();
        void _processScanResults(int networksFound);
        void _addNetworkItem(const char* ssid, int rssi, bool encrypted);

    public:

        void onEnter() override;
        void onLoop() override;
        void onExit() override;
        void onButtonPress(int gpio) override;
};
