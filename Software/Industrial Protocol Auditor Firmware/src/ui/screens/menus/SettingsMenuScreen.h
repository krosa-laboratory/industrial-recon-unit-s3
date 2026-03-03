#pragma once
#include "../../core/BaseMenuScreen.h"

class SettingsMenuScreen : public BaseMenuScreen
{

    public:

        SettingsMenuScreen() : BaseMenuScreen("SYSTEM SETTINGS") {}

    protected:

        void buildOptions() override;
        void onSelect(int id) override;
        void onBack() override;

};