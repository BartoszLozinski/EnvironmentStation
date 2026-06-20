#include "LPS25HB_Async.hpp"

namespace Device
{
    LPS25HB_Async::LPS25HB_Async(Peripherals::I2CBase_IT& i2c_)
        : i2c(i2c_)
    {
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

}
