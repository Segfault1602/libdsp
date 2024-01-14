#include "basic_oscillators.h"

#include <cassert>
#include <cmath>
#include <cstdlib>

#include "dsp_utils.h"
#include "sin_table.h"

#define MOD1(x) (x - std::floor(x))

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

    phase = MOD1(phase);
    float idx = phase * SIN_LUT_SIZE;
    auto idx0 = static_cast<size_t>(idx);
    auto frac = idx - static_cast<float>(idx0);
    return sin_lut[idx0] + frac * (sin_lut[idx0 + 1] - sin_lut[idx0]);
}

void Sine(const float* phases, float* out, size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        float idx1 = phases[i] * SIN_LUT_SIZE;

        static_assert(SIN_LUT_SIZE == 0x1FF + 1);
        auto idxint_1 = static_cast<size_t>(idx1) & 0x1FF;

        auto frac_1 = idx1 - static_cast<size_t>(idx1);
        out[i] = sin_lut[idxint_1] + frac_1 * (sin_lut[idxint_1 + 1] - sin_lut[idxint_1]);
    }
}

float Tri(float phase)
{
    phase = MOD1(phase);
    float t = -1.0f + (2.0f * phase);
    return 2.0f * (fabsf(t) - 0.5f);
}

float Tri(float phase, float phase_increment, float* out, size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        const float t1 = 2.0f * phase - 1.f;
        out[i] = 2.0f * fabsf(t1) - 1.f;

        phase += phase_increment;
        phase = MOD1(phase);
    }

    return phase;
}

float Saw(float phase)
{
    return 2.f * phase - 1.f;
}

float Saw(float phase, float phase_increment, float* out, size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        out[i] = 2.f * phase - 1.f;
        phase += phase_increment;
        phase = MOD1(phase);
    }

    return phase;
}

float Square(float phase, float duty)
{
    return (phase > duty) ? -1.f : 1.f;
}

float Square(float phase, float phase_increment, float* out, size_t size, float duty = 0.5f)
{
    for (size_t i = 0; i < size; ++i)
    {
        out[i] = (phase > duty) ? -1.f : 1.f;
        phase += phase_increment;
        phase = MOD1(phase);
    }

    return phase;
}

float Noise()
{
    return Fast_RandFloat();
}

void BasicOscillator::Init(float samplerate, float freq, OscillatorType type)
{
    samplerate_ = samplerate;
    SetFrequency(freq);
    SetType(type);
}

void BasicOscillator::SetType(OscillatorType type)
{
    type_ = type;
}

OscillatorType BasicOscillator::GetType() const
{
    return type_;
}

void BasicOscillator::SetDuty(float duty)
{
    duty_ = duty;
}

void BasicOscillator::SetFrequency(float frequency)
{
    frequency_ = frequency;
    phase_increment_ = frequency_ / samplerate_;
}

float BasicOscillator::GetFrequency() const
{
    return frequency_;
}

void BasicOscillator::SetPhase(float phase)
{
    phase_ = phase;
}

float BasicOscillator::GetPhase() const
{
    return phase_;
}

float BasicOscillator::Tick()
{
    float out = 0.f;
    switch (type_)
    {
    case OscillatorType::Sine:
        out = Sine(phase_);
        break;
    case OscillatorType::Tri:
        out = Tri(phase_);
        break;
    case OscillatorType::Saw:
        out = Saw(phase_);
        break;
    case OscillatorType::Square:
        out = Square(phase_, duty_);
        break;
    default:
        assert(false);
        out = Sine(phase_);
        break;
    }

    phase_ += phase_increment_;
    phase_ = MOD1(phase_);

    return out;
}

void BasicOscillator::ProcessBlock(float* out, size_t size)
{
    switch (type_)
    {
    case OscillatorType::Sine:
        // prefill the buffer with the current phase
        for (size_t i = 0; i < size; ++i)
        {
            out[i] = phase_;
            phase_ += phase_increment_;
        }
        Sine(out, out, size);
        phase_ = MOD1(phase_);
        break;
    case OscillatorType::Tri:
        phase_ = Tri(phase_, phase_increment_, out, size);
        phase_ = MOD1(phase_);
        break;
    case OscillatorType::Saw:
        phase_ = Saw(phase_, phase_increment_, out, size);
        phase_ = MOD1(phase_);
        break;
    case OscillatorType::Square:
        phase_ = Square(phase_, phase_increment_, out, size, duty_);
        phase_ = MOD1(phase_);
        break;
    default:
        assert(false);
        break;
    }
}

} // namespace sfdsp