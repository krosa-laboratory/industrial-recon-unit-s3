#pragma once
#include "../../core/BaseMenuScreen.h"

class RsMenuScreen : public BaseMenuScreen
{

    public:

        RsMenuScreen() : BaseMenuScreen("RS485 TOOLS") {}

    protected:

        void buildOptions() override;
        void onSelect(int id) override;
        void onBack() override;

};
