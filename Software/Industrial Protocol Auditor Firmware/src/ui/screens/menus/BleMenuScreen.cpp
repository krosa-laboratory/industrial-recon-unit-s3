#include "BleMenuScreen.h"
#include "../../core/UIManager.h"
#include "MainMenuScreen.h"
#include "../ble/BleRadarScreen.h"

enum { OPT_RADAR, OPT_GATT };

void BleMenuScreen::buildOptions()
{

    _options.push_back({"BLE Radar", OPT_RADAR});
    // _options.push_back({"GATT Explorer", OPT_GATT});

}

void BleMenuScreen::onSelect(int id)
{

    switch(id)
    {
        case OPT_RADAR:
            UIManager::getInstance()->switchScreen(new BleRadarScreen());
            break;
        case OPT_GATT:
            // UIManager::getInstance()->switchScreen(new BleGattScreen());
            break;
    }

}

void BleMenuScreen::onBack()
{
    UIManager::getInstance()->switchScreen(new MainMenuScreen());
}