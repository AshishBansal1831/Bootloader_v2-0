#ifndef __SAFE_STRING_HPP__
#define __SAFE_STRING_HPP__

#include <stdint.h>
#include <iostream>
#include <string.h>

// Helper structure to hold hex value and padding width
struct HexFormat {
    uint32_t val;
    uint8_t width;
};

class BaseString {

private :

    uint8_t get_no_digits(const uint32_t no)
    {
        if (no < 100000U) {
            if (no < 100U) {
                return (no < 10U) ? 1 : 2;
            } else {
                if (no < 1000U) return 3;
                return (no < 10000U) ? 4 : 5;
            }
        } else {
            if (no < 10000000U) {
                return (no < 1000000U) ? 6 : 7;
            } else {
                if (no < 100000000U) return 8;
                return (no < 1000000000U) ? 9 : 10;
            }
        }
    }
    /**
     * @brief Converts uint32_t to ASCII directly into destination buffer.
     */
    uint16_t u32_to_str(uint32_t val, char *str_buf) {
        char temp[10]; // Max 32-bit uint is 4,294,967,295 (10 digits)
        uint16_t temp_idx = 0;

        if (val == 0U) {
            str_buf[0] = '0';
            return 1;
        }

        while (val > 0U) {
            temp[temp_idx++] = (char)('0' + (val % 10U));
            val /= 10U;
        }

        uint16_t out_idx = 0;
        while (temp_idx > 0) {
            str_buf[out_idx++] = temp[--temp_idx];
        }

        return out_idx; // Return digits written
    }

    /**
     * @brief Safe append helper for signed int32_t without buffer reversal.
     */
    void append_no(int32_t N) {
        // Leave space for potential '-' sign, digits, and trailing null
        if (cur_idx_ >= Size - 1) return;

        uint32_t abs_val;
        if (N < 0) {
            buffer_[cur_idx_++] = '-';
            // Safe conversion for INT32_MIN without overflow
            abs_val = static_cast<uint32_t>(-(N + 1)) + 1U;
        } else {
            abs_val = static_cast<uint32_t>(N);
        }

        // Calculate maximum space remaining excluding mandatory '\0'
        uint16_t space_left = (Size - 1) - cur_idx_;
        uint16_t written = u32_to_str(abs_val, buffer_ + cur_idx_);

        if (written > space_left) {
            // Truncate if remaining capacity isn't enough
            cur_idx_ += space_left;
        } else {
            cur_idx_ += written;
        }
        
        buffer_[cur_idx_] = '\0';
    }

protected :

    char *buffer_ = nullptr;

public:

    uint16_t cur_idx_ = 0;
    size_t Size = 0;

    __attribute__((always_inline)) inline BaseString(char *P_buf, const uint16_t SIZE) : buffer_(P_buf), Size(SIZE) {
        buffer_[0] = '\0';
    }

    __attribute__((always_inline)) inline void clear() {
        cur_idx_ = 0;
        buffer_[0] = '\0';
    }

    // Append single char
    __attribute__((always_inline)) inline BaseString &operator << (const char c) {
        if (cur_idx_ < Size - 1) { // Reserve 1 byte for null terminator
            buffer_[cur_idx_++] = c;
            buffer_[cur_idx_] = '\0';
        }
        return *this;
    }

    // Append standard signed integer
    __attribute__((always_inline)) inline BaseString &operator << (int no) {
    	append_no(static_cast<int32_t>(no));
    	return *this;
    }

    // Append standard unsigned integer
    __attribute__((always_inline)) inline BaseString &operator << (unsigned int no) {
    	if (cur_idx_ < Size - 1) {
    		uint16_t space_left = (Size - 1) - cur_idx_;
    		uint16_t written = u32_to_str(static_cast<uint32_t>(no), buffer_ + cur_idx_);
    		if (written > space_left) {
    			cur_idx_ += space_left;
    		} else {
    			cur_idx_ += written;
    		}
    		buffer_[cur_idx_] = '\0';
    	}
    	return *this;
    }

