#pragma once

#include "../Peripherals/I2C/I2CBase.hpp"
#include "LPS25HBBase.hpp"

namespace Device
{
    class LPS25HB_Async : public LPS25HBBase
    {
    private:
        enum class State
        {
            Idle,
            TransferScheduled,
            WakeUpScheduled,
            RxTempReading,
            RxPressureReading,
            TempReadyToRead,
            PressureReadyToRead,
            RegisterReadyToRead,
            WakeUpCompleted,
            Error,
        };
        
        Peripherals::I2CBase_IT& i2c;
        void WriteRegister(const uint8_t reg, uint8_t value);
        [[nodiscard]] std::optional<uint8_t> ReadRegister(uint8_t reg);
        State state = State::Idle;
        uint8_t readRegisterBuffer{}; 
        std::array<uint8_t, 2> rawTempBuffer{};
        std::array<uint8_t, 3> rawPressureBuffer{};
        bool isAwake = false;

    public:

        LPS25HB_Async(Peripherals::I2CBase_IT& i2c_);
        [[nodiscard]] std::optional<uint8_t> ReadWhoAmI() override;
        [[nodiscard]] std::optional<float> ReadTemperature() override;
        [[nodiscard]] std::optional<int32_t> ReadPressure() override;
        void SetMeasurementFrequency(const MeasurementFrequency freq);
        [[nodiscard]] bool IsAwake() const { return isAwake; }
        void WakeUp() override;

        void OnRxComplete();
        void OnTxComplete();
    };
}
