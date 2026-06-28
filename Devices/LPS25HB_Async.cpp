#include "LPS25HB_Async.hpp"

namespace Device
{
    LPS25HB_Async::LPS25HB_Async(Peripherals::I2CBase_IT& i2c_)
        : i2c(i2c_) {}

    void LPS25HB_Async::OnRxComplete()
    {
        i2c.OnRxComplete();

        using enum State;
        switch(state)
        {
        case RxPressureReading:
            state = PressureReadyToRead;
            break;
        case RxTempReading:
            state = TempReadyToRead;
            break;
        default:
            state = RegisterReadyToRead;
        }
    }

    void LPS25HB_Async::OnTxComplete()
    {
        i2c.OnTxComplete();
        if (state == State::WakeUpScheduled)
            state = State::WakeUpCompleted;
    }

    std::optional<uint8_t> LPS25HB_Async::ReadRegister(const uint8_t reg)
    {
        if (state == State::Idle)
        {
            state = i2c.Read(Registers::ADDR
                            , reg
                            , std::span<uint8_t>(&readRegisterBuffer, sizeof(readRegisterBuffer))) == Peripherals::I2CResult::Success ? State::TransferScheduled : State::Error;
        }
        else if (state == State::RegisterReadyToRead)
        {
            i2c.NotifyDataIsRead();
            return readRegisterBuffer;
        }

        return std::nullopt;
    }

    std::optional<uint8_t> LPS25HB_Async::ReadWhoAmI()
    {
        const auto result = ReadRegister(Registers::WHO_AM_I);
        if (result && state == State::RegisterReadyToRead)
        {
            state = State::Idle;
        }
        
        return result;
    }

    std::optional<float> LPS25HB_Async::ReadTemperature()
    {
        if (state == State::Idle)
        {
            state = i2c.Read(Registers::ADDR
                            , Registers::TEMP_OUT_L | AUTO_INCREMENT
                            , std::span<uint8_t>(rawTempBuffer.data()
                                                , rawTempBuffer.size())) == Peripherals::I2CResult::Success ? State::RxTempReading : State::Error;
        }
        else if (state == State::TempReadyToRead)
        {
            i2c.NotifyDataIsRead();
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
        else if (state == State::PressureReadyToRead)
        {
            i2c.NotifyDataIsRead();
            const uint32_t pressureBuffer = (static_cast<int16_t>(rawPressureBuffer[2]) << 16) | rawPressureBuffer[1] << 8 | rawPressureBuffer[0];
            state = State::Idle;
            return RecalculateRawPressure(pressureBuffer);
        }

        return std::nullopt;
    }

    void LPS25HB_Async::WakeUp()
    {
        static uint8_t currentCtrlReg1 = 0;

        if (state == State::Idle)
        {
            currentCtrlReg1 = 0;
            state = i2c.Read(Registers::ADDR
                            , Registers::CTRL_REG1
                            , std::span<uint8_t>(&readRegisterBuffer, sizeof(readRegisterBuffer))) == Peripherals::I2CResult::Success ? State::TransferScheduled : State::Error;
        }
        else if (state == State::RegisterReadyToRead)
        {
            currentCtrlReg1 = readRegisterBuffer;
            i2c.NotifyDataIsRead();
            state = State::WakeUpScheduled;
            currentCtrlReg1 = static_cast<uint8_t>(currentCtrlReg1 | Registers::CTRL_REG1_PD);
            i2c.Write(Registers::ADDR, Registers::CTRL_REG1, std::span<uint8_t>(&currentCtrlReg1, sizeof(currentCtrlReg1)));
            
        }
        else if (state == State::WakeUpCompleted)
        {
            state = State::Idle;
            i2c.NotifyDataIsRead();
            isAwake = true;
        }
    }
}
