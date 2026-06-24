#include "LPS25HB_Async.hpp"

namespace Device
{
    LPS25HB_Async::LPS25HB_Async(Peripherals::I2CBase_IT& i2c_)
        : i2c(i2c_) {}

    // TODO: make common base class for LPS25HB,

    std::optional<uint8_t> LPS25HB_Async::ReadRegister(const uint8_t reg)
    {
        if (state == State::Idle)
        {
            state = i2c.Read(Registers::ADDR
                            , reg
                            , std::span<uint8_t>(&readRegisterBuffer, sizeof(readRegisterBuffer))) == Peripherals::I2CResult::Success ? State::TransferScheduled : State::Error;
        }
        else if (i2c.GetState() == Peripherals::I2CState::Done)
        {
            state = State::Done;
            return readRegisterBuffer;
        }

        return std::nullopt;
    }

    std::optional<uint8_t> LPS25HB_Async::ReadWhoAmI()
    {
        const auto result = ReadRegister(Registers::WHO_AM_I);
        if (result && state == State::Done)
            state = State::Idle;

        return result;
    }

    // TO DO: Check if code can be cleaner, remove unnecessary overloads

    std::optional<float> LPS25HB_Async::ReadTemperature()
    {
        if (state == State::Idle)
        {
            state = i2c.Read(Registers::ADDR
                            , Registers::TEMP_OUT_L | AUTO_INCREMENT
                            , std::span<uint8_t>(rawTempBuffer.data()
                                                , rawTempBuffer.size())) == Peripherals::I2CResult::Success ? State::RxTempReading : State::Error;
        }
        else if (state == State::RxTempReading && i2c.GetState() == Peripherals::I2CState::Done)
        {
            //is state::Done necessary?
            i2c.NotifyDataIsRead();
            //state = State::Done;
            const int16_t tempBuffer = (static_cast<int16_t>(rawTempBuffer[1]) << 8) | rawTempBuffer[0];
            state = State::Idle;
            return RecalculateRawTemperature(tempBuffer);
        }

        return std::nullopt;
    }
    
    std::optional<int32_t> LPS25HB_Async::ReadPressure()
    {
        if (state == State::Idle)
        {
            state = i2c.Read(Registers::ADDR
                            , Registers::PRESS_OUT_XL | AUTO_INCREMENT
                            , std::span<uint8_t>(rawPressureBuffer.data()
                                                , rawPressureBuffer.size())) == Peripherals::I2CResult::Success ? State::RxPressureReading : State::Error;
        }
        else if (state == State::RxPressureReading && i2c.GetState() == Peripherals::I2CState::Done)
        {
            //is state::Done necessary?
            i2c.NotifyDataIsRead();
            //state = State::Done;
            const uint32_t pressureBuffer = (static_cast<int16_t>(rawPressureBuffer[2]) << 16) | rawPressureBuffer[1] << 8 | rawPressureBuffer[0];
            state = State::Idle;
            return RecalculateRawPressure(pressureBuffer);
        }

        return std::nullopt;
    }
}
