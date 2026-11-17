/*
 * InterfaceIO.h
 *
 *  Created on: 10-Jul-2026
 *      Author: ashish
 */

#ifndef INTERFACEIO_H_
#define INTERFACEIO_H_

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#ifdef FREERTOS
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#endif


/*
 * Virtual Base Function
 */
class InterfaceIO
{
protected:
#ifdef FREERTOS
	QueueHandle_t Queue;
//	SemaphoreHandle_t RxSem; // Data Receive  Semaphore
#endif
public:

#ifdef FREERTOS
	virtual void init(QueueHandle_t Q) = 0;
#else
	virtual void init() = 0;
#endif
	virtual uint8_t receive() = 0;
	virtual uint32_t receive(uint8_t *byte) = 0; //returns to number of read byte
	virtual void write(uint8_t byte) = 0;
	virtual void write(uint8_t *data, uint16_t Length) = 0;

	void print(const char *ptr)
	{
		write((uint8_t*)ptr, strlen(ptr));
	}
	void printn(const char *ptr, const size_t len)
	{
		write((uint8_t*)ptr, len);
	}
	void printf(const char *format, ...)
	{
		char print_buffer[100];
		va_list args;
		va_start(args, format);

		int written = vsnprintf(print_buffer, sizeof(print_buffer), format, args);

		va_end(args);

		if (written > 0)
		{
			// Optimize: use printn since vsnprintf already calculated the exact string length
			printn(print_buffer, static_cast<size_t>(written));
		}
	}
	size_t get_string_input(char *ReceivedCmd, size_t buf_size, const char *tag = nullptr);
};

#ifdef STM32F407xx



#ifdef USB_USE

class UsbCdcIO : public InterfaceIO
{
public :
#ifdef FREERTOS
	// Explicit Lifecycle/Setup override
	void init(QueueHandle_t Q
			/*, SemaphoreHandle_t RS*/) override;
#else
	void init() override;
#endif
	// Declaring the concrete overrides
	uint8_t receive() override;
	uint32_t receive(uint8_t *byte) override;
	void write(uint8_t byte) override;
	void write(uint8_t *data, uint16_t Length) override;
};
#endif
#endif /* INTERFACEIO_H_ */

#endif
