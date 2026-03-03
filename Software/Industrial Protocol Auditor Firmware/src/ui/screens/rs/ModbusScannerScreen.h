#pragma once
#include "../../core/IScreen.h"
#include <lvgl.h>

class ModbusScannerScreen : public IScreen
{

    private:

        unsigned long _entry_time = 0;
        unsigned long _last_scan_time = 0;
        
        bool _is_scanning = false;
        int _current_slave_id = 1;
        int _devices_found = 0;

        lv_obj_t* _header_label;
        lv_obj_t* _status_label;
        lv_obj_t* _progress_bar;
        lv_obj_t* _slave_list;

        void _startScan();
        void _addFoundSlave(int slave_id);

    public:

        void onEnter() override;
        void onLoop() override;
        void onExit() override;
        void onButtonPress(int gpio) override;

};