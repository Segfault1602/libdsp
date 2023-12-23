#include "vector_phaseshaper.h"

#include "basic_oscillators.h"
#include "dsp_utils.h"

namespace
{
float phase_distort(float x, float d, float v)
{
    if (x < d)
    {
        return (v * x / d);
    }

    return (1 - v) * ((x - d) / (1 - d)) + v;
}
} // namespace

namespace sfdsp
{

void VectorPhaseshaper::Init(float samplerate)
{
    samplerate_ = samplerate;
    phaseIncrement_ = freq_ / samplerate_;
}

void VectorPhaseshaper::SetFreq(float freq)
{
    freq_ = freq;
    phaseIncrement_ = freq_ / samplerate_;
}

void VectorPhaseshaper::SetMod(float d, float v)
{
    d_ = d;
    v_ = v;
}

void VectorPhaseshaper::ProcessBlock(float* out, size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        phase_ += phaseIncrement_;
        phase_ = fast_mod1(phase_);

        float vps = phase_distort(phase_, d_, v_);
        out[i] = -1 * Cosine(vps);
    }
}
} // namespace sfdsp