/*
 * InterfaceIO.cpp
 *
 *  Created on: 10-Jul-2026
 *      Author: ashish
 */

#include "InterfaceIO.h"
#ifdef STM32F407xx
#include "usart.h"
#include "stm32f4xx_ll_usart.h"

#ifdef USB_USE
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "stream_buffer.h"

StreamBufferHandle_t UsbCdcStreamBuffer = NULL;
#endif

size_t InterfaceIO::get_string_input(char *ReceivedCmd, const size_t buf_size, const char *tag)
{
    char received_char = 0;
    size_t index = 0;

    if(tag != nullptr)
    {
    	this->print(tag);
    }

    do
    {
    	received_char = (char)this->receive();

        switch (received_char)
        {
            case BACK_SPACE:
            	this->write(' ');
            	this->write(BACK_SPACE);
            	this->write(BACK_SPACE);
            	ReceivedCmd[index--] = NULL_CHAR;
                break;

            case CAR_RET:
            case ENTER:
            	ReceivedCmd[index] = NULL_CHAR;
            	this->print("\r\n");
                break;

            default:
            	this->write(received_char);
                ReceivedCmd[index++] = received_char;
                break;
        }

    } while (received_char != ENTER && received_char != CAR_RET && buf_size);

    return index;
}


#ifdef USB_USE
/*******************UsbCdcIO Handling***********************/
#ifdef FREERTOS
void UsbCdcIO::init(QueueHandle_t Q/*, SemaphoreHandle_t RS */)
{

#else 
void UsbCdcIO::init()
{

#endif
	if (UsbCdcStreamBuffer == NULL)
	{
		UsbCdcStreamBuffer = xStreamBufferCreate(256, 1);
	}

	// Force the USB Interrupt priority down so FreeRTOS APIs can run inside it safely
	HAL_NVIC_SetPriority(OTG_FS_IRQn, 6, 0);
	HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
}

uint8_t UsbCdcIO::receive()
{
	uint8_t rxByte = 0;

	if (UsbCdcStreamBuffer != NULL) {
		// This blocks completely (portMAX_DELAY) until a byte is available.
		// When CDC_Receive_FS pushes data, this instantly wakes up and returns it.
		xStreamBufferReceive(UsbCdcStreamBuffer, &rxByte, 1, portMAX_DELAY);
	}
	else
	{
		print("StreamBuffer is null for somet reason\r\n");
	}

	return rxByte;
}

uint8_t UsbCdcIO::receive(uint8_t *byte, uint32_t TimeOut)
{
	if(xStreamBufferReceive(UsbCdcStreamBuffer, byte, 1, TimeOut) == pdPASS)
	{
		return 1;
	}
	return 0;
}

void UsbCdcIO::write(uint8_t byte)
{
#ifdef FREERTOS
    uint32_t timeout = 5000; // Defensive loop counter
    while (CDC_Transmit_FS(&byte, 1) == USBD_BUSY)
    {
        if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
        {
            vTaskDelay(pdMS_TO_TICKS(1)); // Yield to let USB IRQ clear the buffer
        }
        else
        {
            // If scheduler hasn't started yet, use a manual spin timeout
            if (--timeout == 0) break;
        }
    }
#else
    while (CDC_Transmit_FS(&byte, 1) == USBD_BUSY)
    {
        HAL_Delay(1);
    }
#endif
}

void UsbCdcIO::write(uint8_t *data, uint16_t Length)
{
    if (Length == 0) return;

    uint32_t timeout = 5000;
    while (CDC_Transmit_FS(data, Length) == USBD_BUSY)
    {
#ifdef FREERTOS
        if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
        {
            vTaskDelay(pdMS_TO_TICKS(1));
        }
        else
        {
            if (--timeout == 0) break;
        }
#else
        Hal_Delay(1);
#endif
    }
}


#endif

#endif
