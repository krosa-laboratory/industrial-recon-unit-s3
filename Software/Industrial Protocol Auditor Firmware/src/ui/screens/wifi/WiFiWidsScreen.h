#pragma once
#include "../../core/IScreen.h"
#include <lvgl.h>
#include <string>

class WiFiWidsScreen : public IScreen
{

    private:

        unsigned long _entry_time = 0;
        unsigned long _last_update = 0;
        unsigned long _last_hop_time = 0;
        int _current_channel = 1;

        // Alarm state
        bool _is_under_attack = false;
        unsigned long _last_attack_time = 0;
        unsigned long _last_deauth_count = 0;

        // UI Elements
        lv_obj_t* _header_label;
        lv_obj_t* _status_panel;
        lv_obj_t* _status_label;
        lv_obj_t* _log_ta;
        lv_obj_t* _stats_label;

        void _updateUI();
        void _performChannelHopping();

    public:

        void onEnter() override;
        void onLoop() override;
        void onExit() override;
        void onButtonPress(int gpio) override;

};