/*
 * InterfaceStream.h
 *
 *  Created on: 17-Oct-2026
 *      Author: ashish
 */

#ifndef COMMON_LIBR_INTERFACESTREAM_H_
#define COMMON_LIBR_INTERFACESTREAM_H_

#include "InterfaceIO.h"
#include "safe_string.hpp"

constexpr uint16_t STREAM_BUFFER_SIZE	= 128; // 128 Bytes

class InterfaceStream
{
	InterfaceIO &io_;
	SafeString<STREAM_BUFFER_SIZE> string_;

	void flush_if_needed()
	{
		// Optional: auto-flush if buffer is getting full
		if (string_.length() >= (STREAM_BUFFER_SIZE - 16))
		{
			flush();
		}
	}

public:
	explicit InterfaceStream(InterfaceIO &io) : io_(io) {}

	// 1. Overload for raw C-strings
	template <size_t N>
	InterfaceStream& operator<<(const char (&string)[N])  {
		string_ << string;
		return *this;
	}

	template <size_t N>
	InterfaceStream& operator<<(const SafeString<N>& str) {
		string_ << str; // Append to our running buffer
		flush_if_needed();
		return *this;
	}

	// CRITICAL: Intercept stream manipulators (like endl)
	InterfaceStream& operator<<(InterfaceStream& (*os_func)(InterfaceStream&)) {
		return os_func(*this);
	}

	template <typename T>
	InterfaceStream &operator << (const T &data)
	{
		string_ << data;
		return *this;
	}

	void flush() {
		if (!string_.empty()) {
			io_.write(reinterpret_cast<uint8_t*>(string_.c_str()), string_.length() % STREAM_BUFFER_SIZE);
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

#endif /* COMMON_LIBR_INTERFACESTREAM_H_ */
