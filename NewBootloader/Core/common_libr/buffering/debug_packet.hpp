#ifndef __DEBUG_PACKET_HPP__
#define __DEBUG_PACKET_HPP__

#include "buffering.hpp"

enum class DebugDataType : uint8_t {
    U8, // uint8_t
    I8, // int8_t 
    CH, // char
    U16, 
    I16,
    U32,
    I32,
    U64,
    I64,
    F32, // float
    F64, // double
    U8P, // U8 Ptr 
    STR,
};

class DebugPacketBase {
    
public:
    const char *_tag;
    uint8_t _tag_size;
    BufferBase &_buffer;
    
    DebugPacketBase(const char *Tag, const uint16_t TagSize, BufferBase &Base) : _tag(Tag), _tag_size(TagSize), _buffer(Base) {}
    
    DebugPacketBase& operator<<(uint8_t byte);
    DebugPacketBase& operator<<(uint16_t no);
    DebugPacketBase& operator<<(uint32_t no);
    DebugPacketBase& operator<<(const char* str);
    DebugPacketBase& operator<<(int8_t byte);
    DebugPacketBase& operator<<(int16_t no);
    DebugPacketBase& operator<<(int32_t no);
    DebugPacketBase& u8_buf(uint8_t *buf, uint16_t len);
    void print()
    {
        _buffer.print();
    }

    const uint8_t *get_buf() {
        return _buffer.data();
    }

    uint16_t length() {
        return _buffer.length();
    }

    friend void debug_packet(DebugPacketBase &Packet);
};

template <uint16_t Size>
class DebugPacket  : public DebugPacketBase 
{
    Buffering<Size> _buff;
public:

    template<size_t N>
    DebugPacket(const char (&Tag)[N]) : DebugPacketBase(Tag, N - 1, _buff) {
        _buff << Tag;
    }
};

#endif /* __DEBUG_PACKET_HPP__ */