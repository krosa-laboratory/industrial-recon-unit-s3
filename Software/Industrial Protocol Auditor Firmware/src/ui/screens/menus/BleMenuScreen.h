#pragma once
#include "../../core/BaseMenuScreen.h"

class BleMenuScreen : public BaseMenuScreen
{

    public:

        BleMenuScreen() : BaseMenuScreen("BLE TOOLS") {}

    protected:

        void buildOptions() override;
        void onSelect(int id) override;
        void onBack() override;

};