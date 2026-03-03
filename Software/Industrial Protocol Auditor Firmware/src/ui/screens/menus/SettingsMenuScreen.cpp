#include <esp_system.h>
#include "SettingsMenuScreen.h"
#include "../../core/UIManager.h"
#include "MainMenuScreen.h"
#include "../settings/SystemInfoScreen.h"

enum { OPT_SYS_INFO, OPT_REBOOT };

void SettingsMenuScreen::buildOptions()
{
    _options.push_back({"System Info", OPT_SYS_INFO});
    _options.push_back({"Reboot Unit", OPT_REBOOT});
}

void SettingsMenuScreen::onSelect(int id)
{

    switch(id)
    {
        case OPT_SYS_INFO:
            UIManager::getInstance()->switchScreen(new SystemInfoScreen());
            break;
        case OPT_REBOOT:
            esp_restart(); 
            break;
    }

}

void SettingsMenuScreen::onBack() { UIManager::getInstance()->switchScreen(new MainMenuScreen()); }
