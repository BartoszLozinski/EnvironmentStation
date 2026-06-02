#include "LPS25HB.hpp"

namespace Device
{
    LPS25HB::LPS25HB(Peripherals::I2CBase& i2c_) : i2c(i2c_) {}

    float LPS25HB::RecalculateRawTemperature(const int16_t rawTemp) const
    {
        // Formula provided by the datasheet
        return rawTemp / 480.0f + 42.5f; // *C
    }
    
    // TODO WRITE UNIT TESTS!!!

    void LPS25HB::WriteRegister(const uint8_t reg, const uint8_t value)
    {
        i2c.Write(Registers::ADDR, reg, value);
    }

    std::optional<uint8_t> LPS25HB::ReadRegister(const uint8_t reg) const
    {
        uint8_t result = 0;
        if (i2c.Read(Registers::ADDR, reg, std::span<uint8_t>(&result, sizeof(result))))
            return result;
        else
            return std::nullopt;

    };
}