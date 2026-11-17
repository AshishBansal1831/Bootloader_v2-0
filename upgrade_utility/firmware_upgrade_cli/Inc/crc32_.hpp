#ifndef __CRC32_H__
#define __CRC32_H__

#include <stdint.h>
#include <vector>
#include <cstddef>

uint32_t calculate_crc32(const uint8_t* data, size_t length);
uint32_t calculate_crc32(std::vector<uint8_t> payload);

#endif