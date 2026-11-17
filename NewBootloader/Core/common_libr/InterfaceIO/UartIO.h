/*
 * UartIO.h
 *
 *  Created on: 17-Oct-2026
 *      Author: ashish
 */

#ifndef COMMON_LIBR_INTERFACEIO_UARTIO_H_
#define COMMON_LIBR_INTERFACEIO_UARTIO_H_

#include "InterfaceIO.h"
#include "FIFO.h"

// STM32 LL Library Headers
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_gpio.h"

// Conditional FreeRTOS Headers
#ifdef FREERTOS
#include "FreeRTOS.h"
#include "queue.h"
#endif

constexpr size_t UART_FIFO_CAP = 128;

#ifndef USE_FIFO
#define USE_FIFO 1
#endif

enum class UART_INSTANCE : uint8_t {
	USART1_INST,
	USART2_INST,
	USART3_INST,
	UART4_INST,
	UART5_INST,
	USART6_INST,
};

struct UartDeviceConfig;

class UartIO : public InterfaceIO {
private:
	// Structure defining hardware parameters for a UART port
	struct UartDeviceConfig {
	    USART_TypeDef* instance;
	    IRQn_Type irq_n;
	    uint8_t priority;
	    uint8_t sub_priority;
	};

	// Global static array mapping STM32F4 UART peripherals to their parameters
	static constexpr UartDeviceConfig UART_CONFIG_TABLE[] = {
	    { USART1, USART1_IRQn, 0, 0 },
	    { USART2, USART2_IRQn, 1, 0 },
	    { USART3, USART3_IRQn, 2, 0 },
	    { UART4,  UART4_IRQn,  3, 0 },
	    { UART5,  UART5_IRQn,  4, 0 },
	    { USART6,  USART6_IRQn,  5, 0 }
	};

	const UartDeviceConfig& uart_;

	// Helper function to look up configuration by instance pointer
//	inline const UartDeviceConfig* find_uart_config(USART_TypeDef* instance) {
//	    for (const auto& cfg : UART_CONFIG_TABLE) {
//	        if (cfg.instance == instance) {
//	            return &cfg;
//	        }
//	    }
//	    return nullptr; // Not found
//	}


#if USE_FIFO == 1
    Fifo<uint8_t, UART_FIFO_CAP> recv_fifo_;
    Fifo<uint8_t, UART_FIFO_CAP> tx_fifo_;
#endif

#ifdef FREERTOS
    QueueHandle_t rx_queue_;
#endif

public:
    explicit UartIO(UART_INSTANCE instance);

    // Lifecycle / Setup (Conditionally compiled for FreeRTOS)
#ifdef FREERTOS
    void init(QueueHandle_t q);
#else
    void init() override;
#endif

    // Concrete Overrides for InterfaceIO
    uint8_t receive() override;
    uint32_t receive(uint8_t *byte) override;
    void write(uint8_t byte) override;
    void write(uint8_t *data, uint16_t length) override;

    // ISR Helper to feed buffers/queues from hardware interrupts
    void handle_isr();
};
#endif /* COMMON_LIBR_INTERFACEIO_UARTIO_H_ */
