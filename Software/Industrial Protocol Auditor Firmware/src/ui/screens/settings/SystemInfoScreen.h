#pragma once
#include "../../core/IScreen.h"
#include <lvgl.h>

class SystemInfoScreen : public IScreen
{

    private:

        unsigned long _entry_time = 0;
        unsigned long _last_update = 0;

        lv_obj_t* _header_label;
        lv_obj_t* _info_ta;

        void _updateInfo();

    public:

        void onEnter() override;
        void onLoop() override;
        void onExit() override;
        void onButtonPress(int gpio) override;

};
