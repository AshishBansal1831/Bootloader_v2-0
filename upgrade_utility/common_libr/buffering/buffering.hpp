#ifndef __BUFFERING_HPP__
#define __BUFFERING_HPP__

#include <stdint.h>
#include <stddef.h>
#include <string.h>

// 1. NON-TEMPLATE BASE CLASS (Single copy of machine code in Flash)
class BufferBase {
protected:
    uint8_t* const buffer_;
    const uint16_t capacity_;
    uint16_t cur_idx_ = 0;

    BufferBase(uint8_t* buf, uint16_t capacity) 
        : buffer_(buf), capacity_(capacity) {}

public:
    void clear() { 
		memset(buffer_, 0, cur_idx_);	
		cur_idx_ = 0; 
	}
    uint16_t length() const { return cur_idx_; }
    uint16_t capacity() const { 
		return capacity_; 
	}
    const uint8_t* data() const { return buffer_; }

    void add(const uint8_t* buf, uint16_t size);

    BufferBase& operator<<(uint8_t byte);
    BufferBase& operator<<(uint16_t no);
    BufferBase& operator<<(uint32_t no);
    BufferBase& operator<<(const char* str);
    BufferBase& operator<<(const BufferBase& other);

    void print();
};

// 2. THIN TEMPLATE WRAPPER (Generates ZERO additional code in Flash)
template <uint16_t Size>
struct Buffering : public BufferBase {
    uint8_t storage_[Size];

    Buffering() : BufferBase(storage_, Size) {}

    Buffering(const uint8_t* buf, uint16_t size) : BufferBase(storage_, Size) {
        add(buf, size);
    }
};

class DecodeBuffering {
	uint8_t *buffer_;
	uint16_t SIZE;
	uint32_t outIndx_ = 0;
public:

	DecodeBuffering(uint8_t *buf, uint16_t size) : buffer_(buf), SIZE(size) {}

	DecodeBuffering& operator>>(uint8_t &byte) {
		if(outIndx_ < SIZE)
		{
			byte = buffer_[outIndx_++];
		}

		return *this;
	}

	template <typename T>
	DecodeBuffering& operator>>(T &no) {
		if((outIndx_ + sizeof(T)) <= SIZE)
		{
			memcpy(&no, &buffer_[outIndx_], sizeof(T));
			outIndx_ += sizeof(T);
		}

		return *this;
	}
	DecodeBuffering& operator>>(uint32_t &no);
	DecodeBuffering& operator>>(const char* str);
};

#endif // __BUFFERING_HPP__