#pragma once
#include "../../core/BaseMenuScreen.h"

class WiFiMenuScreen : public BaseMenuScreen
{

    public:

        WiFiMenuScreen() : BaseMenuScreen("WIFI TOOLS") {}

    protected:

        void buildOptions() override;
        void onSelect(int id) override;
        void onBack() override;

};