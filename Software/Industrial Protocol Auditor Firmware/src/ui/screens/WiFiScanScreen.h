#pragma once
#include "../IScreen.h"
#include <lvgl.h>

class WiFiScanScreen : public IScreen
{

    private:

        lv_obj_t* _header_label;
        lv_obj_t* _wifi_list;
        unsigned long _entry_time = 0;

        // Helper to create list items
        void addNetworkItem(const char* ssid, int rssi, bool encrypted);
        // Mock generator (For testing) <------- TODO: Remove this
        void simulateScan();

    public:

        void onEnter() override;
        void onLoop() override;
        void onExit() override;
        void onButtonPress(int gpio) override;

};
