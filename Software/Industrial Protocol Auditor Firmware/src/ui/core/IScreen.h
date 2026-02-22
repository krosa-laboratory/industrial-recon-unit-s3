#pragma once

/**
 * Enforces a standard lifecycle (Enter -> Loop -> Exit) for the UIManager.
 * All specific screens (Menu, WiFi, CAN) must inherit from this class.
 *
 * Lifecycle:
 * - onEnter(): Called once when the screen becomes active.
 * - onLoop(): Called repeatedly in the main loop.
 * - onExit(): Called once before switching to a new screen.
 */

class IScreen
{

    public:

        // Virtual destructor
        virtual ~IScreen() {}

        // Methods that must exist
        virtual void onEnter() = 0;
        virtual void onLoop() = 0;
        virtual void onExit() = 0;

        // Optional Methods
        virtual void onButtonPress(int gpio) {}

};
