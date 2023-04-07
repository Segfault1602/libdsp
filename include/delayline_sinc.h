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
        memset(inputs_, 0, sizeof(inputs_));
        if (delay > MAX_DELAY)
        {
            delay = MAX_DELAY;
        }

        in_point_ = 0;
        this->SetDelay(delay);

        filter_scale_ = 1.f;
        filter_step_ = SAMPLES_PER_CROSSING * filter_scale_;
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

        // Increment output pointer modulo length.
        if (++out_point_ == MAX_DELAY)
            out_point_ = 0;

        constexpr size_t filter_length = sizeof(sinc_table) / sizeof(sinc_table[0]);

        // Compute left wing
        float left = 0.0;
        float filter_offset = filter_step_ * alpha_;

        size_t coeff_count = static_cast<size_t>((filter_length - filter_offset) / filter_step_) - 1;
        size_t num_element = (out_point_ > in_point_) ? (MAX_DELAY - out_point_ + in_point_) : (in_point_ - out_point_);
        assert(num_element > coeff_count);

        float filter_index = filter_offset + coeff_count * filter_step_;
        int32_t data_index = static_cast<int32_t>(out_point_) - coeff_count;
        if (data_index < 0)
        {
            data_index += MAX_DELAY;
        }

        while (filter_index >= 0)
        {
            assert(data_index != in_point_);

            size_t filter_idx_int = static_cast<size_t>(filter_index);
            float fraction = filter_index - filter_idx_int;

            float weight = sinc_table[filter_idx_int] + fraction * (sinc_table[filter_idx_int + 1] - sinc_table[filter_idx_int]);
            left += inputs_[data_index] * weight;

            filter_index -= filter_step_;
            data_index += 1;
            if (data_index == MAX_DELAY)
            {
                data_index = 0;
            }
        }

        // Compute right wing
        float right = 0.0;
        filter_offset = filter_step_ * om_alpha_;
        filter_index = filter_offset + coeff_count * filter_step_;
        data_index = out_point_ + coeff_count;
        if (data_index >= MAX_DELAY)
        {
            data_index -= MAX_DELAY;
        }

        do
        {
            assert(data_index != in_point_);

            size_t filter_idx_int = static_cast<size_t>(filter_index);
            float fraction = filter_index - filter_idx_int;

            float weight =
                sinc_table[filter_idx_int] + fraction * (sinc_table[filter_idx_int + 1] - sinc_table[filter_idx_int]);
            right += inputs_[data_index] * weight;

            filter_index -= filter_step_;
            data_index -= 1;
            if (data_index < 0)
            {
                data_index = MAX_DELAY - 1;
            }

        } while (filter_index > 0);

        return left + right;
    }

  protected:
    unsigned long in_point_ = 0;
    unsigned long out_point_ = 0;
    float delay_ = 0.f;
    float alpha_ = 0.f;
    float om_alpha_ = 0.f;

    float inputs_[MAX_DELAY];
    float last_frame_ = 0;

    float filter_scale_ = 1.f;
    float filter_step_ = SAMPLES_PER_CROSSING;
};
} // namespace dsp