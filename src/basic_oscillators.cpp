#include "basic_oscillators.h"

#include <cmath>

#include "dsp_base.h"
#include "sin_table.h"

namespace dsp
{

float Sine(float phase)
{
    phase = std::fmodf(phase, 1.f);
    float idx = phase * SIN_LUT_SIZE;
    int idx0 = static_cast<int>(idx);
    float frac = idx - idx0;
    return sin_lut[idx0] + frac * (sin_lut[idx0 + 1] - sin_lut[idx0]);
}

float Tri(float phase)
{
    if (phase < 0.5f)
    {
        return 2.f * phase - 1.f;
    }
    else
    {
        return 2.f - 2.f * phase - 1.f;
    }
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
    constexpr float n = 1.f / RAND_MAX;
    return 2.f * rand() * n - 1.f;
}

} // namespace dsp