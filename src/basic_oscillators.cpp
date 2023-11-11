#include "basic_oscillators.h"

#include <cmath>

#include "dsp_base.h"
#include "sin_table.h"

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
    constexpr float kOneOverRandMax = 1.f / static_cast<float>(RAND_MAX);
    return 2.f * static_cast<float>(rand()) * kOneOverRandMax - 1.f;
}

} // namespace sfdsp