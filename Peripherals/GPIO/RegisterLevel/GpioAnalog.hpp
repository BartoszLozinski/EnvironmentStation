#pragma once
#include "GpioBase.hpp"

namespace Peripherals
{
    namespace RegisterLevel
    {
        template<uint8_t pin_>
        class GpioAnalog : public GpioBase<GPIO_TypeDef, pin_>
        {
            using GpioBase<GPIO_TypeDef, pin_>::port;
            using GpioBase<GPIO_TypeDef, pin_>::pin;

        protected:
            void ConfigureAsAnalog()
            {
                static_assert(pin >= 0 && pin <= 15, "Invalid pin number: needs to be in range of 0 - 15!");
                this->EnableClock();
                this-> template ConfigureMODER(Gpio::MODER::Analog);
                this-> template ConfigureOSPEEDR(Gpio::OSPEEDR::LowSpeed);
                this-> template ConfigurePUPDR(Gpio::PUPDR::None);
                port->ASCR |= GPIO_ASCR_ASC[pin];
            }

        public:
            GpioAnalog(const GpioAnalog& source) = default;
            GpioAnalog(GpioAnalog&& source) = default;
            GpioAnalog& operator=(const GpioAnalog& source) = delete;
            GpioAnalog& operator=(GpioAnalog&& source) = delete;
            GpioAnalog(GPIO_TypeDef* const port_)
                : GpioBase<GPIO_TypeDef, pin_>(port_)
            {
                ConfigureAsAnalog();
            }
        };
    }
}
