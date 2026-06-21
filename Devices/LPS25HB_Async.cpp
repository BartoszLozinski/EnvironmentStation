#include "LPS25HB_Async.hpp"

namespace Device
{
    LPS25HB_Async::LPS25HB_Async(Peripherals::I2CBase_IT& i2c_)
        : i2c(i2c_) {}

    // TODO: make common base class for LPS25HB,
    float LPS25HB_Async::RecalculateRawTemperature(const int16_t rawTemp) const
    {
        // Formula provided by the datasheet
        return rawTemp / 480.0f + 42.5f; // *C
    }

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

    /*void LPS25HB_Async::ReadRawTemperature()
    {
        static constexpr uint8_t AUTO_INCREMENT = 0x80; //TEMP_OUT_L and TEMP_OUT_H are in sequence, so we can read them in one go by setting the auto-increment bit
        if (state == State::Idle)
        {
            state = i2c.Read(Registers::ADDR
                            , Registers::TEMP_OUT_L | AUTO_INCREMENT
                            , std::span<uint8_t>(reinterpret_cast<uint8_t*>(&rawTempBuffer)
                                                , sizeof(rawTempBuffer))) == Peripherals::I2CResult::Success ? State::TransferScheduled : State::Error;
        }//TODO create own RxReadCompleted
        else if (i2c.GetState() == Peripherals::I2CState::Done)
        {
            state = State::Done;
            readingRawTemperatureCompleted = true;
        }
    }*/

    // TO DO: Check if code can be cleaner, remove unnecessary overloads

    std::optional<float> LPS25HB_Async::ReadTemperature()
    {
        //ReadRawTemperature();

        static constexpr uint8_t AUTO_INCREMENT = 0x80; //TEMP_OUT_L and TEMP_OUT_H are in sequence, so we can read them in one go by setting the auto-increment bit
        if (state == State::Idle)
        {
            state = i2c.Read(Registers::ADDR
                            , Registers::TEMP_OUT_L | AUTO_INCREMENT
                            , std::span<uint8_t>(rawTempBuffer.data()
                                                , rawTempBuffer.size())) == Peripherals::I2CResult::Success ? State::TransferScheduled : State::Error;
        }//TODO create own RxReadCompleted
        else if (i2c.GetState() == Peripherals::I2CState::Done)
        {
            //is state::Done necessary?
            i2c.NotifyDataIsRead();
            state = State::Done;
            const int16_t tempBuffer = (static_cast<int16_t>(rawTempBuffer[1]) << 8) | rawTempBuffer[0];
            state = State::Idle;
            return RecalculateRawTemperature(tempBuffer);
        }

        return std::nullopt;
    }

}
