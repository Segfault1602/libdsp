#include "basic_oscillators.h"

#include <cassert>
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

float Sine(float phase, float phase_increment, float* out, size_t size)
{
    size_t write_ptr = 0;
    size_t block_count = size / 4;
    for (size_t i = 0; i < block_count; ++i)
    {
        float phase1 = phase;
        phase += phase_increment;
        float phase2 = phase;
        phase += phase_increment;
        float phase3 = phase;
        phase += phase_increment;
        float phase4 = phase;
        phase += phase_increment;

        float idx1 = phase1 * SIN_LUT_SIZE;
        float idx2 = phase2 * SIN_LUT_SIZE;
        float idx3 = phase3 * SIN_LUT_SIZE;
        float idx4 = phase4 * SIN_LUT_SIZE;

        static_assert(SIN_LUT_SIZE == 0x1FF + 1);
        auto idxint_1 = static_cast<size_t>(idx1) & 0x1FF;
        auto idxint_2 = static_cast<size_t>(idx2) & 0x1FF;
        auto idxint_3 = static_cast<size_t>(idx3) & 0x1FF;
        auto idxint_4 = static_cast<size_t>(idx4) & 0x1FF;

        auto frac_1 = idx1 - static_cast<size_t>(idx1);
        auto frac_2 = idx2 - static_cast<size_t>(idx2);
        auto frac_3 = idx3 - static_cast<size_t>(idx3);
        auto frac_4 = idx4 - static_cast<size_t>(idx4);

        out[write_ptr] = sin_lut[idxint_1] + frac_1 * (sin_lut[idxint_1 + 1] - sin_lut[idxint_1]);
        out[write_ptr + 1] = sin_lut[idxint_2] + frac_2 * (sin_lut[idxint_2 + 1] - sin_lut[idxint_2]);
        out[write_ptr + 2] = sin_lut[idxint_3] + frac_3 * (sin_lut[idxint_3 + 1] - sin_lut[idxint_3]);
        out[write_ptr + 3] = sin_lut[idxint_4] + frac_4 * (sin_lut[idxint_4 + 1] - sin_lut[idxint_4]);

        write_ptr += 4;
    }

    // if size is not a multiple of 4, process the remaining samples
    block_count = size % 4;
    for (size_t i = 0; i < block_count; ++i)
    {
        out[write_ptr] = Sine(phase);
        phase += phase_increment;
        write_ptr += 1;
    }

    return phase;
}

float Tri(float phase)
{
    phase = std::fmod(phase, 1.f);
    float t = -1.0f + (2.0f * phase);
    return 2.0f * (fabsf(t) - 0.5f);
}

float Tri(float phase, float phase_increment, float* out, size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        phase = std::fmod(phase, 1.f);
        const float t = -1.0f + (2.0f * phase);
        out[i] = 2.0f * (fabsf(t) - 0.5f);
        phase += phase_increment;
    }

    return phase;
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
        out = Square(phase_);
        break;
    default:
        assert(false);
        out = Sine(phase_);
        break;
    }

    phase_ += phase_increment_;
    phase_ = std::fmod(phase_, 1.f);

    return out;
}

void BasicOscillator::ProcessBlock(float* out, size_t size)
{
    switch (type_)
    {
    case OscillatorType::Sine:
        phase_ = Sine(phase_, phase_increment_, out, size);
        phase_ = std::fmod(phase_, 1.f);
        break;
    case OscillatorType::Tri:
        phase_ = Tri(phase_, phase_increment_, out, size);
        phase_ = std::fmod(phase_, 1.f);
        break;
    case OscillatorType::Saw:
        for (size_t i = 0; i < size; ++i)
        {
            out[i] = Saw(phase_);
            phase_ += phase_increment_;
            phase_ = std::fmod(phase_, 1.f);
        }
        break;
    case OscillatorType::Square:
        for (size_t i = 0; i < size; ++i)
        {
            out[i] = Square(phase_);
            phase_ += phase_increment_;
            phase_ = std::fmod(phase_, 1.f);
        }
        break;
    default:
        assert(false);
        phase_ = Sine(phase_, phase_increment_, out, size);
        break;
    }
}

} // namespace sfdsp