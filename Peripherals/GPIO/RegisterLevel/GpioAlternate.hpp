#pragma once
#include "GpioBase.hpp"

namespace Peripherals
{
    namespace RegisterLevel
    {
        template<uint8_t pin_>
        class GpioAlternate : public GpioBase<GPIO_TypeDef, pin_>
        {
        protected:
        public:
            GpioAlternate(const GpioAlternate& source) = default;
            GpioAlternate(GpioAlternate&& source) = default;
            GpioAlternate& operator=(const GpioAlternate& source) = delete;
            GpioAlternate& operator=(GpioAlternate&& source) = delete;
            GpioAlternate(GPIO_TypeDef* const port_
                        , Gpio::AlternateFunction alternateFunction
                        , Gpio::OTYPER otyperOption = GpioDefaults::otyperOption
                        , Gpio::OSPEEDR ospeedrOption = GpioDefaults::ospeedrOption
                        , Gpio::PUPDR pupdrOption = GpioDefaults::pupdrOption)
                : GpioBase<GPIO_TypeDef, pin_>(port_)
            {
                this->template ConfigureMODER(Gpio::MODER::Alternate);
                this->template ConfigureOTYPER(otyperOption);
                this->template ConfigureOSPEEDR(ospeedrOption);
                this->template ConfigurePUPDR(pupdrOption);
                this->template ConfigureAlternateFunction(alternateFunction);
            }
        };
    }
}
