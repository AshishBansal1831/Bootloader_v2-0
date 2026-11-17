#include "debug_packet.hpp"

//#include <iostream>

DebugPacketBase& DebugPacketBase::operator<<(uint8_t byte)
{
    _buffer << (uint8_t)(DebugDataType::U8) << byte;
    return (*this);
}

DebugPacketBase& DebugPacketBase::operator<<(uint16_t no)
{
    _buffer << (uint8_t)(DebugDataType::U16) << no;
    return (*this);
}
DebugPacketBase& DebugPacketBase::operator<<(uint32_t no)
{
    _buffer << (uint8_t)(DebugDataType::U32) << no;
    return (*this);
}

DebugPacketBase& DebugPacketBase::operator<<(const char* str)
{
    _buffer << (uint8_t)(DebugDataType::STR) << (uint16_t)strlen(str) << str;
    return (*this);
}

DebugPacketBase &DebugPacketBase::operator<<(int8_t byte)
{
    _buffer << (uint8_t)(DebugDataType::I8) << (uint8_t)byte;
    return (*this);
}

DebugPacketBase &DebugPacketBase::operator<<(int16_t no)
{
    _buffer << (uint8_t)(DebugDataType::I16) << (uint16_t)no;
    return (*this);
}

DebugPacketBase &DebugPacketBase::operator<<(int32_t no)
{
    _buffer << (uint8_t)(DebugDataType::I32) << (uint32_t)no;
    return (*this);
}


DebugPacketBase& DebugPacketBase::u8_buf(uint8_t *buf, uint16_t len)
{
    _buffer << (uint8_t)(DebugDataType::U8P) << len;
    _buffer.add(buf, len);
    return (*this);
}
