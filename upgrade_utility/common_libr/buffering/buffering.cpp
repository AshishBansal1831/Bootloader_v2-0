#include "buffering.hpp"

#include <iostream>



void BufferBase::add(const uint8_t* buf, uint16_t size) {
    if ((cur_idx_ + size) <= capacity_) {
        memcpy(buffer_ + cur_idx_, buf, size);
        cur_idx_ += size;
    }
}

BufferBase& BufferBase::operator<<(uint8_t byte) {
    if (cur_idx_ < capacity_) {
        buffer_[cur_idx_++] = byte;
    }
    return *this;
}

BufferBase& BufferBase::operator<<(uint16_t no) {
    add(reinterpret_cast<const uint8_t*>(&no), sizeof(no));
    return *this;
}

BufferBase& BufferBase::operator<<(uint32_t no) {
    add(reinterpret_cast<const uint8_t*>(&no), sizeof(no));
    return *this;
}

BufferBase& BufferBase::operator<<(const char* str) {
    if (str) {
        add(reinterpret_cast<const uint8_t*>(str), static_cast<uint16_t>(strlen(str)));
    }
    return *this;
}

BufferBase& BufferBase::operator<<(const BufferBase& other) {
    add(other.buffer_, other.cur_idx_);
    return *this;
}

void BufferBase::print()
{
//    for(uint16_t Iter = 0; Iter < cur_idx_; Iter++)
//    {
//        std::cout << std::hex << (int)buffer_[Iter] << ",";
//    }
//    std::cout << std::endl;
}
