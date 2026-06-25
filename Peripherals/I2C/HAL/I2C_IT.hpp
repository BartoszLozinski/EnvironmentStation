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
    // TODO Make it clear
    // think about architecture refactor?
    // probably doesnt need buffer, just force user to pass buffer
    namespace HAL
    {
        template<std::size_t BufferSize>
        class I2C_IT : public I2CBase_IT
        {
        private:
            I2C_HandleTypeDef& i2cHandle;
            std::array<uint8_t, BufferSize> rXBuffer{};
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
            void OnRxComplete() override;
            void OnTxComplete() override;
            void NotifyDataIsRead() override;
            Peripherals::I2CState GetState() const override { return state; }

        };
    }
}