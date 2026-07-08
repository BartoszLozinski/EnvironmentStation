#pragma once

namespace Peripherals
{
    class IGpioInput
    {
    public:
        virtual uint32_t Read() const = 0;
        virtual ~IGpioInput() = default;
    };

    /*
    template<typename Implementation>
    class IGpioInputIT : public IGpioInput<Implementation>
    {
    public:
        // TO DO
        // interrupts
    };
    */
}