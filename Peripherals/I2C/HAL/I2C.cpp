#include "I2C.hpp"

namespace Peripherals
{
    namespace HAL
    {
        I2C::I2C(I2C_HandleTypeDef& i2cHandle_, const uint32_t timeOut_)
            : i2cHandle(i2cHandle_)
            , timeOut(timeOut_)
        {}

        void I2C::SetTimeout(const uint32_t timeOut_)
        {
            timeOut = timeOut_;
        }

        void I2C::Write(const uint16_t deviceAddress, const uint16_t memoryAddress, const uint8_t value)
        {
            uint8_t data = value;
            HAL_I2C_Mem_Write(&i2cHandle, deviceAddress, memoryAddress, 1, &data, sizeof(data), timeOut);
        }

        bool I2C::Read(const uint16_t deviceAddress, const uint16_t memoryAddress, std::span<uint8_t> buffer)
        {
            if (buffer.empty())
                return false;

            static constexpr uint16_t memoryAddressSize = 1;
            const auto status = HAL_I2C_Mem_Read( &i2cHandle
                                                , deviceAddress
                                                , memoryAddress
                                                , memoryAddressSize
                                                , buffer.data()
                                                , buffer.size()
                                                , timeOut);
            return status == HAL_OK;
        }
    }
}