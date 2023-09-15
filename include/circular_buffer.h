// =============================================================================
// circular_buffer.h -- Simple circular buffer
// =============================================================================
#pragma once

#include <array>

#include "dsp_base.h"

namespace dsp
{
/// @brief Simple circular buffer.
/// @tparam CONTAINER_SIZE
template <size_t CONTAINER_SIZE>
class CircularBuffer
{
  public:
    /// @brief Constructor. Initialize a circular buffer with empty values.
    CircularBuffer() = default;
    ~CircularBuffer() = default;

    /// @brief Reset the buffer by filling it with zeros.
    void Reset();

    /// @brief Returns if the buffer is empty.
    /// @return True if the buffer is empty, false otherwise.
    bool IsEmpty() const;

    /// @brief Returns if the buffer is full.
    /// @return True if the buffer is full, false otherwise.
    bool IsFull() const;

    /// @brief  Returns the number of elements in the buffer.
    /// @return The number of elements in the buffer.
    size_t Count() const;

    /// @brief The size of the buffer. This is the maximum number of elements that the buffer can hold.
    /// @return The size of the buffer.
    size_t Size() const;

    /// @brief  Fill the buffer with a value.
    /// @param value The value to fill the buffer with.
    void Fill(float value);

    /// @brief Return the oldest value from the buffer and remove it.
    /// @return The oldest value from the buffer.
    float Pop();

    /// @brief Return the oldest value from the buffer without removing it.
    /// @return The oldest value from the buffer.
    float Peek() const;

    /// @brief  Write a value to the buffer.
    /// @param in The value to write to the buffer.
    /// @note If the buffer is full, the oldest value will be overwritten.
    void Write(float in);

    /// @brief Read a value from the buffer.
    /// @param idx The index of the value to read.
    /// @return the value at the given index.
    const float& operator[](size_t idx) const;

  private:
    std::array<float, CONTAINER_SIZE> buffer_ = {0};
    size_t count_ = 0;
    size_t write_ = 0;
    size_t read_ = 0;
};

template <size_t CONTAINER_SIZE>
void CircularBuffer<CONTAINER_SIZE>::Reset()
{
    Fill(0.f);
}

template <size_t CONTAINER_SIZE>
bool CircularBuffer<CONTAINER_SIZE>::IsEmpty() const
{
    return count_ == 0;
}

template <size_t CONTAINER_SIZE>
bool CircularBuffer<CONTAINER_SIZE>::IsFull() const
{
    return count_ == CONTAINER_SIZE;
}

template <size_t CONTAINER_SIZE>
size_t CircularBuffer<CONTAINER_SIZE>::Count() const
{
    return count_;
}

template <size_t CONTAINER_SIZE>
size_t CircularBuffer<CONTAINER_SIZE>::Size() const
{
    return CONTAINER_SIZE;
}

template <size_t CONTAINER_SIZE>
void CircularBuffer<CONTAINER_SIZE>::Fill(float value)
{
    buffer_.fill(value);
    count_ = buffer_.size();
}

template <size_t CONTAINER_SIZE>
float CircularBuffer<CONTAINER_SIZE>::Pop()
{
    float val = buffer_[read_];
    read_ = (read_ + 1) % CONTAINER_SIZE;
    --count_;

    return val;
}

template <size_t CONTAINER_SIZE>
float CircularBuffer<CONTAINER_SIZE>::Peek() const
{
    return buffer_[read_];
}

template <size_t CONTAINER_SIZE>
void CircularBuffer<CONTAINER_SIZE>::Write(float in)
{
    buffer_[write_] = in;
    write_ = (write_ + 1) % CONTAINER_SIZE;
    ++count_;

    // Check if we overwrote the oldest data
    if (count_ > CONTAINER_SIZE)
    {
        read_ = (read_ + 1) % CONTAINER_SIZE;
        count_ = CONTAINER_SIZE;
    }
}

template <size_t CONTAINER_SIZE>
const float& CircularBuffer<CONTAINER_SIZE>::operator[](size_t idx) const
{
    auto real_idx = (read_ + idx) % CONTAINER_SIZE;
    return buffer_[real_idx];
}
} // namespace dsp