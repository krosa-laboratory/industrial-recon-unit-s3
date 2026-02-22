#include "MainMenuScreen.h"
#include "../../core/UIManager.h"
#include "CanMenuScreen.h"
#include "WiFiMenuScreen.h"

enum { OPT_CAN_TOOLS, OPT_WIFI_TOOLS, OPT_SETTINGS };

void MainMenuScreen::buildOptions()
{

    _options.push_back({"CAN Bus Tools", OPT_CAN_TOOLS});
    _options.push_back({"WiFi Tools",    OPT_WIFI_TOOLS});
    _options.push_back({"Settings",      OPT_SETTINGS});

}

void MainMenuScreen::onSelect(int id)
{

    switch(id)
    {
        case OPT_CAN_TOOLS:
            UIManager::getInstance()->switchScreen(new CanMenuScreen());
            break;
        case OPT_WIFI_TOOLS:
            UIManager::getInstance()->switchScreen(new WiFiMenuScreen());
            break;
    }

}