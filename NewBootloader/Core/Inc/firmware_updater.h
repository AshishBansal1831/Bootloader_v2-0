/*
 * firmware_updater.h
 *
 *  Created on: 22-Oct-2026
 *      Author: ashish
 */

#ifndef INC_FIRMWARE_UPDATER_H_
#define INC_FIRMWARE_UPDATER_H_

#include <stdint.h>
#include "as_pkt.hpp"

constexpr uint32_t APPLICATION_ADDRESS = 0x08008000; //sector 2 start

ACKFLAG bootloader_handler(const uint8_t *payload, const uint16_t len, const uint8_t flags);
void jump_to_app(void);
#endif /* INC_FIRMWARE_UPDATER_H_ */
