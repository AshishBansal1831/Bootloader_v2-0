/*
 * UartIO.cpp
 *
 *  Created on: 17-Oct-2026
 *      Author: ashish
 */

#include <UartIO.h>

// Provide the out-of-line storage definition for the static constexpr table
constexpr UartIO::UartDeviceConfig UartIO::UART_CONFIG_TABLE[];

// Initialize the reference using the enum class index to directly index the static table
UartIO::UartIO(UART_INSTANCE port)
    : uart_(UartIO::UART_CONFIG_TABLE[static_cast<size_t>(port)])
#ifdef FREERTOS
    , rx_queue_(nullptr)
#endif
{
}

// ==========================================
// INITIALIZATION LAYER (LL + FREERTOS)
// ==========================================
#ifdef FREERTOS
void UartIO::init(QueueHandle_t q) {
    rx_queue_ = q;

    // Use uart_ directly without any searching loops!
    NVIC_SetPriority(uart_.irq_n, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), uart_.priority, uart_.sub_priority));
    NVIC_EnableIRQ(uart_.irq_n);

    LL_USART_EnableIT_RXNE(uart_.instance);
}
#else
void UartIO::init() {
    // Use uart_ directly
    NVIC_SetPriority(uart_.irq_n, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), uart_.priority, uart_.sub_priority));
    NVIC_EnableIRQ(uart_.irq_n);

    LL_USART_EnableIT_RXNE(uart_.instance);
}
#endif

// ==========================================
// RECEIVE LAYER (FIFO & LL)
// ==========================================
uint8_t UartIO::receive() {
#if USE_FIFO == 1
    uint8_t byte = 0;
    recv_fifo_.pop(byte);
    return byte;
#else
    while (!LL_USART_IsActiveFlag_RXNE(uart_.instance));
    return LL_USART_ReceiveData8(uart_.instance);
#endif
}

uint32_t UartIO::receive(uint8_t *byte) {
    if (!byte) return 0;

#if USE_FIFO == 1
    return recv_fifo_.pop(*byte) ? 1 : 0;
#else
    if (LL_USART_IsActiveFlag_RXNE(uart_.instance)) {
        *byte = LL_USART_ReceiveData8(uart_.instance);
        return 1;
    }
    return 0;
#endif
}

// ==========================================
// TRANSMIT LAYER (FIFO & LL)
// ==========================================
void UartIO::write(uint8_t byte) {
#if USE_FIFO == 1
    while (!tx_fifo_.push(byte)); // Block if FIFO is full
    LL_USART_EnableIT_TXE(uart_.instance); // Trigger hardware transmission via LL
#else
    while (!LL_USART_IsActiveFlag_TXE(uart_.instance));
    LL_USART_TransmitData8(uart_.instance, byte);
#endif
}

void UartIO::write(uint8_t *data, uint16_t length) {
    if (!data || length == 0) return;

#if USE_FIFO == 1
    size_t pushed = 0;
    while (pushed < length) {
        pushed += tx_fifo_.push(data + pushed, length - pushed);
    }
    LL_USART_EnableIT_TXE(uart_.instance);
#else
    for (uint16_t i = 0; i < length; ++i) {
        write(data[i]);
    }
#endif
}

// ==========================================
// ISR HANDLER (LL + FIFO + FREERTOS QUEUE)
// ==========================================
void UartIO::handle_isr() {
    // Handle RX Event via LL
    if (LL_USART_IsActiveFlag_RXNE(uart_.instance) && LL_USART_IsEnabledIT_RXNE(uart_.instance)) {
        uint8_t rx_data = LL_USART_ReceiveData8(uart_.instance);

    #if USE_FIFO == 1
        recv_fifo_.push(rx_data);
    #endif

    #ifdef FREERTOS
        if (rx_queue_ != nullptr) {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            xQueueSendFromISR(rx_queue_, &rx_data, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    #endif
    }

    // Handle TX Empty Event via LL
    if (LL_USART_IsActiveFlag_TXE(uart_.instance) && LL_USART_IsEnabledIT_TXE(uart_.instance)) {
    #if USE_FIFO == 1
        uint8_t tx_data = 0;
        if (tx_fifo_.pop(tx_data)) {
            LL_USART_TransmitData8(uart_.instance, tx_data);
        } else {
            // Buffer empty, disable TXE interrupt until more data is pushed
            LL_USART_DisableIT_TXE(uart_.instance);
        }
    #endif
    }
}
