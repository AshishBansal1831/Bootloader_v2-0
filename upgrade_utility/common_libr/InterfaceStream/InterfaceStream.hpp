#ifndef __INTERFACE_STREAM_HPP__
#define __INTERFACE_STREAM_HPP__

#include "safe_string.hpp"
#include "InterfaceIO.h"

class InterfaceStream {
    InterfaceIO &io_;
    SafeString<128> string_;

    void flush_if_needed() {
        // Optional: auto-flush if buffer is getting full
        if (string_.cur_idx_ >= (128 - 16)) 
        {
            flush();
        }
    }

public:
    explicit InterfaceStream(InterfaceIO& io) : io_(io) {}

    // 1. Overload for raw C-strings
    template <size_t N>
    InterfaceStream& operator<<(const char (&string)[N])  {
        string_ << str;
        return *this;
    }

    template <size_t N>
    InterfaceStream& operator<<(const SafeString<N>& str) {
        string_ += str; // Append to our running buffer
        flush_if_needed();
        return *this;
    }

    void flush() {
        if (!string_.empty()) {
            io_.write_buffer(reinterpret_cast<const uint8_t*>(string_.c_str()), string_.length());
            string_.clear(); // Reset for next chain
        }
    }
};

// Custom endl that forces a flush
inline InterfaceStream& endl(InterfaceStream& stream) {
    stream << "\r\n";
    stream.flush();
    return stream;
}


#endif /* __INTERFACE_STREAM_HPP__ */