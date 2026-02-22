#include "CanMenuScreen.h"
#include "../../core/UIManager.h"
#include "MainMenuScreen.h"
#include "../can/CanSnifferScreen.h" 
#include "../can/CanDashboardScreen.h"

// Local IDs for this menu
enum { OPT_SNIFFER, OPT_DASHBOARD, OPT_SENDER };

void CanMenuScreen::buildOptions()
{

    _options.push_back({"Sniffer Log",     OPT_SNIFFER});
    _options.push_back({"Dashboard Stats", OPT_DASHBOARD});
    _options.push_back({"Packet Sender",   OPT_SENDER});

}

void CanMenuScreen::onSelect(int id)
{

    switch(id)
    {
        case OPT_SNIFFER:
            UIManager::getInstance()->switchScreen(new CanSnifferScreen());
            break;
        case OPT_DASHBOARD:
            UIManager::getInstance()->switchScreen(new CanDashboardScreen());
            break;
        case OPT_SENDER:
            //UIManager::getInstance()->switchScreen(new CanMonitorScreen(MODE_SENDER)); NOT IMPLEMENTED YET
            break;
    }

}

void CanMenuScreen::onBack()
{
    UIManager::getInstance()->switchScreen(new MainMenuScreen());
}
