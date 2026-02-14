#pragma once
#include "../IScreen.h"
#include <lvgl.h>

class MainMenuScreen : public IScreen {

    private:

        static void event_handler(lv_event_t * e);

    public:

        void onEnter() override;
        void onLoop() override;
        void onExit() override;
        void onButtonPress(int gpio) override;

};