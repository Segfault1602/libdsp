// =============================================================================
// sinc_resampler.cpp -- Windowed sinc interpolation
//
// Based on the CCRMA implementation: https://ccrma.stanford.edu/~jos/resample/
// =============================================================================

#include "sinc_resampler.h"

namespace dsp
{
    SincResampler::SincResampler(float ratio)
    {
        SetRatio(ratio);
        left_history_.Fill(0);
        right_history_.Fill(0);
    }
}
