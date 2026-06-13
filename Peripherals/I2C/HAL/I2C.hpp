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
            uint32_t timeOut{ HAL_MAX_DELAY };

        public:
            void Write(const uint16_t deviceAddress, const uint16_t memoryAddress, const uint8_t value) override;
            [[nodiscard]] bool Read(const uint16_t deviceAddress, const uint16_t memoryAddress, std::span<uint8_t> buffer) override;

            I2C() = delete;
            I2C(const I2C&) = delete;
            I2C(I2C&&) = delete;
            I2C& operator=(const I2C&) = delete;
            I2C& operator=(I2C&&) = delete;
            ~I2C() = default;
            explicit I2C(I2C_HandleTypeDef& i2cHandle_);
            void SetTimeout(const uint32_t timeOut_);
        };
    }
}