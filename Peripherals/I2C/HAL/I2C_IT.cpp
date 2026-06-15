#include "I2C_IT.hpp"

namespace Peripherals
{
    namespace HAL
    {
        I2C_IT::I2C_IT(I2C_HandleTypeDef& i2cHandle_) : i2cHandle(i2cHandle_) {}

        void I2C_IT::Write(const uint16_t deviceAddress, const uint16_t memoryAddress, const uint8_t value)
        {
            // copy value into internal TX buffer so its address remains valid
            tXBuffer[0] = value;
            HAL_I2C_Mem_Write_IT(&i2cHandle, deviceAddress, memoryAddress, I2C_MEMADD_SIZE_8BIT, tXBuffer.data(), 1);
        }

        bool I2C_IT::Read(const uint16_t deviceAddress, const uint16_t memoryAddress, std::span<uint8_t> buffer /* = std::span<uint8_t>(rXBuffer, sizeof(rXBuffer)) */)
        {
            if (state != I2CState::Idle)
                return false;

            state = I2CState::RxBusy;
            return HAL_I2C_Mem_Read_IT(&i2cHandle
                                      , deviceAddress
                                      , memoryAddress
                                      , I2C_MEMADD_SIZE_8BIT
                                      , buffer.data()
                                      , buffer.size()) == HAL_OK;
        }

        void I2C_IT::OnRxComplete()
        {
            if (state == I2CState::RxBusy)
            {
                state = I2CState::Done;
            }
        }

        std::optional<std::array<uint8_t, 4>> I2C_IT::GetBuffer()
        {
            if (state == I2CState::Done)
            {
                state = I2CState::Idle;
                return rXBuffer;
            }

            return std::nullopt;
        }
    }
}