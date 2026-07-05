#pragma once
#include "GpioRegisterLevelBase.hpp"

namespace Peripherals
{
    namespace RegisterLevel
    {
        template<GpioPort Port, uint8_t pin_>
        class GpioAlternate : public GpioRegisterLevelBase<GpioAlternate<GPIO_TypeDef, pin_>>
        {
        protected:
        public:
            volatile Port* const port = nullptr;
            static constexpr uint8_t pin = pin_;
            GpioAlternate(const GpioAlternate& source) = default;
            GpioAlternate(GpioAlternate&& source) = default;
            GpioAlternate& operator=(const GpioAlternate& source) = delete;
            GpioAlternate& operator=(GpioAlternate&& source) = delete;
            GpioAlternate(Port* const port_
                        , AlternateFunction alternateFunction
                        , OptionsOTYPER otyperOption = GpioDefaults::otyperOption
                        , OptionsOSPEEDR ospeedrOption = GpioDefaults::ospeedrOption
                        , OptionsPUPDR pupdrOption = GpioDefaults::pupdrOption)
                : port(port_)
            {
                this->EnableClock();
                this->template ConfigureMODER(OptionsMODER::Alternate);
                this->template ConfigureOTYPER(otyperOption);
                this->template ConfigureOSPEEDR(ospeedrOption);
                this->template ConfigurePUPDR(pupdrOption);
                this->template ConfigureAlternateFunction(alternateFunction);
            }
        };
    }
}
