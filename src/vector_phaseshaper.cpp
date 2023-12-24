#include "vector_phaseshaper.h"

#include <cassert>

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

void VectorPhaseshaper::SetFormantMode(FormantMode mode)
{
    formantMode_ = mode;
}

VectorPhaseshaper::FormantMode VectorPhaseshaper::GetFormantMode() const
{
    return formantMode_;
}

void VectorPhaseshaper::ProcessBlock(float* out, size_t size)
{
    if (formantMode_ == FormantMode::FREE || v_ <= 1.f)
    {
        for (size_t i = 0; i < size; ++i)
        {
            phase_ += phaseIncrement_;
            phase_ = fast_mod1(phase_);

            float vps = phase_distort(phase_, d_, v_);
            out[i] = -1 * Cosine(vps);
        }
    }
    else
    {
        assert(formantMode_ == FormantMode::RATIO);
        const float gain = formantMode_ == FormantMode::FREE ? 0.f : fast_mod1(2.f * v_ - 1.f);

        float v1 = formantMode_ == FormantMode::FREE ? v_ : ((2.f * v_) - gain) * 0.5f;

        for (size_t i = 0; i < size; ++i)
        {
            phase_ += phaseIncrement_;
            phase_ = fast_mod1(phase_);

            float vps = phase_distort(phase_, d_, v1);
            out[i] = -1 * Cosine(vps) * (1.f - gain);

            if (formantMode_ == FormantMode::RATIO)
            {
                float vps2 = phase_distort(phase_, d_, v1 + 0.5f);
                out[i] += -1 * Cosine(vps2) * gain;
            }
        }
    }
}
} // namespace sfdsp