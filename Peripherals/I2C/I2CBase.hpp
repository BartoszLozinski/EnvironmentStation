#pragma once
#include <cstdint>

// TODO test dynamic polymorphism vs CRTP later
// Start with dynamic polymorphism first

namespace Peripherals
{
    class I2CBase
    {
    public:
        virtual ~I2CBase() = default;
        virtual void Write(const uint16_t deviceAddress, const uint16_t memoryAddress, const uint8_t value) = 0;
        [[nodiscard]] virtual int32_t Read(const uint16_t deviceAddress, const uint16_t memoryAddress) = 0;
    };
}