#pragma once
#include "../IScreen.h"
#include <lvgl.h>

// Internal states in this screen
enum CanScreenMode
{
    MODE_SNIFFER,
    MODE_STATS,
    MODE_SENDER
};

/**
 * @brief Main screen for CAN Bus auditory.
 * * Handles three sub-views:
 * 1. Sniffer: Shows payloads in real time.
 * 2. Dashboard: Shows bus load and error.
 * 3. Sender: Allows payload inyection.
 */
class CanMonitorScreen : public IScreen
{

    private:

        // Internal state
        CanScreenMode _current_mode = MODE_SNIFFER;
        bool _is_paused = false;
        unsigned long _last_update = 0;
        unsigned long _entry_time = 0;

        // GUI Elements
        lv_obj_t* _header_label;
        lv_obj_t* _sniffer_view;
        lv_obj_t* _stats_view;
        lv_obj_t* _terminal_text;
        lv_obj_t* _bar_bus_load;
        lv_obj_t* _label_error_count;
        lv_obj_t* _label_unique_ids;

        // Private Methods
        void buildUiSniffer();
        void buildUiStats();
        void updateSniffer();
        void updateStats();
        void toggleMode();

    public:

        void onEnter() override;
        void onLoop() override;
        void onExit() override;
        void onButtonPress(int gpio) override;

};
