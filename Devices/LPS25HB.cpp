#include "LPS25HB.hpp"

namespace Device
{
    LPS25HB::LPS25HB(Peripherals::I2CBase& i2c_) : i2c(i2c_) {}

    float LPS25HB::RecalculateRawTemperature(const int16_t rawTemp) const
    {
        // Formula provided by the datasheet
        return rawTemp / 480.0f + 42.5f; // *C
    }
    
    int32_t LPS25HB::RecalculateRawPressure(const int32_t rawPressure) const
    {
        // Formula provided by the datasheet
        return rawPressure / 4096; // hPa
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

    std::optional<uint8_t> LPS25HB::ReadWhoAmI() const
    {
        return ReadRegister(Registers::WHO_AM_I);
    }

    std::optional<float> LPS25HB::ReadTemperature() const
    {

        //TODO: Get rid of magic numbers
        int16_t rawTemp = 0;
        if (i2c.Read(Registers::ADDR, Registers::TEMP_OUT_L | 0x80, std::span<uint8_t>(reinterpret_cast<uint8_t*>(&rawTemp), sizeof(rawTemp))))
            return RecalculateRawTemperature(rawTemp);
        else
            return std::nullopt;
    };

    std::optional<int32_t> LPS25HB::ReadPressure() const
    {
        //TODO Get rid of magic numbers
        int32_t rawPressure = 0;
        if (i2c.Read(Registers::ADDR, Registers::PRESS_OUT_XL | 0x80, std::span<uint8_t>(reinterpret_cast<uint8_t*>(&rawPressure), sizeof(rawPressure) - 1)))
            return RecalculateRawPressure(rawPressure);
        else
            return std::nullopt;
    }

    void LPS25HB::SetMeasurementFrequency(const MeasurementFrequency freq)
    {
        uint8_t regValue = 0;
        switch (freq)
        {
            case MeasurementFrequency::OneShot:
                regValue = 0x00;
                break;
            case MeasurementFrequency::Hz1:
                regValue = Registers::CTRL_REG1_ODR0;
                break;
            case MeasurementFrequency::Hz7:
                regValue = Registers::CTRL_REG1_ODR1;
                break;
            case MeasurementFrequency::Hz12_5:
                regValue = Registers::CTRL_REG1_ODR1 | Registers::CTRL_REG1_ODR0;
                break;
            case MeasurementFrequency::Hz25:
                regValue = Registers::CTRL_REG1_ODR2;
                break;
        }

        static constexpr uint8_t ODR_MASK = Registers::CTRL_REG1_ODR2 | Registers::CTRL_REG1_ODR1 | Registers::CTRL_REG1_ODR0;
        uint8_t current = 0;
        if (auto curOpt = ReadRegister(Registers::CTRL_REG1); curOpt.has_value())
            current = curOpt.value();

        uint8_t newVal = (current & ~ODR_MASK) | (regValue & ODR_MASK);
        WriteRegister(Registers::CTRL_REG1, newVal);
    }

    void LPS25HB::WakeUp()
    {
        uint8_t current = 0;
        if (auto curOpt = ReadRegister(Registers::CTRL_REG1); curOpt.has_value())
            current = curOpt.value();

        WriteRegister(Registers::CTRL_REG1, static_cast<uint8_t>(current | Registers::CTRL_REG1_PD));
    }
}