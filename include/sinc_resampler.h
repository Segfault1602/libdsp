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
void sinc_resample(const float* in, size_t input_size, float ratio, float* out, size_t& out_size)
{
    constexpr size_t filter_length = sizeof(sinc_table) / sizeof(sinc_table[0]);
    float time_step = 1.0 / ratio;
    float filter_scale = (ratio < 1.0) ? ratio : 1.0;
    float filter_step = SAMPLES_PER_CROSSING * filter_scale;

    size_t out_idx = 0;
    float t = 0.0;
    while (t < input_size)
    {

        size_t idx_integer = static_cast<size_t>(t);
        float fractional_part = t - idx_integer;

        // Left wing
        float left = 0.0;
        float filter_offset = filter_step * fractional_part;

        size_t left_coeff_count = static_cast<size_t>((filter_length - filter_offset) / filter_step);
        left_coeff_count = std::min(idx_integer, left_coeff_count);
        for (int32_t i = left_coeff_count; i >= 0; --i)
        {
            float filter_idx = filter_offset + filter_step * i;
            size_t filter_idx_int = static_cast<size_t>(filter_idx);
            float fraction = filter_idx - filter_idx_int;

            float weight =
                sinc_table[filter_idx_int] + fraction * (sinc_table[filter_idx_int + 1] - sinc_table[filter_idx_int]);
            left += in[idx_integer - i] * weight;
        }

        // Right wing
        float right = 0.0;
        fractional_part = 1 - fractional_part;
        filter_offset = filter_step * fractional_part;

        size_t right_coeff_count = static_cast<size_t>((filter_length - filter_offset) / filter_step);
        right_coeff_count = std::min(input_size - idx_integer - 1, right_coeff_count);
        for (size_t i = 0; i < right_coeff_count; ++i)
        {
            float filter_idx = filter_offset + filter_step * i;
            size_t filter_idx_int = static_cast<size_t>(filter_idx);
            float fraction = filter_idx - filter_idx_int;

            float weight =
                sinc_table[filter_idx_int] + fraction * (sinc_table[filter_idx_int + 1] - sinc_table[filter_idx_int]);
            right += in[idx_integer + 1 + i] * weight;
        }

        if (out_idx < out_size)
        {
            out[out_idx] = (left + right) * filter_scale;
            ++out_idx;
        }

        t += time_step;
    }

    out_size = out_idx;
}
} // namespace dsp