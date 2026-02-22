#pragma once
#include "../IScreen.h"
#include <lvgl.h>
#include <vector>
#include <string>

// Basic structure to define an option
struct MenuOption
{
    const char* label;
    int id;
};

class BaseMenuScreen : public IScreen
{

    protected:

        lv_obj_t* _header_label;
        lv_obj_t* _list_obj;
        const char* _title;

        // Option list for the child
        std::vector<MenuOption> _options;

        // Child methods to be implemented
        virtual void buildOptions() = 0;
        virtual void onSelect(int id) = 0;
        virtual void onBack() = 0;

        // Internal helper
        static void event_handler(lv_event_t * e);

    public:

        BaseMenuScreen(const char* title);
        
        void onEnter() override;
        void onLoop() override {} // By default the loop does nothing
        void onExit() override;
        void onButtonPress(int gpio) override;

};
