#pragma once

namespace Peripherals
{
    enum class GpioOutputState
    {
        Set,
        Reset,
    };

    class IGpioOutput
    {
    public:
        virtual void Set() = 0;
        virtual void Clear() = 0;
        virtual void Toggle() = 0;
        virtual GpioOutputState GetState() const = 0;

        virtual ~IGpioOutput() = default;
    };
}