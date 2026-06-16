#pragma once

#include "I2CBase_IT.hpp"
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

    // TODO Make it clear
    // think about architecture refactor?
    namespace HAL
    {
        class I2C_IT : public I2CBase_IT
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

            [[nodiscard]] I2CResult Write(const uint16_t deviceAddress, const uint16_t memoryAddress, std::span<uint8_t> buffer) override;
            [[nodiscard]] I2CResult Read(const uint16_t deviceAddress, const uint16_t memoryAddress, std::span<uint8_t> buffer) override;

            // convenience overload that uses the internal buffer
            [[nodiscard]] I2CResult Read(const uint16_t deviceAddress, const uint16_t memoryAddress)
            {
                return Read(deviceAddress, memoryAddress, std::span<uint8_t>(rXBuffer.data(), rXBuffer.size()));
            }
            void OnRxComplete();
            void Acknowledge();
            std::optional<std::array<uint8_t, 4>> GetResult();
            Peripherals::I2CState GetState() const { return state; }

        };
    }
}