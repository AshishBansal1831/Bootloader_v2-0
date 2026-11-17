/*
 * FIFO.h
 *
 *  Created on: 17-Oct-2026
 *      Author: ashish
 */

#ifndef COMMON_LIBR_FIFO_FIFO_H_
#define COMMON_LIBR_FIFO_FIFO_H_

#include <stdint.h>
#include <stddef.h>
#include <cstring>

constexpr uint16_t FIFO_LENGTH = 128;

template <typename T, size_t Capacity>
class Fifo {
private:
    T buffer_[Capacity];
    volatile size_t head_;
    volatile size_t tail_;
    volatile size_t count_;

public:
    Fifo() : head_(0), tail_(0), count_(0) {}

    // Small accessor functions marked inline
    inline bool is_empty() const {
        return (count_ == 0);
    }

    inline bool is_full() const {
        return (count_ == Capacity);
    }

    inline size_t size() const {
        return count_;
    }

    inline size_t capacity() const {
        return Capacity;
    }

    inline void clear() {
        head_ = 0;
        tail_ = 0;
        count_ = 0;
    }

    // Push item into the FIFO
    bool push(const T& item) {
        if (is_full()) {
            return false; // Buffer overflow protection
        }

        buffer_[tail_] = item;
        tail_ = (tail_ + 1) % Capacity;
        count_++;
        return true;
    }

    // Bulk push function using memcpy (zero for-loops)

    size_t push(const T* data, size_t length) {
        if (length == 0 || is_full() || data == nullptr) {
            return 0;
        }

        // Determine how many items we can actually fit
        size_t available = Capacity - count_;
        size_t to_push = (length < available) ? length : available;

        if (to_push == 0) {
            return 0;
        }

        // Check if the write operation wraps around the end of the circular buffer
        size_t first_chunk = Capacity - tail_;

        if (to_push <= first_chunk) {
            // Case 1: Fits entirely in the remaining tail space (Single memcpy)
            std::memcpy(&buffer_[tail_], data, to_push * sizeof(T));
            tail_ = (tail_ + to_push) % Capacity;
        } else {
            // Case 2: Wraps around to the beginning of the buffer (Two memcpy chunks)
            std::memcpy(&buffer_[tail_], data, first_chunk * sizeof(T));
            size_t second_chunk = to_push - first_chunk;
            std::memcpy(&buffer_[0], data + first_chunk, second_chunk * sizeof(T));
            tail_ = second_chunk;
        }

        count_ += to_push;
        return to_push; // Returns number of items successfully pushed
    }

    // Pop item from the FIFO

    bool pop(T& item) {
        if (is_empty()) {
            return false; // Buffer underflow protection
        }

        item = buffer_[head_];
        head_ = (head_ + 1) % Capacity;
        count_--;
        return true;
    }

    // Peek at the next item without removing it

    bool peek(T& item) const
    {
        if (is_empty()) {
            return false;
        }

        item = buffer_[head_];
        return true;
    }

};

#endif /* COMMON_LIBR_FIFO_FIFO_H_ */
