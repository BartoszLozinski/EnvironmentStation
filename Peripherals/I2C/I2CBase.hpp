#pragma once
#include <cstdint>
#include <span>
#include <optional>

namespace Peripherals
{
    enum class I2CResult
    {
        Success,
        Busy,
        Error,        
    };

    enum class I2CState
    {
        Idle,
        RxBusy,
        TxBusy,
        Done,
        Error
    };

    class I2CBase
    {
    public:
        virtual ~I2CBase() = default;
        virtual I2CResult Write(const uint16_t deviceAddress, const uint16_t memoryAddress, std::span<uint8_t> buffer) = 0;
        [[nodiscard]] virtual I2CResult Read(const uint16_t deviceAddress, const uint16_t memoryAddress, std::span<uint8_t> buffer) = 0;
    };

    class I2CBase_IT : public I2CBase
    {
    public:
        virtual ~I2CBase_IT() = default;
        virtual void OnRxComplete() = 0;
        virtual void NotifyDataIsRead() = 0;
        virtual Peripherals::I2CState GetState() const = 0;
    };
}