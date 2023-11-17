#include "basic_oscillators.h"

#include <cmath>
#include <cstdlib>

#include "dsp_utils.h"
#include "sin_table.h"

namespace
{

/// @brief Fast random number generator from https://www.musicdsp.org/en/latest/Other/273-fast-float-random-numbers.html
/// @return
float Fast_RandFloat()
{
    static int gRandSeed = 1.f;
    gRandSeed *= 16807;
    return (float)gRandSeed * 4.6566129e-010f;
}
} // namespace

namespace sfdsp
{

float Sine(float phase)
{
    while (phase < 0.f)
    {
        phase += 1.f;
    }
    phase = std::fmod(phase, 1.f);
    float idx = phase * SIN_LUT_SIZE;
    auto idx0 = static_cast<size_t>(idx);
    auto frac = idx - static_cast<float>(idx0);
    return sin_lut[idx0] + frac * (sin_lut[idx0 + 1] - sin_lut[idx0]);
}

float Tri(float phase)
{
    phase = std::fmod(phase, 1.f);
    if (phase < 0.5f)
    {
        return 2.f * phase - 1.f;
    }

    return 2.f - 2.f * phase - 1.f;
}

float Saw(float phase)
{
    return 2.f * phase - 1.f;
}

float Square(float phase)
{
    return (phase < 0.5f) ? -1.f : 1.f;
}

float Noise()
{
    return Fast_RandFloat();
}

} // namespace sfdsp