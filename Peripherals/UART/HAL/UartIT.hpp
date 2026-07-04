#pragma once

#include "IUart.hpp"
#include "Utils/RingBuffer.hpp"

extern "C"
{
#include "stm32l4xx_hal.h"
}

template<std::size_t BufferSize>
using UartRingBuffer = RingBuffer<uint8_t, BufferSize>;

namespace Peripherals
{
    namespace HAL
    {
        template<std::size_t BufferSize = 64>
        class UartIT : public IUart
        {
        private:
            UART_HandleTypeDef& huart;
            std::size_t overflowCount{ 0 };
            UartRingBuffer<64> rxBuffer{};
            UartRingBuffer<64> txBuffer{};
            uint8_t rxByte{};
            uint8_t txByte{};
            bool txBusy{ false };

        public:
            UartIT() = delete;
            UartIT(const UartIT&) = delete;
            UartIT(UartIT&&) = delete;
            UartIT& operator=(const UartIT&) = delete;
            UartIT& operator=(UartIT&&) = delete;
            ~UartIT() = default;
            explicit UartIT(UART_HandleTypeDef& huart_)
                : huart(huart_)
            {};

            void StartReceiveIT()
            {
                HAL_UART_Receive_IT(&huart, &rxByte, 1);
            }

            void StartTransmitIT()
            {
                if (txBusy || txBuffer.IsEmpty())
                    return;

                txBusy = true;
                
                if (const auto byte = txBuffer.Pop())
                {
                    txByte = byte.value();
                    HAL_UART_Transmit_IT(&huart, &txByte, 1);
                }
                else
                {
                    txBusy = false;
                }
            }

            void RxCpltCallback()
            {
                if (!rxBuffer.Push(rxByte))
                    ++overflowCount;
    
                StartReceiveIT();
            }

            //TODO callback shoul set a state machine, and start transmit should be done by TransmitIT in the machine
            void TxCpltCallback()
            {
                txBusy = false;
                StartTransmitIT();
            }

            [[nodiscard]] std::optional<uint8_t> Read() override
            {
                return rxBuffer.Pop();
            }

            void Transmit(const uint8_t* data, size_t size) override
            {
                for (size_t i = 0; i < size; ++i)
                {
                    if (!txBuffer.Push(data[i]))
                    {
                        ++overflowCount; // or handle drop differently
                        break;
                    }
                }

                if (!txBusy)
                    StartTransmitIT();
            }
        };
    }
}