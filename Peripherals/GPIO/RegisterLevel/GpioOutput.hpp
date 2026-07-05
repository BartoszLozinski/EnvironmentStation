#pragma once
#include "GpioBase.hpp"

namespace Peripherals
{
    namespace RegisterLevel
    {
        template<uint8_t pin_>
        class GpioOutput : public GpioBase<GPIO_TypeDef, pin_>
        {
            using GpioBase<GPIO_TypeDef, pin_>::port;
            using GpioBase<GPIO_TypeDef, pin_>::pin;

        private:
            void ConfigureAsOutput(const OptionsOTYPER otyperOption = GpioDefaults::otyperOption
                , const OptionsOSPEEDR ospeedrOption = GpioDefaults::ospeedrOption
                , const OptionsPUPDR pupdrOption = GpioDefaults::pupdrOption)
            {
                this->template ConfigureMODER(OptionsMODER::Output);
                this->template ConfigureOTYPER(otyperOption);
                this->template ConfigureOSPEEDR(ospeedrOption);
                this->template ConfigurePUPDR(pupdrOption);
            }

        public:
            GpioOutput(const GpioOutput& source) = delete;
            GpioOutput(GpioOutput&& source) = delete;
            GpioOutput& operator=(const GpioOutput& source) = delete;
            GpioOutput& operator=(GpioOutput&& source) = delete;
            GpioOutput(GPIO_TypeDef* const port_
                      , const OptionsOTYPER otyperOption = GpioDefaults::otyperOption
                      , const OptionsOSPEEDR ospeedrOption = GpioDefaults::ospeedrOption
                      , const OptionsPUPDR pupdrOption = GpioDefaults::pupdrOption)
                : GpioBase<GPIO_TypeDef, pin_>(port_)
            {
                ConfigureAsOutput(otyperOption, ospeedrOption, pupdrOption);
            }
            ~GpioOutput() = default;

            //Output data register ODR - reference manual 8.5.6
            bool IsPinSet() const { return port->ODR & PinMask<pin>(); }
            void Set() { port->BSRR |= (0b1 << pin); }
            void Clear()
            {
                static constexpr uint8_t bitShiftOffset = 16;
                static constexpr uint8_t bitShift = bitShiftOffset + pin; //bit resets are 16-32
                port->BSRR |= (0b1 << bitShift);
            }
            void Toggle() { port->ODR ^= (0b1 << pin); /*Bitwise XOR*/} //ODR has 16 bits
        };
    }
}
