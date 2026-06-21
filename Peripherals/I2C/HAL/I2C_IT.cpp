#include "I2C_IT.hpp"

namespace Peripherals
{
    namespace HAL
    {
        template<std::size_t BufferSize>
        I2C_IT<BufferSize>::I2C_IT(I2C_HandleTypeDef& i2cHandle_) : i2cHandle(i2cHandle_) {}

        template<std::size_t BufferSize>
        I2CResult I2C_IT<BufferSize>::Write(const uint16_t deviceAddress, const uint16_t memoryAddress, std::span<uint8_t> buffer)
        {
            if (state != I2CState::Idle)
                return I2CResult::Busy;

            const bool success = HAL_I2C_Mem_Write_IT( &i2cHandle
                                                    , deviceAddress
                                                    , memoryAddress
                                                    , I2C_MEMADD_SIZE_8BIT
                                                    , buffer.data()
                                                    , buffer.size()) == HAL_OK;

            state = success ? I2CState::TxBusy : I2CState::Error;
            return success ? I2CResult::Success : I2CResult::Error;
        }

        template<std::size_t BufferSize>
        I2CResult I2C_IT<BufferSize>::Read(const uint16_t deviceAddress, const uint16_t memoryAddress, std::span<uint8_t> buffer)
        {
            if (state != I2CState::Idle)
                return I2CResult::Busy;

            const bool success = HAL_I2C_Mem_Read_IT(&i2cHandle
                                                    , deviceAddress
                                                    , memoryAddress
                                                    , I2C_MEMADD_SIZE_8BIT
                                                    , buffer.data()
                                                    , buffer.size()) == HAL_OK;

            state = success ? I2CState::RxBusy : I2CState::Error;
            return success ? I2CResult::Success : I2CResult::Error;
        }

        template<std::size_t BufferSize>
        void I2C_IT<BufferSize>::OnRxComplete()
        {
            if (state == I2CState::RxBusy)
                state = I2CState::Done;
        }

        template<std::size_t BufferSize>
        std::optional<std::span<uint8_t>> I2C_IT<BufferSize>::GetResult()
        {
            if (state == I2CState::Done)
            {
                state = I2CState::Idle;
                return std::span<uint8_t>(rXBuffer.data(), rXBuffer.size());
            }

            if (state == I2CState::Error)
            {
                state = I2CState::Idle;
                return std::nullopt;
            }

            return std::nullopt;
        }

        template<std::size_t BufferSize>
        void I2C_IT<BufferSize>::NotifyDataIsRead()
        {
            state = I2CState::Idle;
        }

        template class I2C_IT<4>;
    }
}