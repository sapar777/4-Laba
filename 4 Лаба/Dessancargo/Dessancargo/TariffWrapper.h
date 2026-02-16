// TariffWrapper.h
#pragma once

using namespace System;
#include "TariffWrapper.h"
namespace DessanCargoWinForms {
    public ref class TariffWrapper {
    public:
        property String^ Name;
        property double BasePrice;
        property String^ StrategyType;
        property double DiscountPercent;

        TariffWrapper() {
            Name = "";
            BasePrice = 0;
            StrategyType = "STANDARD";
            DiscountPercent = 0;
        }
    };
}
