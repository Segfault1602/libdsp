// =============================================================================
// sinc_resampler.h -- Windowed sinc interpolation
//
// Based on the CCRMA implementation: https://ccrma.stanford.edu/~jos/resample/
// =============================================================================
#pragma once

#include <stdlib.h>

#include "circular_buffer.h"
#include "sinc_table.h"

namespace dsp
{
class SincResampler
{
  public:
    SincResampler(float ratio = 1);
    ~SincResampler() = default;

    void SetRatio(float ratio)
    {
        ratio_ = ratio;
        time_step_ = 1.f / ratio_;
        filter_scale_ = 1;//std::min(1.f, ratio_);
    }

    size_t BaseDelayInSamples() const
    {
        return right_history_.Size();
    }

    void Process(const float in, float *out, size_t& out_size)
    {
        float current_sample = right_history_.Read();
        right_history_.Write(in);
        left_history_.Write(current_sample);

        size_t filter_step = SAMPLES_PER_CROSSING * filter_scale_;

        size_t out_idx = 0;

        while (time_register_ < 1)
        {
            float sum = 0;

            float frac = time_register_;
            frac *= filter_scale_;

            // Compute left wing
            float filter_idx_frac = SAMPLES_PER_CROSSING * frac;
            size_t filter_offset = static_cast<size_t>(filter_idx_frac);
            float eta = filter_idx_frac - filter_offset;
            for (size_t i = 0; i < left_history_.Count(); ++i)
            {
                size_t filter_idx = filter_offset + filter_step * i;
                float weight = sinc_table[filter_idx] + eta * (sinc_table[filter_idx+1] - sinc_table[filter_idx]);
                sum += left_history_[left_history_.Count() - i - 1] * weight;
            }

            // Compute right wing
            frac = filter_scale_ - frac;
            filter_idx_frac = SAMPLES_PER_CROSSING * frac;
            filter_offset = static_cast<size_t>(filter_idx_frac);
            eta = filter_idx_frac - filter_offset;
            for (size_t i = 0; i < right_history_.Count(); ++i)
            {
                size_t filter_idx = filter_offset + filter_step * i;
                float weight = sinc_table[filter_idx] + eta * (sinc_table[filter_idx+1] - sinc_table[filter_idx]);
                sum += right_history_[i] * weight;
            }

            if (out_idx < out_size)
            {
                out[out_idx++] = sum;
            }

            time_register_ += time_step_;
        }

        time_register_ -= 1;
        out_size = out_idx;
    }

  private:
    Buffer<SINC_ZERO_COUNT> left_history_;
    Buffer<SINC_ZERO_COUNT> right_history_;

    float ratio_ = 1;
    float time_step_ = 1.f / ratio_;
    float filter_scale_ = 1.f;
    float time_register_ = 0.f;
};
} // namespace dsp