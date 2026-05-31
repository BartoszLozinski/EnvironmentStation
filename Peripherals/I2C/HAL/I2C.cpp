#include "I2C.hpp"

namespace Peripherals
{
    namespace HAL
    {
        void I2C::Write(const uint16_t deviceAddress, const uint16_t memoryAddress, const uint8_t value)
        {
            uint8_t data = value;
            HAL_I2C_Mem_Write(&i2cHandle, deviceAddress, memoryAddress, 1, &data, sizeof(data), HAL_MAX_DELAY);
        }

        int32_t I2C::Read(const uint16_t deviceAddress, const uint16_t memoryAddress)
        {
            int32_t value = 0;
            HAL_I2C_Mem_Read(&i2cHandle, deviceAddress, memoryAddress, 1, reinterpret_cast<uint8_t*>(&value), sizeof(value), HAL_MAX_DELAY);
            return value;
        }
    }
}