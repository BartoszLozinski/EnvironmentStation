#include <stdio.h>
#include <cstring>
#include <string_view>
#include <array>

#include "stm32CubeMXGenerated.h"

#include "../../Peripherals/Timer/RegisterLevel/SoftwareTimer.hpp"
#include "../../Peripherals/I2C/HAL/I2C_IT.hpp"
#include "../../Peripherals/UART/HAL/UartIT.hpp"
#include "../../Peripherals/UART/LineParser.hpp"
#include "../../Peripherals/Timer/HAL/Pwm.hpp"
#include "../../Devices/LPS25HB_Async.hpp"

#include "../Inc/peripheralsDefinition.h"

#include "../../Scheduler/Task.hpp"
#include "../../Scheduler/Scheduler.hpp"

Peripherals::HAL::UartIT btHC06Uart{ huart1 }; //PA9 (TX), PA10 (RX)
Peripherals::HAL::I2C_IT i2c1IT{ hi2c1 };
Device::LPS25HB_Async lps25hbAsync{ i2c1IT };
//Peripherals::HAL::Pwm tim3_ch1_pa6{ htim3, 1 }; //PA6


int main()
{
    /* MCU Configuration--------------------------------------------------------*/
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();
    SystemClock_Config();
    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    //MX_USART2_UART_Init();
    MX_USART1_UART_Init();
    MX_I2C1_Init();
    MX_TIM3_Init();

    
    RegisterLevel::SoftwareTimer btUartResetTimer{ 2000 };
    RegisterLevel::SoftwareTimer btUartPollTimer{ 1 };
    UcCommunication::LineParser lineParser{ uart2 };
    UcCommunication::LineParser btLineParser{ btHC06Uart };

    float temperature = 0.0f;
    uint32_t pressure = 0;

    Task<RegisterLevel::SoftwareTimer> readLPS25HBSensorTask{ 50, [&]()
    {
        if (const auto readTemp = lps25hbAsync.ReadTemperature(); readTemp)
            temperature = readTemp.value();

        if (const auto readPressure = lps25hbAsync.ReadPressure(); readPressure)
            pressure = readPressure.value();
    }};

    Task<RegisterLevel::SoftwareTimer> printTemperaturePressureTask{ 500, [&]()
    {
        char messageBuffer[48];
        snprintf(messageBuffer, sizeof(messageBuffer), "Temp: %.2f C, Pressure: %lu hPa\r\n", temperature, pressure);
        uart2.Transmit(reinterpret_cast<const uint8_t*>(messageBuffer), strlen(messageBuffer));
    }};

    Task<RegisterLevel::SoftwareTimer> ld2Task{ 150, [&]()
    {
        ld2.Toggle();
    }};

    Task<RegisterLevel::SoftwareTimer> uart2ReadLineTask{ 1, [&]()
    {
        if (const auto lineOpt = lineParser.ReadLine())
        {
            const auto line = *lineOpt;
            const char* prefix = "RX Uart2IT: ";
            uart2.Transmit(reinterpret_cast<const uint8_t*>(prefix), strlen(prefix));
            uart2.Transmit(reinterpret_cast<const uint8_t*>(line.data()), line.size());
            uart2.Transmit(reinterpret_cast<const uint8_t*>("\r\n"), 2);
        }
    }};
    
    Scheduler<Task<RegisterLevel::SoftwareTimer>, 4> scheduler{ { readLPS25HBSensorTask
                                                      , printTemperaturePressureTask
                                                      , ld2Task
                                                      , uart2ReadLineTask } };

    // LPS25HB test

    uart2.ConfigureInterruptsPriority(IRQn_Type::USART2_IRQn, 1);
    uart2.Init(uart2Tx, uart2Rx, 115200);
    ld2.Init();
    //tim3_ch1_pa6.Start();
    std::size_t pulse = 50;

    RegisterLevel::SoftwareTimer pulseTimer{ 50 };
    //tim3_ch1_pa6.SetPulse(pulse);

    HAL_TIM_Base_Start_IT(&htim3);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);

    while (true)
    {
        // UART 1 Test - Bluetooth HC-06
        // connection at linux
        // sudo rfcomm connect 0 <Address>
        // on second terminal window: screen /dev/rfcomm0 9600

        uart2.ProcessTx();
        btHC06Uart.ProcessTx();
        //uart2.ProcessRx();
        btHC06Uart.ProcessRx();

        //TODO: fix bthc06 reciever
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
        
        // End of UART Test
        if (lps25hbAsync.IsAwake())
            scheduler.Run();
        else
        {
            lps25hbAsync.WakeUp();
        }

        
        
        if (pulseTimer.IsExpired())
        {
            pulseTimer.Reset();
            //tim3_ch1_pa6.SetPulse(pulse);
            __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pulse);
            pulse += 10;
            if (pulse >= 1000)
                pulse = 0;
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

    /*
    if (huart->Instance == USART2)
    {
        uart2.RxCpltCallback();
    }
    */
}

extern "C" void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        btHC06Uart.TxCpltCallback();
    }

    /*
    if (huart->Instance == USART2)
    {
        uart2.TxCpltCallback();
    }
    */
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

extern "C" void USART2_IRQHandler(void)
{
    uart2.IRQHandler();
}

