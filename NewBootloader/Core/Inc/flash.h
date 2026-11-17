/*
 * flash.h
 *
 *  Created on: Oct 28, 2025
 *      Author: Ashish Bansal
 */

#ifndef INC_FLASH_H_
#define INC_FLASH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_flash.h"
#include "stm32f4xx_hal_flash_ex.h"

HAL_StatusTypeDef EraseFlash(const uint32_t StartBank, const uint8_t no_of_banks);
HAL_StatusTypeDef write_bank(uint32_t StartAddress, uint8_t *Buffer, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif /* INC_FLASH_H_ */
