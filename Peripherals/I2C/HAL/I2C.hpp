#pragma once
#include "I2CBase.hpp"

extern "C"
{
    #include "stm32l4xx_hal.h"
}

namespace Peripherals
{
    namespace HAL
    {
        class I2C : public I2CBase
        {
        private:
            I2C_HandleTypeDef& i2cHandle;

        public:
            void Write(const uint16_t deviceAddress, const uint16_t memoryAddress, const uint8_t value, const uint32_t timeOut = HAL_MAX_DELAY) override;
            [[nodiscard]] bool Read(const uint16_t deviceAddress, const uint16_t memoryAddress, std::span<uint8_t> buffer, const uint32_t timeout = HAL_MAX_DELAY) override;
            void SetTimeout(const uint32_t timeOut_);

            I2C() = delete;
            I2C(const I2C&) = delete;
            I2C(I2C&&) = delete;
            I2C& operator=(const I2C&) = delete;
            I2C& operator=(I2C&&) = delete;
            ~I2C() = default;
            explicit I2C(I2C_HandleTypeDef& i2cHandle_);
        };
    }
}