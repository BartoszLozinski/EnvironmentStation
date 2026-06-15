#pragma once

#include "I2CBase.hpp"
#include <array>
#include <optional>
#include <span>

extern "C"
{
    #include "stm32l4xx_hal.h"
}

namespace Peripherals
{
    enum class I2CState
    {
        Idle,
        RxBusy,
        TxBusy,
        Done,
        Error
    };

    namespace HAL
    {
        class I2C_IT : public I2CBase
        {
        private:
            I2C_HandleTypeDef& i2cHandle;
            std::array<uint8_t, 4> rXBuffer{};
            // small transmit buffer to hold data until IRQ-driven transfer completes
            std::array<uint8_t, 1> tXBuffer{};
            volatile I2CState state{  I2CState::Idle };

        public:
            I2C_IT() = delete;
            I2C_IT(const I2C_IT&) = delete;
            I2C_IT(I2C_IT&&) = delete;
            I2C_IT& operator=(const I2C_IT&) = delete;
            I2C_IT& operator=(I2C_IT&&) = delete;
            ~I2C_IT() = default;
            explicit I2C_IT(I2C_HandleTypeDef& i2cHandle_);

            void Write(const uint16_t deviceAddress, const uint16_t memoryAddress, const uint8_t value) override;
            [[nodiscard]] bool Read(const uint16_t deviceAddress, const uint16_t memoryAddress, std::span<uint8_t> buffer) override;

            // convenience overload that uses the internal buffer
            [[nodiscard]] bool Read(const uint16_t deviceAddress, const uint16_t memoryAddress)
            {
                return Read(deviceAddress, memoryAddress, std::span<uint8_t>(const_cast<uint8_t*>(rXBuffer.data()), rXBuffer.size()));
            }
            void OnRxComplete();

            std::optional<std::array<uint8_t, 4>> GetBuffer();

            Peripherals::I2CState GetState() const { return state; }

        };
    }
}