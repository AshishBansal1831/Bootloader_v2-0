/*
 * dbgswd.h
 *
 *  Created on: 17-Oct-2026
 *      Author: ashish
 */

#ifndef _INTERFACEIO_DBGSWD_H_
#define _INTERFACEIO_DBGSWD_H_

#include "InterfaceIO.h"

#define RECEIVE_NONBLOCKER	(0)
#define RECEIVE_BLOCKER	(1)

#define RECEIVE_BEHAVIOUR	RECEIVE_BLOCKER

class DEBUG_SWD : public InterfaceIO
{
public:
//	dbg_swd();
	void init() override;
	uint8_t receive() override;
	uint32_t receive(uint8_t *byte) override; // added this
	void write(uint8_t byte) override;
	void write(uint8_t *data, uint16_t Length) override;
	// Check if a character is available in the ITM receiver channel

//	virtual ~dbg_swd();
};

#endif /* COMMON_LIBR_INTERFACEIO_DBGSWD_H_ */
