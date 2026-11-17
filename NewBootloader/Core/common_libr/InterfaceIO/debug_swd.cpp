/*
 * dbgswd.cpp
 *
 *  Created on: 17-Oct-2026
 *      Author: ashish
 */
#include <debug_swd.h>
#include "stm32f4xx.h"
#include "core_cm4.h"

static inline int dbg_has_char(void) {
	// Check if Data Register Not Empty (RXNE equivalent for CoreDebug DWT/ITM channel)
	// Alternatively, check CoreDebug DHCSR or ITM instrument receive flags if supported
	return (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk) &&
		   (ITM->TCR & ITM_TCR_ITMENA_Msk);
}

void DEBUG_SWD::init()
{
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	ITM->TCR |= ITM_TCR_ITMENA_Msk;
	ITM->TER |= 1UL;  // enable port 0
}

uint8_t DEBUG_SWD::receive()
{
	// Wait until data is available
	while (!dbg_has_char());

	// Read the character from the ITM RX register
	return (uint8_t)(ITM->PORT[0].u32);
}

uint32_t DEBUG_SWD::receive(uint8_t *byte)
{
	(void)byte;
//	(void)TimeOut;
	return 0;
}

void DEBUG_SWD::write(uint8_t byte) {
    // Check if a debugger is connected (C_DEBUGEN bit in DHCSR)
    if (!(CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk)) {
        return; // Drop prints silently if no debugger is attached
    }

    if ((ITM->TCR & ITM_TCR_ITMENA_Msk) && (ITM->TER & (1UL << 0))) {
        // Timeout-guarded wait to prevent infinite lockups if FIFO is full
        uint32_t timeout = 10000;
        while (ITM->PORT[0].u32 == 0 && timeout > 0) {
            timeout--;
        }
        if (timeout > 0) {
            ITM->PORT[0].u8 = byte;
        }
    }
}

void DEBUG_SWD::write(uint8_t *data, uint16_t Length)
{
	for(uint16_t Iter = 0; Iter < Length; ++Iter)
	{
		write(data[Iter]);
	}
}

