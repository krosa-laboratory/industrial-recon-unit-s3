#include "BaseMenuScreen.h"
#include "../UIManager.h"

extern lv_group_t* navigation_group;

BaseMenuScreen::BaseMenuScreen(const char* title) : _title(title) {}

void BaseMenuScreen::onEnter()
{

    lv_obj_t* screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);
    // Common header for options listed
    _header_label = lv_label_create(screen);
    lv_label_set_text(_header_label, _title);
    lv_obj_align(_header_label, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_style_text_color(_header_label, lv_color_white(), 0);
    lv_obj_set_style_bg_color(_header_label, lv_color_hex(0x333333), 0);
    lv_obj_set_style_width(_header_label, 128, 0);
    lv_obj_set_style_text_align(_header_label, LV_TEXT_ALIGN_CENTER, 0);
    // Create the list of options
    _list_obj = lv_list_create(screen);
    lv_obj_set_size(_list_obj, 128, 105);
    lv_obj_align(_list_obj, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(_list_obj, lv_color_black(), 0);
    lv_obj_set_style_border_width(_list_obj, 0, 0);
    // Fill options defined by child
    _options.clear();
    buildOptions(); 
    for (const auto& opt : _options)
    {
        lv_obj_t* button = lv_list_add_btn(_list_obj, NULL, opt.label);
        // Styles
        lv_obj_set_style_bg_color(button, lv_color_hex(0x222222), 0);
        lv_obj_set_style_text_color(button, lv_color_white(), 0);
        lv_obj_set_style_border_width(button, 0, 0);
        // Callback passing the ID of the button
        lv_obj_add_event_cb(button, event_handler, LV_EVENT_CLICKED, (void*)(intptr_t)opt.id);
        // Add the navigation group
        if (navigation_group != NULL) lv_group_add_obj(navigation_group, button);
    }
    // Focus on the first element
    if (navigation_group != NULL && lv_obj_get_child_cnt(_list_obj) > 0)
        lv_group_focus_obj(lv_obj_get_child(_list_obj, 0));

}

void BaseMenuScreen::event_handler(lv_event_t * e)
{

    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED)
    {
        // Get the actual instance screen and the button ID
        BaseMenuScreen* screen = (BaseMenuScreen*)UIManager::getInstance()->getCurrentScreen();
        int id = (int)(intptr_t)lv_event_get_user_data(e);
        // Launch the corresponding callback
        if (screen) screen->onSelect(id);
    }

}

void BaseMenuScreen::onExit()
{
    if (navigation_group != NULL) lv_group_remove_all_objs(navigation_group);
}

void BaseMenuScreen::onButtonPress(int gpio)
{
    if (gpio == 27) onBack(); // BACK button
}