#include "UIManager.h"

UIManager* UIManager::_instance = nullptr;

// Static method to return the manager from anywhere in the code.
UIManager* UIManager::getInstance()
{

    if (_instance == nullptr) _instance = new UIManager();
    return _instance;

}

// Setup method for the manager.
void UIManager::setup() {}

// Method to call every cycle in the loop to update the active screen.
void UIManager::update()
{

    if (_current_screen != nullptr)
        _current_screen->onLoop();

}

// Method to switch from one screen to another cleaning the memory.
void UIManager::switchScreen(IScreen* new_screen)
{

    // Clean the old screen
    if (_current_screen != nullptr)
    {

        _current_screen->onExit();
        delete _current_screen;

    }

    // Clean LVGL
    lv_obj_clean(lv_scr_act());
    // Switch to the new screen
    _current_screen = new_screen;
    // Boot the new screen
    if (_current_screen != nullptr)
        _current_screen->onEnter();

}

// Handles the button activation and gives them to the active screen.
void UIManager::handleInput(int button_gpio)
{

    if (_current_screen != nullptr)
        _current_screen->onButtonPress(button_gpio);

}
