#include "CanMenuScreen.h"
#include "../UIManager.h"
#include "MainMenuScreen.h"
#include "CanMonitorScreen.h"

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
            UIManager::getInstance()->switchScreen(new CanMonitorScreen(MODE_SNIFFER));
            break;
        case OPT_DASHBOARD:
            UIManager::getInstance()->switchScreen(new CanMonitorScreen(MODE_STATS));
            break;
        case OPT_SENDER:
            UIManager::getInstance()->switchScreen(new CanMonitorScreen(MODE_SENDER));
            break;
    }

}

void CanMenuScreen::onBack()
{
    UIManager::getInstance()->switchScreen(new MainMenuScreen());
}