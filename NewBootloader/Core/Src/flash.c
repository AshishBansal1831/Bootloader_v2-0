/*
 * flash.c
 *
 *  Created on: Oct 28, 2025
 *      Author: Ashish Bansal
 */
#include <stdio.h>
#include "flash.h"
#include "string.h"

HAL_StatusTypeDef write_bank(uint32_t StartAddress,
                             uint8_t *Buffer,
                             uint32_t size)
{
    HAL_StatusTypeDef status;

    HAL_FLASH_Unlock();

    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR |
                           FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR |
                           FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    __disable_irq();

    for (uint32_t i = 0; i < size; i += 4)
    {
        uint32_t data32 = 0xFFFFFFFF;
        memcpy(&data32, Buffer + i, 4);

        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
                                   StartAddress + i,
                                   data32);

        if (status != HAL_OK)
        {
            printf("Write error %d\r\n", status);
            break;
        }
    }

    __enable_irq();

    HAL_FLASH_Lock();

    return status;
}

HAL_StatusTypeDef EraseFlash(const uint32_t StartBank, const uint8_t no_of_banks)
{
    FLASH_EraseInitTypeDef eraseInitStruct;
    uint32_t sectorError;

    HAL_FLASH_Unlock();

    eraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
    eraseInitStruct.VoltageRange = VOLTAGE_RANGE_3;
    eraseInitStruct.Sector = StartBank;  // Adjust according to your app start
    eraseInitStruct.NbSectors = no_of_banks;            // Erase till end of flash
    __disable_irq();
    if (HAL_FLASHEx_Erase(&eraseInitStruct, &sectorError) != HAL_OK)
    {
        return HAL_ERROR;
    }
    __enable_irq();
    /* VERY IMPORTANT */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR |
                           FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR |
                           FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    HAL_FLASH_Lock();
    return HAL_OK;
}
