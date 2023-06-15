// =============================================================================
// delayline_sinc.h
// Interpolated delay line usinc windowed sinc algorithm.
// Heavily based on STK's Delay class (https://github.com/thestk/stk/blob/master/include/Delay.h)
// =============================================================================
#pragma once

#include "circular_buffer.h"
#include "sinc_table.h"

#include <cassert>

constexpr size_t MIN_DELAY = SINC_ZERO_COUNT;

namespace dsp
{
/// @brief Implements a sinc delay line with a maximum delay of MAX_DELAY
/// @tparam MAX_DELAY The maximum delay in samples
template <size_t MAX_DELAY>
class DelaySinc
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
        this->SetDelay(delay, true);

        filter_scale_ = 1.f;
        filter_step_ = SAMPLES_PER_CROSSING * filter_scale_;
    }

    ~DelaySinc() = default;

    unsigned long MaxDelay(void)
    {
        return MAX_DELAY - 1;
    }

    void SetDelay(DspFloat delay, bool resetFilterScale)
    {
        if (delay > MAX_DELAY)
        {
            delay = MAX_DELAY;
        }

        if (delay < MIN_DELAY)
        {
            delay = static_cast<DspFloat>(MIN_DELAY);
        }

        DspFloat time_step = 1 - std::abs(delay - delay_);

        DspFloat outPointer = in_point_ - delay; // read chases write
        delay_ = delay;

        while (outPointer < 0)
            outPointer += MAX_DELAY;      // modulo maximum length

        out_point_ = (long)outPointer;    // integer part

        alpha_ = outPointer - out_point_; // fractional part
        om_alpha_ = 1.f - alpha_;

        if (out_point_ == MAX_DELAY)
            out_point_ = 0;

        if (!resetFilterScale)
        {
            DspFloat ratio = 1.f / time_step;

            filter_scale_ = (ratio < 1.0f) ? ratio : 1.0f;
            filter_step_ = SAMPLES_PER_CROSSING * filter_scale_;
        }
        else
        {
            filter_scale_ = 1.f;
            filter_step_ = SAMPLES_PER_CROSSING;
            last_out_ptr_ = outPointer - 1;
        }
    }

    unsigned long GetDelay(void) const
    {
        return delay_;
    }

    DspFloat TapOut(size_t delay)
    {
        int tap_ptr = in_point_ - delay;

        while (tap_ptr < 0)
        {
            tap_ptr += MAX_DELAY;
        }

        return inputs_[tap_ptr];
    }

    /// @brief Returns the last output sample
    /// @param input
    /// @return DspFloat
    DspFloat Tick(DspFloat input)
    {
        inputs_[in_point_++] = input;
        last_out_ptr_ = out_point_ + alpha_;

        // Increment input pointer modulo length.
        if (in_point_ == MAX_DELAY)
            in_point_ = 0;

        // Increment output pointer modulo length.
        if (++out_point_ == MAX_DELAY)
            out_point_ = 0;

        constexpr size_t filter_length = sizeof(sinc_table) / sizeof(sinc_table[0]);

        // Compute left wing
        DspFloat left = 0.0;
        DspFloat filter_offset = filter_step_ * alpha_;

        auto coeff_count = static_cast<uint8_t>((filter_length - filter_offset) / filter_step_) - 1;
        size_t num_element = (out_point_ > in_point_) ? (MAX_DELAY - out_point_ + in_point_) : (in_point_ - out_point_);
        assert(num_element > coeff_count);

        DspFloat filter_index = filter_offset + coeff_count * filter_step_;
        int32_t data_index = static_cast<int32_t>(out_point_) - coeff_count;
        if (data_index < 0)
        {
            data_index += MAX_DELAY;
        }

        while (filter_index >= 0)
        {
            assert(data_index != in_point_);

            size_t filter_idx_int = static_cast<size_t>(filter_index);
            DspFloat fraction = filter_index - filter_idx_int;

            DspFloat weight =
                sinc_table[filter_idx_int] + fraction * (sinc_table[filter_idx_int + 1] - sinc_table[filter_idx_int]);
            left += inputs_[data_index] * weight;

            filter_index -= filter_step_;
            data_index += 1;
            if (data_index == MAX_DELAY)
            {
                data_index = 0;
            }
        }

        // Compute right wing
        DspFloat right = 0.0;
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
            DspFloat fraction = filter_index - filter_idx_int;

            DspFloat weight =
                sinc_table[filter_idx_int] + fraction * (sinc_table[filter_idx_int + 1] - sinc_table[filter_idx_int]);
            right += inputs_[data_index] * weight;

            filter_index -= filter_step_;
            data_index -= 1;
            if (data_index < 0)
            {
                data_index = MAX_DELAY - 1;
            }

        } while (filter_index > 0);

        return (left + right) * filter_scale_;
    }

  protected:
    unsigned long in_point_ = 0;
    unsigned long out_point_ = 0;
    DspFloat last_out_ptr_ = 0.f;
    DspFloat delay_ = 0.f;
    DspFloat alpha_ = 0.f;
    DspFloat om_alpha_ = 0.f;

    DspFloat inputs_[MAX_DELAY];
    DspFloat last_frame_ = 0;

    DspFloat filter_scale_ = 1.f;
    DspFloat filter_step_ = SAMPLES_PER_CROSSING;
};
} // namespace dsp