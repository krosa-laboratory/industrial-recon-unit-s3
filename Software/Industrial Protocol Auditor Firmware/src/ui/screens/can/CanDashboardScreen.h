#pragma once
#include "../../core/IScreen.h"
#include <lvgl.h>

class CanDashboardScreen : public IScreen
{

    private:

        unsigned long _entry_time = 0;
        unsigned long _last_update = 0;

        lv_obj_t* _header_label;
        lv_obj_t* _bar_bus_load;
        lv_obj_t* _label_error_count;
        lv_obj_t* _label_unique_ids;

        void updateStats();

    public:

        void onEnter() override;
        void onLoop() override;
        void onExit() override;
        void onButtonPress(int gpio) override;

};
