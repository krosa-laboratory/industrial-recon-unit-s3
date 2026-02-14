#pragma once
#include <lvgl.h>
#include "IScreen.h"

/**
 * Singleton manager for the application's UI state.
 * Orchestrates screen transitions and dispatches global events.
 *
 * Key Responsibilities:
 * - Maintains the currently active IScreen instance.
 * - Handles memory management during screen switches.
 * - Routes input and loop events to the active screen.
 */

class UIManager
{

    private:

        // Pointer to the current active screen.
        IScreen* _current_screen = nullptr;
        // The only instance of the manager (Singleton).
        static UIManager* _instance;

    public:

        // Static method to return the manager from anywhere in the code.
        static UIManager* getInstance();
        // Setup method for the manager.
        void setup();
        // Method to call every cycle in the loop to update the active screen.
        void update();
        // Method to switch from one screen to another cleaning the memory.
        void switchScreen(IScreen* new_screen);
        // Handles the button activation and gives them to the active screen.
        void handleInput(int button_gpio);

};
