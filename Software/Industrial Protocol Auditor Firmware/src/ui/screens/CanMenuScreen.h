#pragma once
#include "BaseMenuScreen.h"

class CanMenuScreen : public BaseMenuScreen
{

    public:

        CanMenuScreen() : BaseMenuScreen("CAN TOOLS") {}

    protected:

        void buildOptions() override;
        void onSelect(int id) override;
        void onBack() override;

};
