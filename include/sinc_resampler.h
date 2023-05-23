// =============================================================================
// sinc_resampler.h -- Windowed sinc interpolation
//
// Based on the CCRMA implementation: https://ccrma.stanford.edu/~jos/resample/
// =============================================================================
#pragma once

#include <stdlib.h>

#include "dsp_base.h"
#include "circular_buffer.h"
#include "sinc_table.h"

namespace dsp
{
void sinc_resample(const DspFloat* in, size_t input_size, DspFloat ratio, DspFloat* out, size_t& out_size)
{
    constexpr size_t filter_length = sizeof(sinc_table) / sizeof(sinc_table[0]);
    DspFloat time_step = 1.0 / ratio;
    DspFloat filter_scale = (ratio < 1.0) ? ratio : 1.0;
    DspFloat filter_step = SAMPLES_PER_CROSSING * filter_scale;

    size_t out_idx = 0;
    DspFloat t = 0.0;
    while (t < input_size)
    {

        size_t idx_integer = static_cast<size_t>(t);
        DspFloat fractional_part = t - idx_integer;

        // Left wing
        DspFloat left = 0.0;
        DspFloat filter_offset = filter_step * fractional_part;

        size_t left_coeff_count = static_cast<size_t>((filter_length - filter_offset) / filter_step);
        left_coeff_count = std::min(idx_integer, left_coeff_count);
        for (int32_t i = left_coeff_count; i >= 0; --i)
        {
            DspFloat filter_idx = filter_offset + filter_step * i;
            size_t filter_idx_int = static_cast<size_t>(filter_idx);
            DspFloat fraction = filter_idx - filter_idx_int;

            DspFloat weight =
                sinc_table[filter_idx_int] + fraction * (sinc_table[filter_idx_int + 1] - sinc_table[filter_idx_int]);
            left += in[idx_integer - i] * weight;
        }

        // Right wing
        DspFloat right = 0.0;
        fractional_part = 1 - fractional_part;
        filter_offset = filter_step * fractional_part;

        size_t right_coeff_count = static_cast<size_t>((filter_length - filter_offset) / filter_step);
        right_coeff_count = std::min(input_size - idx_integer - 1, right_coeff_count);
        for (size_t i = 0; i < right_coeff_count; ++i)
        {
            DspFloat filter_idx = filter_offset + filter_step * i;
            size_t filter_idx_int = static_cast<size_t>(filter_idx);
            DspFloat fraction = filter_idx - filter_idx_int;

            DspFloat weight =
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