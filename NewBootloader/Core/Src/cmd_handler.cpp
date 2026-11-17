/*
 * cmd_handler.cpp
 *
 *  Created on: 17-Oct-2026
 *      Author: ashish
 */
#include <UartIO.h>
#include <InterfaceStream.h>
#include "cmd_handler.h"
#include "firmware_updater.h"

extern UartIO IO;

InterfaceStream cout(IO);
extern InterfaceStream debugIO;
extern UartIO IO;

AsPkt SendPacket; //kept global to avoid the stacking and unstacking of sizeof(AsPkt)

void send_operation_ack(const ACKFLAG Flag);


void decode_packet(const AsPkt &Pkt)
{
	ACKFLAG Flag =  ACKFLAG::ACK; // TO prevent the Flag from having some invalid value

//	debugIO << "Got the packet" << endl;
//	debugIO << "Payload Length - " << Pkt.length << endl;
//	debugIO << "Flags - " << static_cast<int>(Pkt.flagsRaised) << endl;
//	debugIO << "PacketID - " << static_cast<int> (Pkt.pkt_id) << endl;
//	debugIO << "Packet : ";
//
//    for(uint16_t Iter = 0; Iter < Pkt.length; ++Iter)
//    {
//    	debugIO << "0x" << hex(Pkt.payload[Iter]) << ' ';
//    }
//
//    debugIO << endl;

    switch(Pkt.pkt_id)
    {
        case PktID::FIRMWARE_UPDATE:
        	Flag = bootloader_handler(Pkt.payload, Pkt.length, Pkt.flagsRaised);
//        	debugIO << "bootloader_handler Flag : " << static_cast<int>(Flag) << endl;
        	break;

        default:
            break;
    }

    send_operation_ack(Flag);
}


void send_operation_ack(const ACKFLAG Flag)
{
    SendPacket.reset();
    SendPacket.pkt_id = PktID::OPERATION_ACK;
    const uint8_t payload = static_cast<uint8_t>(Flag);

    SendPacket.make_packet(&payload, 1);

//    for(uint8_t Iter = 0; Iter < SendPacket.length; ++Iter)
//	{
//		debugIO << hex(SendPacket.payload[Iter]) << ' ';
//	}
//
//    debugIO << endl;

    IO.write(SendPacket.payload, SendPacket.length);
}
