#include <stdio.h>
#include <cstring>
#include <string_view>
#include <array>

#include "stm32CubeMXGenerated.h"

#include "../../Peripherals/Timer/HAL/SoftwareTimer.hpp"
#include "../../Peripherals/I2C/HAL/I2C.hpp"
#include "../../Peripherals/I2C/HAL/I2C_IT.hpp"
#include "../../Peripherals/UART/HAL/UartIT.hpp"
#include "../../Peripherals/UART/LineParser.hpp"

#include "../../Peripherals/GPIO/RegisterLevel/GpioOutput.hpp"
#include "../../Peripherals/UART/RegisterLevel/Uart.hpp"



#include "../../Devices/LPS25HB.hpp"
#include "../../Devices/LPS25HB_Async.hpp"


Peripherals::HAL::UartIT btHC06Uart{ huart1 }; //PA9 (TX), PA10 (RX)
Peripherals::HAL::UartIT uart2{ huart2 };
Peripherals::HAL::I2C_IT i2c1IT{ hi2c1 };
Device::LPS25HB_Async lps25hbAsync{ i2c1IT };
Peripherals::RegisterLevel::GpioOutput<5> ld2(GPIOA);

int main()
{
    /* MCU Configuration--------------------------------------------------------*/
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();
    SystemClock_Config();
    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_USART2_UART_Init();
    MX_USART1_UART_Init();
    MX_I2C1_Init();
    
    HAL::SoftwareTimer ld2Timer{ 500 };
    HAL::SoftwareTimer uartResetTimer{ 2000 };
    HAL::SoftwareTimer uart2PollTimer{ 1 };
    HAL::SoftwareTimer btUartResetTimer{ 2000 };
    HAL::SoftwareTimer btUartPollTimer{ 1 };
    HAL::SoftwareTimer temperatureReadingTimer{ 500 };
    HAL::SoftwareTimer pressureReadingTimer{ 500 };
    UcCommunication::LineParser lineParser{ uart2 };
    UcCommunication::LineParser btLineParser{ btHC06Uart };
    Peripherals::HAL::I2C i2c1{ hi2c1 };
    i2c1.SetTimeout(100);
    Device::LPS25HB lps25hb{ i2c1 };

    ld2.Init();

    // LPS25HB test

    static constexpr char testMsg[] = "LPS25HB test:\r\n";
    uart2.Transmit(reinterpret_cast<const uint8_t*>(testMsg), strlen(testMsg));
    
    const auto whoAmI = lps25hb.ReadWhoAmI();

    if (whoAmI.has_value() && whoAmI.value() == 0xBD)
    {
        static constexpr char successMsg[] = "LPS25HB Found!\r\n";
        uart2.Transmit(reinterpret_cast<const uint8_t*>(successMsg), strlen(successMsg));
        //TO DO - change into state machine, instead of waking up  in blocking loop
        while(!lps25hbAsync.IsAwake())
            lps25hbAsync.WakeUp();

        lps25hb.SetMeasurementFrequency(Device::LPS25HB::MeasurementFrequency::Hz25);
        HAL_Delay(100); //TODO - get rid to blocking delay
    }
    else
    {
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "Error: (x%02X)\r\n", whoAmI.value());
        uart2.Transmit(reinterpret_cast<const uint8_t*>(buffer), strlen(buffer));
        return 1;
    }

    while (true)
    {
        // UART 1 Test - Bluetooth HC-06
        // connection at linux
        // sudo rfcomm connect 0 <Address>
        // on second terminal window: screen /dev/rfcomm0 9600

        uart2.ProcessTx();
        btHC06Uart.ProcessTx();
        uart2.ProcessRx();
        btHC06Uart.ProcessRx();
        
        if (const auto lineOpt = btLineParser.ReadLine())
        {
            const auto line = *lineOpt;
            const char* prefix = "RX: ";
            btHC06Uart.Transmit(reinterpret_cast<const uint8_t*>(prefix), strlen(prefix));
            btHC06Uart.Transmit(reinterpret_cast<const uint8_t*>(line.data()), line.size());
            btHC06Uart.Transmit(reinterpret_cast<const uint8_t*>("\r\n"), 2);
        }
        
        if (btUartResetTimer.IsExpired())
        {  
            btUartResetTimer.Reset();
            std::string_view resetMsg = "BT UART Reset\r\n";
            btHC06Uart.Transmit(reinterpret_cast<const uint8_t*>(resetMsg.data()), resetMsg.size());
        }
        
        if (const auto lineOpt = lineParser.ReadLine())
        {
            const auto line = *lineOpt;
            const char* prefix = "RX Uart2IT: ";
            uart2.Transmit(reinterpret_cast<const uint8_t*>(prefix), strlen(prefix));
            uart2.Transmit(reinterpret_cast<const uint8_t*>(line.data()), line.size());
            uart2.Transmit(reinterpret_cast<const uint8_t*>("\r\n"), 2);
        }

        // End of UART Test

        if (const auto result = lps25hbAsync.ReadTemperature(); result && temperatureReadingTimer.IsExpired())
        {
            temperatureReadingTimer.Reset();
            char messageBuffer[32];
            snprintf(messageBuffer, sizeof(messageBuffer), "Temp LPS IT: %.2f C\r\n", result.value());
            uart2.Transmit(reinterpret_cast<const uint8_t*>(messageBuffer), strlen(messageBuffer));
        }
        
        if (const auto result = lps25hbAsync.ReadPressure(); result && pressureReadingTimer.IsExpired())
        {
            pressureReadingTimer.Reset();
            char messageBuffer[32];
            snprintf(messageBuffer, sizeof(messageBuffer), "Pressure LPS IT: %lu hPa\r\n", result.value());
            uart2.Transmit(reinterpret_cast<const uint8_t*>(messageBuffer), strlen(messageBuffer));
        }

        if (ld2Timer.IsExpired())
        {
            ld2Timer.Reset();
            ld2.Toggle();
        }
    }
}



void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}

extern "C" void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        btHC06Uart.RxCpltCallback();
    }

    if (huart->Instance == USART2)
    {
        uart2.RxCpltCallback();
    }
}

extern "C" void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        btHC06Uart.TxCpltCallback();
    }

    if (huart->Instance == USART2)
    {
        uart2.TxCpltCallback();
    }
}

extern "C" void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == I2C1)
    {
        lps25hbAsync.OnRxComplete();
    }
}

extern "C" void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == I2C1)
    {
        lps25hbAsync.OnTxComplete();
    }
}

extern "C" void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == I2C1)
    {
        lps25hbAsync.OnTxComplete();
    }
}

extern "C" void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == I2C1)
    {
        // Optionally inspect hi2c->ErrorCode here for future debug.
    }
}
