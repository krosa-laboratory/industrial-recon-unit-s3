#pragma once
#include "../../core/IScreen.h"
#include <lvgl.h>

class WiFiMonitorScreen : public IScreen
{

    private:

        unsigned long _entry_time = 0;
        unsigned long _last_update = 0;
        unsigned long _last_hop_time = 0;
        int _current_channel = 1;
        
        // Variables to calculate the speed in packets per second
        unsigned long _last_mgmt_count = 0;
        unsigned long _last_data_count = 0;

        // UI Elements
        lv_obj_t* _header_label;
        lv_obj_t* _chart_traffic;
        lv_chart_series_t* _series_mgmt;
        lv_chart_series_t* _series_data;
        lv_obj_t* _label_channel;
        lv_obj_t* _label_stats;

        void _updateUI();
        void _performChannelHopping();

    public:

        void onEnter() override;
        void onLoop() override;
        void onExit() override;
        void onButtonPress(int gpio) override;
};
