#ifndef __AS_PKT_HPP__
#define __AS_PKT_HPP__

#include <stdint.h>

constexpr uint16_t MAX_PAYLOAD_LENGTH = 512;

enum class PktID : uint8_t {
    NO_PACKET       = 0x00, // invalid packet
    OPERATION_ACK   = 0x05,
    FIRMWARE_UPDATE = 0xAB,
    DEBUG_PRINT     = 0x45,
};

enum class ACKFLAG : int8_t {
	NACK_ERROR	= -4,
	NACK_LOCKED	= -3,
	NACK_OUT_OF_RANGE = -2,
    NACK_INVALID = -1,
    ACK = 0,
    ACK_BOOT_RESP1 = 1,
    ACK_BOOT_RESP2 = 2,
};

class AsPkt
{
    enum class SyncState : uint8_t {
        HEADER1,
        HEADER2,
        PROTO_VER,
        FLAG,
        PKT_ID,
        LENGTH,
        PAYLOAD,
        CRC16
    };

    uint8_t protocolVer = 0;
    SyncState state = SyncState::HEADER1;
    uint16_t calcCrc; // CRC-16, using lookup table
    uint16_t recvCrc; // CRC-16
    uint8_t noBytes;
    uint16_t index = 0;

public:
    uint8_t  flagsRaised = 0;
    PktID    pkt_id = PktID::NO_PACKET;
    uint16_t length = 0; // payload length
    uint8_t  payload[MAX_PAYLOAD_LENGTH];
    uint32_t noDroppedBytes = 0; // no of bytes dropped

    void reset() 
    {
        index = 0;
        length = 0;
        calcCrc = 0;
    }

    int8_t get_packet(const uint8_t recvByte);
    void make_packet(const uint8_t *payload, const uint16_t payload_len);
};

void decode(AsPkt &Pkt); // will depend upon whether the device is host or device


#endif /* __AS_PKT_HPP__ */