// =============================================================================
// delayline_sinc.h
// Interpolated delay line usinc windowed sinc algorithm.
// =============================================================================

#include "circular_buffer.h"

namespace dsp
{
    template <size_t MAX_DELAY_SIZE>
    class DelayLineSinc
    {
public:
    DelayLineSinc(float delay)
    {
        SetDelay(delay_);
    }

    ~DelayLineSinc() = default;
    DelayLineSinc(const DelayLineSinc& d) = delete;

    void Reset()
    {
        buffer_.Reset();
    }

    void SetDelay(float delay)
    {
        delay_ = delay;

        delay_int_ = static_cast<size_t>(delay_);
        delay_frac_ = delay_ - delay_int_;
    }

    float Tick(float in)
    {
        buffer_.Write(in);
        return buffer_.Read();
    }

private:
    Buffer<MAX_DELAY_SIZE> buffer_;
    float delay_ = 0.f;
    size_t delay_int_ = 0;
    float delay_frac_ = 0.f;
    };
}