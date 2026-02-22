#pragma once
#include "../../core/IScreen.h"
#include <lvgl.h>

class CanSnifferScreen : public IScreen
{

    private:
        
        unsigned long _entry_time = 0;
        unsigned long _last_update = 0;
        bool _is_paused = false;

        lv_obj_t* _header_label;
        lv_obj_t* _terminal_text;

        void _updateSniffer();

    public:

        void onEnter() override;
        void onLoop() override;
        void onExit() override;
        void onButtonPress(int gpio) override;
};