    __attribute__((always_inline)) inline BaseString &operator << (long no) {
    	append_no(static_cast<int32_t>(no));
    	return *this;
    }

    __attribute__((always_inline)) inline BaseString &operator << (unsigned long no) {
    	*this << static_cast<unsigned int>(no);
    	return *this;
    }

    // Append another SafeString
    __attribute__((always_inline)) inline BaseString &operator << (const BaseString &other) {
        if (cur_idx_ < Size - 1) {
            uint16_t copy_len = other.cur_idx_;
            uint16_t space_left = (Size - 1) - cur_idx_;
            if (copy_len > space_left) {
                copy_len = space_left;
            }
            memcpy(buffer_ + cur_idx_, other.buffer_, copy_len);
            cur_idx_ += copy_len;
            buffer_[cur_idx_] = '\0';
        }
        return *this;
    }

    __attribute__((always_inline)) inline BaseString &append_hex(uint32_t val, uint8_t width = 0) {
		if (cur_idx_ >= Size - 1) return *this;

		const char hex_chars[] = "0123456789ABCDEF";
		char temp[8];
		uint8_t temp_idx = 0;

		uint32_t temp_val = val;
		do {
			temp[temp_idx++] = hex_chars[temp_val & 0x0F];
			temp_val >>= 4;
		} while (temp_val > 0 && temp_idx < 8);

		uint8_t total_digits = (width > temp_idx) ? width : temp_idx;
		if (total_digits > 8) total_digits = 8;

		uint16_t space_left = (Size - 1) - cur_idx_;
		if (total_digits > space_left) {
			total_digits = static_cast<uint8_t>(space_left);
		}

		int leading_zeros = total_digits - temp_idx;
		if (leading_zeros < 0) leading_zeros = 0;

		char *dest = buffer_ + cur_idx_;
		uint16_t written = 0;

		for (int i = 0; i < leading_zeros && written < total_digits; ++i) {
			dest[written++] = '0';
		}

		while (temp_idx > 0 && written < total_digits) {
			dest[written++] = temp[--temp_idx];
		}

		cur_idx_ += written;
		buffer_[cur_idx_] = '\0';
		return *this;
	}

    // Stream operator overload for the hex wrapper struct
	__attribute__((always_inline)) inline BaseString &operator << (const HexFormat &h) {
		return append_hex(h.val, h.width);
	}

    // Template Overload for string literals ("text")
//    template <size_t N>
//    __attribute__((always_inline)) inline BaseString &operator << (const char (&string)[N]) {
//        // N includes '\0', text length is N - 1
//        constexpr size_t str_len = N - 1;
//        if (cur_idx_ < Size - 1) {
//            uint16_t copy_len = static_cast<uint16_t>(str_len);
//            uint16_t space_left = (Size - 1) - cur_idx_;
//            if (copy_len > space_left) {
//                copy_len = space_left;
//            }
//            memcpy(buffer_ + cur_idx_, string, copy_len);
//            cur_idx_ += copy_len;
//            buffer_[cur_idx_] = '\0';
//        }
//        return *this;
//    }

    // Append runtime C-string pointer (const char*)
	__attribute__((always_inline)) inline BaseString &operator << (const char *str) {
		if (str && cur_idx_ < Size - 1) {
			while (*str != '\0' && cur_idx_ < Size - 1) {
				buffer_[cur_idx_++] = *str++;
			}
			buffer_[cur_idx_] = '\0';
		}
		return *this;
	}

    char *c_str() {
    	return buffer_;
    }

    __attribute__((always_inline)) inline uint16_t length() {
    	return cur_idx_;
    }

    __attribute__((always_inline)) inline bool empty() {
		return (cur_idx_ == 0 ? true : false);
	}
};

template <uint16_t SIZE>
class SafeString : public BaseString{
private:
    char buffer_der[SIZE];

public:
    SafeString() : BaseString(buffer_der, SIZE) {}
};

// Factory function for clean syntax: hex(value, width)
inline HexFormat hex(uint32_t val, uint8_t width = 0) {
    return HexFormat{val, width};
}

#endif
