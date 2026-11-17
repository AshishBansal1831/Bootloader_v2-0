#include <array>
//#include <iostream>
#include <cstdint>
#include <cstddef>
#include <string.h>
#include "as_pkt.hpp"
#include "crc/crc_.hpp"

#include <iostream>

/*
 * brief : Sync the incoming packet
 */
int8_t AsPkt::get_packet(const uint8_t recvByte)
{
    switch(state)
    {
        case SyncState::HEADER1 :
            if(recvByte == 0xA5)
            {
                state = SyncState::HEADER2;
            }
            else
            {
                noDroppedBytes++;
            }
            break;

        case SyncState::HEADER2:
            if(recvByte == 0x5A)
            {
                state = SyncState::PROTO_VER;
                calcCrc = 0;
            }
            else
            {
                state = SyncState::HEADER1;
                noDroppedBytes++;
            }
            break;

        case SyncState::PROTO_VER:
            // protocolVer = recvByte; // ignored for v1.0
            state = SyncState::FLAG;
            {
                uint8_t idx = (uint8_t)((calcCrc >> 8) ^ recvByte);
                calcCrc = (uint16_t)((calcCrc << 8) ^ CRC16_TABLE[idx]);
            }
            break;

        case SyncState::FLAG:
            flagsRaised = recvByte;
            state = SyncState::PKT_ID;
            {
                uint8_t idx = (uint8_t)((calcCrc >> 8) ^ recvByte);
                calcCrc = (uint16_t)((calcCrc << 8) ^ CRC16_TABLE[idx]);
            }
            break;

        case SyncState::PKT_ID:
            pkt_id = static_cast<PktID>(recvByte);
            state = SyncState::LENGTH;
            length = 0;
            noBytes = 0;
            
            {
                uint8_t idx = (uint8_t)((calcCrc >> 8) ^ recvByte);
                calcCrc = (uint16_t)((calcCrc << 8) ^ CRC16_TABLE[idx]);
            }
            break;

        case SyncState::LENGTH:
            length |= recvByte << (8 * noBytes);
            noBytes++;
            {
                
                uint8_t idx = (uint8_t)((calcCrc >> 8) ^ recvByte);
                calcCrc = (uint16_t)((calcCrc << 8) ^ CRC16_TABLE[idx]);
            }

            if(noBytes == 2)
            {
                if(length < MAX_PAYLOAD_LENGTH)
                {
                    index = 0;
                    state = SyncState::PAYLOAD;
                }
                else
                {
                    state = SyncState::HEADER1;
                }
            }
            
            break;

        case SyncState::PAYLOAD:
            payload[index++] = recvByte;     
            {
                uint8_t idx = (uint8_t)((calcCrc >> 8) ^ recvByte);
                calcCrc = (uint16_t)((calcCrc << 8) ^ CRC16_TABLE[idx]);
            }       
            if(index == length)
            {
                state = SyncState::CRC16;
                noBytes = 0;
                recvCrc = 0;
            }
            break;

        case SyncState::CRC16:
            recvCrc |=  recvByte << (8 * noBytes);
            noBytes++;

            if(noBytes == 2)
            {
                state = SyncState::HEADER1;
                // std::cout << "CRC " << (uint32_t) recvCrc << " " <<(uint32_t) calcCrc << std::endl;
                if(recvCrc == calcCrc)
                {
                    return 1;
                }
                else
                {
                    return -1;
                }
            }
            break;
    }
    return 0;
}

/*
 * brief : create the binary packet to send
 */
void AsPkt::make_packet(const uint8_t *buffer, const uint16_t payload_len)
{
    length = 0;
    index = 0;

    payload[0] = 0xA5;
    payload[1] = 0x5A;
    payload[2] = protocolVer;
    payload[3] = flagsRaised;
    payload[4] = static_cast<uint8_t>(pkt_id);

    memcpy(payload + 5, &payload_len, sizeof(uint16_t));
    memcpy(payload + 5 + sizeof(uint16_t), buffer, payload_len);

    uint16_t idx = 5 + sizeof(uint16_t) + payload_len; 

    const uint16_t crc16 = calculate_crc16(payload + 2, idx - 2, 0); 

    memcpy(payload + idx, &crc16, sizeof(uint16_t));

    length = idx + 2;

}
