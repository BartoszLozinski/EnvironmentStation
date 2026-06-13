#pragma once
#include <cstdint>
#include <span>

// TODO test dynamic polymorphism vs CRTP later
// Start with dynamic polymorphism first

namespace Peripherals
{
    class I2CBase
    {
    public:
        virtual ~I2CBase() = default;
        virtual void Write(const uint16_t deviceAddress, const uint16_t memoryAddress, const uint8_t value) = 0;
        [[nodiscard]] virtual bool Read(const uint16_t deviceAddress, const uint16_t memoryAddress, std::span<uint8_t> buffer) = 0;
    };
}