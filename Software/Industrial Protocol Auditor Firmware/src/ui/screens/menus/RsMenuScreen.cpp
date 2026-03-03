#include "RsMenuScreen.h"
#include "../../core/UIManager.h"
#include "MainMenuScreen.h"

#include "../rs/RsSnifferScreen.h"
#include "../rs/ModbusScannerScreen.h"

enum { OPT_SNIFFER, OPT_MODBUS };

void RsMenuScreen::buildOptions()
{
    _options.push_back({"Raw Sniffer",    OPT_SNIFFER});
    _options.push_back({"Modbus Scanner", OPT_MODBUS});
}

void RsMenuScreen::onSelect(int id)
{

    switch(id)
    {
        case OPT_SNIFFER:
            UIManager::getInstance()->switchScreen(new RsSnifferScreen());
            break;
        case OPT_MODBUS:
            UIManager::getInstance()->switchScreen(new ModbusScannerScreen());
            break;
    }

}

void RsMenuScreen::onBack() { UIManager::getInstance()->switchScreen(new MainMenuScreen()); }
