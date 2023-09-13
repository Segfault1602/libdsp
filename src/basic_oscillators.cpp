#include "basic_oscillators.h"

#include <cmath>

#include "dsp_base.h"

namespace dsp
{

float Sine(float phase)
{
    return std::sinf(phase * TWO_PI);
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

} // namespace dsp