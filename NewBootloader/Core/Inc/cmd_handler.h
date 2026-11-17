/*
 * cmd_handler.h
 *
 *  Created on: 17-Oct-2026
 *      Author: ashish
 */

#ifndef INC_CMD_HANDLER_H_
#define INC_CMD_HANDLER_H_

#include "as_pkt.hpp"

void decode_packet(const AsPkt &Pkt);
void send_operation_ack(const ACKFLAG Flag);

#endif /* INC_CMD_HANDLER_H_ */
