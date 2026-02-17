#include <Arduino.h>
#include "CanMonitorScreen.h"
#include "../UIManager.h"
#include "MainMenuScreen.h"

// Design constants.
#define COLOR_MATRIX lv_color_hex(0x00FF00)
#define COLOR_WARNING lv_color_hex(0xFFAA00)
#define COLOR_ERROR lv_color_hex(0xFF0000)
#define DATA_REFRESH_RATE 150 // ms

void CanMonitorScreen::onEnter()
{

    // Prepare the screen.
    _entry_time = millis();
    lv_obj_t* screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);
    // Common main header.
    _header_label = lv_label_create(screen);
    lv_label_set_text(_header_label, "CAN: 500kbps [ACTIVE]");
    lv_obj_set_style_text_color(_header_label, lv_color_white(), 0);
    lv_obj_set_style_bg_color(_header_label, lv_color_hex(0x333333), 0);
    lv_obj_set_style_bg_opa(_header_label, LV_OPA_COVER, 0);
    lv_obj_set_width(_header_label, 132);
    lv_obj_set_style_text_align(_header_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(_header_label, LV_ALIGN_TOP_MID, 0, 0);
    // Build views.
    buildUiSniffer();
    buildUiStats();
    // Set default value for variables
    _is_paused = false;
    _last_update = millis();
    // Default state.
    _current_mode = MODE_SNIFFER;
    lv_obj_clear_flag(_sniffer_view, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(_stats_view, LV_OBJ_FLAG_HIDDEN);
    // Force display update
    lv_obj_update_layout(screen);
    lv_obj_invalidate(screen);

}

void CanMonitorScreen::onLoop()
{

    static unsigned long debug_timer = 0;
    if (millis() - debug_timer > 1000) {
        debug_timer = millis();
        Serial.printf(">>> DEBUG: onLoop vivo. Mode: %d, Paused: %d, Heap: %d bytes\n", _current_mode, _is_paused, ESP.getFreeHeap());
    }

    if (millis() - _last_update > DATA_REFRESH_RATE)
    {

        _last_update = millis();
        // Update by actual mode.
        if (_current_mode == MODE_SNIFFER && !_is_paused) updateSniffer();
        else if (_current_mode == MODE_STATS) updateStats();

    }

}

void CanMonitorScreen::onExit() {}

void CanMonitorScreen::onButtonPress(int gpio)
{

    if (millis() - _entry_time < 500) return;

    if (gpio == 27) // BUTTON BACK.
    {
        UIManager::getInstance()->switchScreen(new MainMenuScreen());
        return;
    }

    if (gpio == 14) // BUTTON ENTER.
        toggleMode();

    if (_current_mode == MODE_SNIFFER) // FOR UP/DOWN BUTTONS.
    {
        if (gpio == 13) lv_textarea_cursor_up(_terminal_text);
        if (gpio == 12) lv_textarea_cursor_down(_terminal_text);
    }

}

void CanMonitorScreen::buildUiSniffer()
{

    // Create a transparent container.
    _sniffer_view = lv_obj_create(lv_scr_act());
    lv_obj_set_size(_sniffer_view, 128, 110);
    lv_obj_align(_sniffer_view, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_opa(_sniffer_view, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(_sniffer_view, 0, 0);
    lv_obj_set_style_pad_all(_sniffer_view, 0, 0);
    // Create text terminal.
    _terminal_text = lv_textarea_create(_sniffer_view);
    lv_obj_set_size(_terminal_text, 128, 110);
    lv_obj_align(_terminal_text, LV_ALIGN_CENTER, 0, 0);
    // Gives style to the terminal (Matrix style).
    lv_obj_set_style_bg_color(_terminal_text, lv_color_black(), 0);
    lv_obj_set_style_text_color(_terminal_text, COLOR_MATRIX, 0);
    lv_obj_set_style_border_width(_terminal_text, 0, 0);
    //lv_obj_set_style_text_font(_terminal_text, &lv_font_montserrat_14, 0);
    lv_textarea_set_cursor_click_pos(_terminal_text, false);
    lv_textarea_set_text(_terminal_text, ">> RECON-UNIT LISTENING...\n");

}

void CanMonitorScreen::buildUiStats()
{

    // Create a container.
    _stats_view = lv_obj_create(lv_scr_act());
    lv_obj_set_size(_stats_view, 128, 110);
    lv_obj_align(_stats_view, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(_stats_view, lv_color_hex(0x101010), 0);
    lv_obj_set_style_border_width(_stats_view, 0, 0);
    // Bus load bar.
    lv_obj_t* labelLoad = lv_label_create(_stats_view);
    lv_label_set_text(labelLoad, "BUS LOAD:");
    lv_obj_align(labelLoad, LV_ALIGN_TOP_LEFT, 5, 5);
    lv_obj_set_style_text_color(labelLoad, lv_color_white(), 0);
    _bar_bus_load = lv_bar_create(_stats_view);
    lv_obj_set_size(_bar_bus_load, 118, 10);
    lv_obj_align(_bar_bus_load, LV_ALIGN_TOP_MID, 0, 25);
    lv_bar_set_range(_bar_bus_load, 0, 100);
    lv_bar_set_value(_bar_bus_load, 0, LV_ANIM_ON);
    lv_obj_set_style_bg_color(_bar_bus_load, COLOR_MATRIX, LV_PART_INDICATOR);
    // Error counters.
    _label_error_count = lv_label_create(_stats_view);
    lv_label_set_text(_label_error_count, "ERRORS: 0");
    lv_obj_align(_label_error_count, LV_ALIGN_LEFT_MID, 5, 10);
    lv_obj_set_style_text_color(_label_error_count, COLOR_ERROR, 0);
    // Unique IDs.
    _label_unique_ids = lv_label_create(_stats_view);
    lv_label_set_text(_label_unique_ids, "UNIQUE IDs: 0");
    lv_obj_align(_label_unique_ids, LV_ALIGN_LEFT_MID, 5, 30);
    lv_obj_set_style_text_color(_label_unique_ids, lv_color_hex(0x00AAFF), 0);

}

void CanMonitorScreen::updateSniffer()
{

    // Generate fake paylaod for testing <---------- TODO: Remove this.
    int id = (esp_random() % (0x400 - 0x100)) + 0x100;
    int data_1 = esp_random() % 256;
    int data_2 = esp_random() % 256;
    // Give it format for the display.
    char buffer[32];
    sprintf(buffer, "ID:%03X [%02X %02X..]\n", id, data_1, data_2);
    // Add to the terminal.
    lv_textarea_add_text(_terminal_text, buffer);
    // Clean the buffer to avoid RAM problems.
    const char* text = lv_textarea_get_text(_terminal_text);
    if (strlen(text) > 500)
    {
        lv_textarea_set_text(_terminal_text, ""); // Drastic cleaning by the moment.
        lv_textarea_add_text(_terminal_text, ">> BUFFER CLEARED\n");
    }

}

void CanMonitorScreen::updateStats()
{

    // Emulate load in the bus for testing <---------- TODO: Remove this.
    static int load = 0;
    load = (load + (esp_random() % 15) - 5);
    if (load < 0) load = 0;
    if (load > 100) load = 100;
    // Store value of bar load
    lv_bar_set_value(_bar_bus_load, load, LV_ANIM_ON);
    // Change bar color by load
    if (load > 80) lv_obj_set_style_bg_color(_bar_bus_load, COLOR_ERROR, LV_PART_INDICATOR);
    else if (load > 50) lv_obj_set_style_bg_color(_bar_bus_load, COLOR_WARNING, LV_PART_INDICATOR);
    else lv_obj_set_style_bg_color(_bar_bus_load, COLOR_MATRIX, LV_PART_INDICATOR);
    // Simulate counters
    static int errors = 0;
    static int ids_num = 0;
    if ((esp_random() % 100) > 95) errors++;
    if ((esp_random() % 100) > 80) ids_num++;
    // Store values for counters
    lv_label_set_text_fmt(_label_error_count, "ERRORS: %d", errors);
    lv_label_set_text_fmt(_label_unique_ids, "UNIQUE IDs: %d", ids_num);

}

void CanMonitorScreen::toggleMode()
{

    if (_current_mode == MODE_SNIFFER)
    {
        _current_mode = MODE_STATS;
        lv_obj_add_flag(_sniffer_view, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(_stats_view, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(_header_label, "CAN: DASHBOARD");
    } else {
        _current_mode = MODE_SNIFFER;
        lv_obj_add_flag(_stats_view, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(_sniffer_view, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(_header_label, "CAN: SNIFFER LOG");
    }

}
