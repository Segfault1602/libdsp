// =============================================================================
// delayline_sinc.h
// Interpolated delay line usinc windowed sinc algorithm.
// Heavily based on STK's Delay class (https://github.com/thestk/stk/blob/master/include/Delay.h)
// =============================================================================

#include "circular_buffer.h"
#include "sinc_table.h"

constexpr size_t MIN_DELAY = SINC_ZERO_COUNT;

namespace dsp
{
template <size_t MAX_DELAY> class DelaySinc
{
  public:
    DelaySinc(unsigned long delay = 0)
    {
        if (delay > MAX_DELAY)
        {
            delay = MAX_DELAY;
        }

        in_point_ = 0;
        this->SetDelay(delay);
        left_history_.Fill(0);
    }

    ~DelaySinc() = default;

    unsigned long MaxDelay(void)
    {
        return MAX_DELAY - 1;
    };

    void SetDelay(float delay)
    {
        if (delay > MAX_DELAY)
        {
            delay = MAX_DELAY;
        }

        if (delay < MIN_DELAY)
        {
            delay = static_cast<float>(MIN_DELAY);
        }

        float outPointer = in_point_ - delay; // read chases write
        delay_ = delay;

        while (outPointer < 0)
            outPointer += MAX_DELAY; // modulo maximum length

        out_point_ = (long)outPointer; // integer part

        alpha_ = outPointer - out_point_; // fractional part
        om_alpha_ = 1.f - alpha_;

        if (out_point_ == MAX_DELAY)
            out_point_ = 0;
    }

    unsigned long GetDelay(void) const
    {
        return delay_;
    };

    float Tick(float input)
    {
        inputs_[in_point_++] = input;

        // Increment input pointer modulo length.
        if (in_point_ == MAX_DELAY)
            in_point_ = 0;

        left_history_.Write(inputs_[out_point_]);
        // Increment output pointer modulo length.
        if (++out_point_ == MAX_DELAY)
            out_point_ = 0;

        last_frame_ = 0.f;

        // Compute left wing
        float filter_idx_frac = SAMPLES_PER_CROSSING * alpha_;
        size_t filter_offset = static_cast<size_t>(filter_idx_frac);
        float eta = filter_idx_frac - filter_offset;
        for (size_t i = 0; i < left_history_.Count(); ++i)
        {
            size_t filter_idx = filter_offset + SAMPLES_PER_CROSSING * i;
            float weight = sinc_table[filter_idx] + eta * (sinc_table[filter_idx + 1] - sinc_table[filter_idx]);
            last_frame_ += left_history_[left_history_.Count() - i - 1] * weight;
        }

        // Compute right wing
        filter_idx_frac = SAMPLES_PER_CROSSING * om_alpha_;
        filter_offset = static_cast<size_t>(filter_idx_frac);
        eta = filter_idx_frac - filter_offset;
        size_t tmp_out_ptr = out_point_;
        for (size_t i = 0; i < MIN_DELAY; ++i)
        {
            size_t filter_idx = filter_offset + SAMPLES_PER_CROSSING * i;
            float weight = sinc_table[filter_idx] + eta * (sinc_table[filter_idx + 1] - sinc_table[filter_idx]);
            last_frame_ += inputs_[tmp_out_ptr] * weight;

            if (++tmp_out_ptr == MAX_DELAY)
                tmp_out_ptr = 0;
        }

        return last_frame_;
    }

  protected:
    unsigned long in_point_ = 0;
    unsigned long out_point_ = 0;
    float delay_ = 0.f;
    float alpha_ = 0.f;
    float om_alpha_ = 0.f;

    float inputs_[MAX_DELAY];
    float last_frame_ = 0;

    Buffer<SINC_ZERO_COUNT> left_history_;
};
} // namespace dsp