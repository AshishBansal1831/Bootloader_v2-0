#ifndef __CRC_16_H__
#define __CRC_16_H__

#include <cstdint>
#include <cstddef>

extern const uint16_t CRC16_TABLE[256];
uint16_t calculate_crc16(const uint8_t *data, size_t length, uint16_t initial_crc);


#endif /* __CRC_16_H__ */