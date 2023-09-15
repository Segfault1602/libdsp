// =============================================================================
// sinc_resampler.h -- Windowed sinc interpolation
//
// Based on the CCRMA implementation: https://ccrma.stanford.edu/~jos/resample/
// =============================================================================
#pragma once

#include <stdlib.h>

namespace dsp
{
/// @brief Resamples the input signal by the given ratio using a windowed sinc filter
/// @param in Input signal
/// @param input_size Size of the input signal
/// @param ratio Ratio to resample by
/// @param out Output signal
/// @param[in, out] out_size Size of the output signal
void sinc_resample(const float* in, size_t input_size, float ratio, float* out, size_t& out_size);
} // namespace dsp