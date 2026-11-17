#ifndef __FIRMWARE_UPDATER_HPP__
#define __FIRMWARE_UPDATER_HPP__

#include <stdint.h>
#include <vector>

constexpr uint8_t WORD_SIZE = 4; // 4 bytes = 1 Word
constexpr uint16_t FIRMWARE_WRITE_SIZE_BYTES = 128; // 

struct HexParser{
    uint32_t Address;
    std::vector<uint8_t> Data;
};

enum class FirmwarUpdateStages : uint8_t
{
    START_FIRMWARE_UPDATE,
    CONFIRM_ERASE,
    CONTINUE_PACKET_SENDING,
    JUMP_TO_APP,
};

/*
 * Structure definitions
 */

struct FirmwareData
{
    FirmwarUpdateStages Stage = FirmwarUpdateStages::START_FIRMWARE_UPDATE;
    uint8_t fileType; // 1-Hex, 0-Binary
    uint32_t offset;
    uint32_t size;
    uint8_t retyCnt = 0;
    bool waitForPacket = false;
    std::vector<uint8_t> firmwareData;
};

void fileParse();

#endif /* __FIRMWARE_UPDATER_HPP__ */