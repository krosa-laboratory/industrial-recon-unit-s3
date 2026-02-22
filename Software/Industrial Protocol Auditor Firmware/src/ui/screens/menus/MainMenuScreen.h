#pragma once
#include "../../core/BaseMenuScreen.h"

class MainMenuScreen : public BaseMenuScreen
{

    public:

        MainMenuScreen() : BaseMenuScreen("RECON-UNIT v1.0") {} // Title

    protected:

        void buildOptions() override;
        void onSelect(int id) override;
        void onBack() override {} // In the main menu, BACK button does nothing

};