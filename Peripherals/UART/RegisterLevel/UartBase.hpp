#pragma once
//TO connect with you linux terminal (for example at ubuntu)
//
// check tty com ports with
// dmesg | grep tty 		(or sudo if not allow due to reading the kernel buffer failed: Operation not permitted)
// screen /dev/tty*ACMx* [baud rate] (i.e. screen /dev/ttyACM1 115200
//
// to kill screen process: ctrl + A --> K to kill process
// to detach session:      ctrl + A --> D
// available sockets:      screen -ls
// reconnect:              screen -r [ID]
//
// if you cannot reconnect, try: lsof /dev/ttyACM1
// kill [PID]
//
// try once more

#include "../../Config.hpp"
#include <stm32l476xx.h>
#include <array>
#include <concepts>

#include "Peripherals/GPIO/RegisterLevel/GpioAlternate.hpp"

//concept basing on USART_TypeDef struct from CMSIS
template<typename T>
concept USARTx = requires (T uart)
{
	{ uart.CR1 } 		-> std::convertible_to<volatile uint32_t&>;		/*!< USART Control register 1,                 Address offset: 0x00 */
    { uart.CR2 } 		-> std::convertible_to<volatile uint32_t&>;		/*!< USART Control register 2,                 Address offset: 0x04 */
    { uart.CR3 } 		-> std::convertible_to<volatile uint32_t&>;		/*!< USART Control register 3,                 Address offset: 0x08 */
    { uart.BRR } 		-> std::convertible_to<volatile uint32_t&>;		/*!< USART Baud rate register,                 Address offset: 0x0C */
    { uart.GTPR}  		-> std::convertible_to<volatile uint16_t&>;		/*!< USART Guard time and prescaler register,  Address offset: 0x10 */
    { uart.RESERVED2} 	-> std::convertible_to<uint16_t&>;     			/*!< Reserved, 0x12                                                 */
    { uart.RTOR } 		-> std::convertible_to<volatile uint32_t&>;     /*!< USART Receiver Time Out register,         Address offset: 0x14 */
	{ uart.RQR} 		-> std::convertible_to<volatile uint16_t&>;     /*!< USART Request register,                   Address offset: 0x18 */
    { uart.RESERVED3 } 	-> std::convertible_to<uint16_t&>;       		/*!< Reserved, 0x1A                                                 */
	{ uart.ISR} 		-> std::convertible_to<volatile uint32_t&>;     /*!< USART Interrupt and status register,      Address offset: 0x1C */
	{ uart.ICR } 		-> std::convertible_to<volatile uint32_t&>;     /*!< USART Interrupt flag Clear register,      Address offset: 0x20 */
	{ uart.RDR }        -> std::convertible_to<volatile uint16_t&>;		/*!< USART Receive Data register,              Address offset: 0x24 */
	{ uart.RESERVED4 }  -> std::convertible_to<uint16_t&>;				/*!< Reserved, 0x26                                                 */
	{ uart.TDR }        -> std::convertible_to<volatile uint16_t&>;		/*!< USART Transmit Data register,             Address offset: 0x28 */
	{ uart.RESERVED5 }  -> std::convertible_to<uint16_t&>;				/*!< Reserved, 0x2A                                                 */
};


/*Rx and Tx pins needs to be configured/initialized before Uart*/
template<USARTx Usart, std::size_t bufferSize = 80>
class Uart
{
protected:
	volatile Usart* const usart = nullptr;

	void EnableClock()
    {
        //UART2 clock enable
        if (usart == USART1)
            RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
        else if (usart == USART2)
            RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
    }
	
    // TODO configure also for other uarts (especially uart1 for this project)
    // Check in RM uart1 clocks and BRR configurations, same with other peripehrals
    //probably something like in stm32l4xx_hal_rcc.c:
    // HAL_RCC_GetPCLK1Freq(void) and HAL_RCC_GetPCLK2Freq(void) functions
    template<uint32_t baudRate = 115200>
	void UartConfig()
	{
		//USART Baud Rate Register BRR - speed of the USART1
		//UARTDIV (RM 40.8 USART baud rate register) = 80 000 000 / 115200 = 34,7
		//USART2 uses PCLK1 clock by default (reset state 00)
		//know from USART2SEL bits value
		usart->BRR = SystemCoreClock / baudRate; /// system_stm32l4xx.c file

		//frame 8m1 -- 0b00 - for USART_CR1 (reset value)
		//PCE parity control enable: 0 - disabled (reset value), 1 - enabled
		//USART_CR2 STOP register - 00 - 1 stop bit; (RM USART_CR2) reset value
		//USART_CR1 UE (UART enable bit) - 0 -disabled (reset value): 1 - enabled
		usart->CR1 |= USART_CR1_UE; // UART enabled
		usart->CR1 |= USART_CR1_TE; //transmitter enabled - 0 disable, 1- enabled
		usart->CR1 |= USART_CR1_RE; //receiver enabled - 0 disable, 1- enabled
	}

	volatile bool messageReady_ = false;
	std::array<char, bufferSize>::iterator index_;


public:
	Uart(const Uart& source) = delete;
	Uart(Uart&& source) = delete;
	Uart& operator=(const Uart& source) = delete;
	Uart& operator=(Uart&& source) = delete;
	Uart(Usart* const usart_) : usart(usart_) {};

    /*Get appropriate Tx and Rx pins from datasheet*/
    template<Peripherals::RegisterLevel::GpioPort TxPin, Peripherals::RegisterLevel::GpioPort RxPin>
    void Init(TxPin& txPin, RxPin& rxPin)
    {
        txPin.Init();
        rxPin.Init();
        EnableClock();
        UartConfig();
    }

	void ConfigureExtiReceive()
	{
		RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; //enable SYSCFG clock
		//9.2.6 System configuration controller SYSCFG
		SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI3; //0000
		SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI3_PA; //set bit for PA3 exti route to syscfg

		usart->CR1 |= USART_CR1_RXNEIE; //Enable RX interrupt
		NVIC_SetPriority(USART2_IRQn, 1); //set priority (for exti ,  priotity = 1
		NVIC_EnableIRQ(USART2_IRQn);//enable interrupt
		//enum from stm32l476xx.h (CMSIS file) - Interrupt number definition
	};
};
