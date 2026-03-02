#include "WiFiMenuScreen.h"
#include "../../core/UIManager.h"
#include "MainMenuScreen.h"

// Pantallas a las que vamos a llamar
#include "../wifi/WiFiScannerScreen.h"
#include "../wifi/WiFiMonitorScreen.h"
#include "../wifi/WiFiWidsScreen.h"

// IDs locales para este menú
enum { OPT_SCANNER, OPT_MONITOR, OPT_WIDS };

void WiFiMenuScreen::buildOptions() {
    _options.push_back({"WiFi Scanner",   OPT_SCANNER});
    _options.push_back({"Packet Monitor", OPT_MONITOR});
    _options.push_back({"WIDS Detector",  OPT_WIDS});
}

void WiFiMenuScreen::onSelect(int id) {
    switch(id) {
        case OPT_SCANNER:
            UIManager::getInstance()->switchScreen(new WiFiScannerScreen());
            break;
        case OPT_MONITOR:
            UIManager::getInstance()->switchScreen(new WiFiMonitorScreen());
            break;
        case OPT_WIDS:
            UIManager::getInstance()->switchScreen(new WiFiWidsScreen());
            break;
    }
}

void WiFiMenuScreen::onBack() {
    // Al pulsar BACK (físico o visual), volvemos al menú principal
    UIManager::getInstance()->switchScreen(new MainMenuScreen());
}
