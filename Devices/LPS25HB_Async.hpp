#pragma once

#include "../Peripherals/I2C/I2CBase.hpp"
#include <optional>

namespace Device
{
    class LPS25HB_Async
    {
    private:
        struct Registers
        {
            static constexpr uint16_t ADDR = 0xBA;
            static constexpr uint16_t WHO_AM_I = 0x0F;
            static constexpr uint16_t CTRL_REG1 = 0x20;
            static constexpr uint16_t CTRL_REG2 = 0x21;
            static constexpr uint16_t CTRL_REG3 = 0x22;
            static constexpr uint16_t CTRL_REG4 = 0x23;
            static constexpr uint16_t PRESS_OUT_XL = 0x28;
            static constexpr uint16_t PRESS_OUT_L = 0x29;
            static constexpr uint16_t PRESS_OUT_H = 0x2A;
            static constexpr uint16_t TEMP_OUT_L = 0x2B;
            static constexpr uint16_t TEMP_OUT_H = 0x2C;
            static constexpr uint16_t CTRL_REG1_PD = 0x80;
            static constexpr uint16_t CTRL_REG1_ODR2 = 0x40;
            static constexpr uint16_t CTRL_REG1_ODR1 = 0x20;
            static constexpr uint16_t CTRL_REG1_ODR0 = 0x10;
        };

        enum class State
        {
            Idle,
            TransferScheduled,
            Done,
            Error,
        };
        
        Peripherals::I2CBase_IT& i2c;
        [[nodiscard]] float RecalculateRawTemperature(const int16_t rawTemp) const;
        [[nodiscard]] int32_t RecalculateRawPressure(const int32_t rawPressure) const;
        void WriteRegister(const uint8_t reg, uint8_t value);
        [[nodiscard]] std::optional<uint8_t> ReadRegister(uint8_t reg);
        State state = State::Idle;
        uint8_t readRegisterBuffer = 0;
        uint16_t rawTempBuffer = 0;
        uint32_t rawPressureBuffer = 0;

    public:
        enum class MeasurementFrequency
        {
            OneShot,
            Hz1,
            Hz7,
            Hz12_5,
            Hz25
        };

        LPS25HB_Async(Peripherals::I2CBase_IT& i2c_);
        [[nodiscard]] std::optional<uint8_t> ReadWhoAmI();
        [[nodiscard]] std::optional<float> ReadTemperature() const;
        [[nodiscard]] std::optional<int32_t> ReadPressure() const;
        void SetMeasurementFrequency(const MeasurementFrequency freq);
        void WakeUp();
    };
}
