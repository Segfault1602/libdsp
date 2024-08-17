// =============================================================================
// phaseshapers.cpp -- Phaseshaper implementations
//
// Original implementation by:
// J.Kleimola, V.Lazzarini, J.Timoney, V.Valimaki,
// "PHASESHAPING OSCILLATOR ALGORITHMS FOR MUSICAL SOUND SYNTHESIS",
// SMC 2010, Barcelona, Spain, July 21-24, 2010.
//
// Adapted from origianl python source by:
// Alex St-Onge
// =============================================================================

#include "phaseshapers.h"

#include <algorithm>
#include <cmath>
#include <cstdint>

#include "basic_oscillators.h"

#define G_B(x) (2 * (x)-1)
#define MODM(x, m) (x - m * std::floor(x / m))
#define MOD1(x) (x - std::floor(x))

// -- Linear transformations

inline float g_lin(float x, float a1 = 1, float a0 = 0)
{
    return a1 * x + a0;
}

inline float g_ramp(float x, float a1 = 1, float a0 = 0)
{
    return MOD1(g_lin(x, a1, a0));
}

inline float g_tri(float x, float a1 = 1, float a0 = 0)
{
    return MOD1(g_lin(std::abs(G_B(x)), a1, a0));
}

inline float g_pulse(float x, float width = 0.5f)
{
    return x - MOD1(x + 1.f - width) + (1.f - width);
}

inline float s_tri(float x)
{
    if (x < 0.5f)
    {
        return 2.f * x;
    }

    return 2.f - 2.f * x;
}

inline float s_vtri(float x, float phaseIncrement, float w = 0.5f, float P = 100)
{
    float d = std::ceil(w * P + 0.5f);
    float x1 = 2 * x - 1;
    float xd = 2 * MOD1(x + phaseIncrement * d) - 1;
    float xdiff = x1 * x1 - xd * xd;
    float at = 1.f / (8.f * (w - w * w));
    constexpr float bt = 0.5f;
    float s = g_lin(xdiff, at, bt);
    return s;
}

inline float g_vtri(float x, float phaseIncrement, float w = 0.5f, float a1 = 1.f, float a0 = 0.f, float P = 100)
{
    float vtri = s_vtri(x, phaseIncrement, w, P);
    return MOD1(g_lin(vtri, a1, a0));
}

inline float g_ripple(float x, float m = 1.f)
{
    // Orignal equation was 'x+MODM(x,m)', but I found that subtracting the modulo resulted in the same output but
    // without clipping.
    return G_B(x - MODM(x, m));
}

/// @brief
/// @param x Signal
/// @param T Phase increament
/// @param h Transition height (negative for falling transition)
/// @return
float polyBLEP(float x, float phase, float T, float h = -1.f)
{
    float s = 0.f;
    float p = phase;
    p -= floor(p); // phase [0,1)

    if (p > (1 - T))
    {
        float t = (p - 1.f);
        t /= T;
        float c = 0.5f * t * t + t + 0.5f;
        c *= h;
        s = x + c;
    }
    else if (p < T)
    {
        float t = p / T;
        float c = -0.5f * t * t + t - 0.5f;
        c *= h;
        s = x + c;
    }
    else
    {
        s = x;
    }

    return s;
}

namespace sfdsp
{

void Phaseshaper::Init(float sampleRate)
{
    samplerate_ = sampleRate;
    phase_ = 0.f;
    freq_ = 220.f;
    phaseIncrement_ = freq_ / samplerate_;
    period_ = samplerate_ / freq_;
}

void Phaseshaper::SetMod(float mod)
{
    mod = std::clamp(mod, 0.f, 1.f);
    mod_ = mod;
}

float Phaseshaper::Process()
{
    float wave1 = std::floor(waveform_);
    float out1 = ProcessWave(static_cast<Waveform>(wave1));

    float wave2 = std::ceil(waveform_);
    float out2 = 0.f;
    if (wave1 != wave2)
    {
        out2 = ProcessWave(static_cast<Waveform>(wave2));
    }

    float w1 = 1.f - (waveform_ - wave1);
    float w2 = 1.f - w1;

    phase_ += phaseIncrement_;
    phase_ = MOD1(phase_);

    return out1 * w1 + out2 * w2;
}

void Phaseshaper::ProcessBlock(float* out, size_t size)
{
    constexpr size_t kBlockSize = 64;
    float phase_buffer[kBlockSize];
    float w1_buffer[kBlockSize];
    float w2_buffer[kBlockSize];

    const float wave1 = std::floor(waveform_);
    const float wave2 = std::ceil(waveform_);

    const float gain1 = 1.f - (waveform_ - wave1);
    const float gain2 = 1.f - gain1;

    const size_t num_blocks = size / kBlockSize;
    size_t write_ptr = 0;
    for (size_t i = 0; i < num_blocks; ++i)
    {
        for (size_t j = 0; j < kBlockSize; ++j)
        {
            phase_buffer[j] = phase_;
            phase_ = MOD1(phase_ + phaseIncrement_);
        }

        ProcessWaveBlock(phase_buffer, w1_buffer, kBlockSize, static_cast<Waveform>(wave1), gain1);
        if (wave1 != wave2)
        {
            ProcessWaveBlock(phase_buffer, w2_buffer, kBlockSize, static_cast<Waveform>(wave2), gain2);
        }

        for (size_t j = 0; j < kBlockSize; ++j)
        {
            out[write_ptr] = w1_buffer[j] + w2_buffer[j];
            ++write_ptr;
        }
    }

    const size_t remaining = size - write_ptr;
    if (remaining > 0)
    {
        for (size_t j = 0; j < remaining; ++j)
        {
            phase_buffer[j] = phase_;
            phase_ = MOD1(phase_ + phaseIncrement_);
        }

        ProcessWaveBlock(phase_buffer, w1_buffer, remaining, static_cast<Waveform>(wave1), gain1);
        if (wave1 != wave2)
        {
            ProcessWaveBlock(phase_buffer, w2_buffer, remaining, static_cast<Waveform>(wave2), gain2);
        }
        else
        {
            memset(w2_buffer, 0, remaining * sizeof(float));
        }

        for (size_t j = 0; j < remaining; ++j)
        {
            out[write_ptr] = w1_buffer[j] + w2_buffer[j];
            ++write_ptr;
        }
    }
}

float Phaseshaper::ProcessWaveSlice() const
{
    // a1 vary from 0.25  to 0.40
    float a1 = 0.25f + (mod_ * 0.15f);
    float slicePhase = g_lin(phase_, a1);
    float trivial = G_B(sfdsp::Sine(slicePhase));

    float blep = polyBLEP(trivial, phase_, phaseIncrement_, -2);
    return blep;
}

void Phaseshaper::ProcessWaveSliceBlock(const float* p, float* out, size_t size, float gain) const
{
    // a1 vary from 0.25  to 0.40
    float a1 = 0.25f + (mod_ * 0.15f);

    for (size_t i = 0; i < size; ++i)
    {
        float slicePhase = g_lin(p[i], a1);
        float trivial = G_B(sfdsp::Sine(slicePhase));

        float blep = polyBLEP(trivial, p[i], phaseIncrement_, -2);
        out[i] = blep * gain;
    }
}

float Phaseshaper::ProcessHardSync() const
{
    // a1 vary from 2 to 3
    float a1 = 2.0f + mod_;
    return G_B(g_ramp(phase_, a1));
}

void Phaseshaper::ProcessHardSyncBlock(const float* p, float* out, size_t size, float gain) const
{
    // a1 vary from 2 to 3
    float a1 = 2.0f + mod_;

    for (size_t i = 0; i < size; ++i)
    {
        out[i] = G_B(g_ramp(p[i], a1)) * gain;
    }
}

float Phaseshaper::ProcessSoftSync() const
{
    // a1 vary from 1 to 1.5
    float a1 = 1.f + (mod_ * 0.5f);
    float softPhase = g_tri(phase_, a1);
    return G_B(s_tri(softPhase));
}

void Phaseshaper::ProcessSoftSyncBlock(const float* p, float* out, size_t size, float gain) const
{
    // a1 vary from 1 to 1.5
    float a1 = 1.f + (mod_ * 0.5f);

    for (size_t i = 0; i < size; ++i)
    {
        float softPhase = g_tri(p[i], a1);
        out[i] = G_B(s_tri(softPhase)) * gain;
    }
}

float Phaseshaper::ProcessTriMod() const
{
    // atm vary from 0.5 to 1.5
    float mod = 0.5f + mod_;
    float trimodPhase = mod * G_B(g_tri(phase_));
    return 2 * (trimodPhase - std::ceil(trimodPhase - 0.5f));
}

void Phaseshaper::ProcessTriModBlock(const float* p, float* out, size_t size, float gain) const
{
    // atm vary from 0.5 to 1.5
    float mod = 0.5f + mod_;

    for (size_t i = 0; i < size; ++i)
    {
        float trimodPhase = mod * G_B(g_tri(p[i]));
        out[i] = 2 * (trimodPhase - std::ceil(trimodPhase - 0.5f)) * gain;
    }
}

float Phaseshaper::ProcessSupersaw() const
{
    // m1 vary from 0.25 to 0.75
    const float m1 = 0.25f + (mod_ * 0.50f);
    const float m2 = 0.88f;
    const float a1 = 1.5f;
    float xs = g_lin(phase_, a1);

    float supersawPhase = MODM(xs, m1) + MODM(xs, m2);

    // Original equation was sin(supersawPhase) but since sfdsp::Sine expects a value between 0 and 1
    // we need to remove the implied 2pi factor.
    constexpr float one_over_2pi = 1.f / TWO_PI;
    return G_B(sfdsp::Sine(supersawPhase * one_over_2pi));
}

void Phaseshaper::ProcessSupersawBlock(const float* p, float* out, size_t size, float gain) const
{
    // m1 vary from 0.25 to 0.75
    const float m1 = 0.25f + (mod_ * 0.50f);
    const float m2 = 0.88f;
    const float a1 = 1.5f;

    for (size_t i = 0; i < size; ++i)
    {
        float xs = g_lin(p[i], a1);

        float supersawPhase = MODM(xs, m1) + MODM(xs, m2);

        // Original equation was sin(supersawPhase) but since sfdsp::Sine expects a value between 0 and 1
        // we need to remove the implied 2pi factor.
        constexpr float one_over_2pi = 1.f / TWO_PI;
        out[i] = G_B(sfdsp::Sine(supersawPhase * one_over_2pi)) * gain;
    }
}

float Phaseshaper::ProcessVarSlope(float phase) const
{
    // Width can vary from 0.1 to 0.5
    const float width = 0.1f + (mod_ * 0.5f);
    const float pulse = g_pulse(phase, width);
    float out = 0.5f * phase * (1.0f - pulse) / width + pulse * (phase - width) / (1.f - width);

    out = sfdsp::Sine(out);

    return out;
}

void Phaseshaper::ProcessVarSlopeBlock(const float* p, float* out, size_t size, float gain) const
{
    // Width can vary from 0.1 to 0.5
    const float width = 0.1f + (mod_ * 0.5f);

    for (size_t i = 0; i < size; ++i)
    {
        const float pulse = g_pulse(p[i], width);
        out[i] = 0.5f * p[i] * (1.0f - pulse) / width + pulse * (p[i] - width) / (1.f - width);
        out[i] = sfdsp::Sine(out[i]) * gain;
    }
}

float Phaseshaper::ProcessVarTri() const
{
    // a1 can vary from 1.25 to 1.75
    float a1 = 1.25f + (mod_ * 0.5f);
    float w3 = 0.50;
    float vtri = g_vtri(phase_, phaseIncrement_, w3, a1, 0, period_);
    return sfdsp::Sine(vtri);
}

void Phaseshaper::ProcessVarTriBlock(const float* p, float* out, size_t size, float gain) const
{
    // a1 can vary from 1.25 to 1.75
    float a1 = 1.25f + (mod_ * 0.5f);
    float w3 = 0.50;

    for (size_t i = 0; i < size; ++i)
    {
        float vtri = g_vtri(p[i], phaseIncrement_, w3, a1, 0, period_);
        out[i] = sfdsp::Sine(vtri) * gain;
    }
}

float Phaseshaper::ProcessRipple() const
{
    // ripples amount goes from no ripple at 0 to some ripples at 1.
    float ripple_amount = mod_ * 0.1f;
    return g_ripple(phase_, ripple_amount);
}

void Phaseshaper::ProcessRippleBlock(const float* p, float* out, size_t size, float gain) const
{
    // ripples amount goes from no ripple at 0 to some ripples at 1.
    float ripple_amount = mod_ * 0.1f;

    for (size_t i = 0; i < size; ++i)
    {
        out[i] = g_ripple(p[i], ripple_amount) * gain;
    }
}

float Phaseshaper::ProcessWave(Waveform wave)
{
    float out = 0.f;
    switch (wave)
    {
    case Waveform::VARIABLE_SLOPE:
        out = ProcessVarSlope(phase_);
        break;
    // case Waveform::VARIABLE_TRIANGLE:
    //     out = ProcessVarTri();
    //     break;
    case Waveform::SOFTSYNC:
        out = ProcessSoftSync();
        break;
    case Waveform::WAVESLICE:
        out = ProcessWaveSlice();
        break;
    case Waveform::SUPERSAW:
        out = ProcessSupersaw();
        break;
    // case Waveform::HARDSYNC:
    //     out = ProcessHardSync();
    //     break;
    case Waveform::TRIANGLE_MOD:
        out = ProcessTriMod();
        break;
    case Waveform::RIPPLE:
        out = ProcessRipple();
        break;
    default:
        break;
    }
    return out;
}

void Phaseshaper::ProcessWaveBlock(const float* p, float* out, size_t size, Waveform wave, float gain)
{
    switch (wave)
    {
    case Waveform::VARIABLE_SLOPE:
        ProcessVarSlopeBlock(p, out, size, gain);
        break;
    case Waveform::SOFTSYNC:
        ProcessSoftSyncBlock(p, out, size, gain);
        break;
    case Waveform::WAVESLICE:
        ProcessWaveSliceBlock(p, out, size, gain);
        break;
    case Waveform::SUPERSAW:
        ProcessSupersawBlock(p, out, size, gain);
        break;
    case Waveform::TRIANGLE_MOD:
        ProcessTriModBlock(p, out, size, gain);
        break;
    case Waveform::RIPPLE:
        ProcessRippleBlock(p, out, size, gain);
        break;
    default:
        memset(out, 0, size * sizeof(float));
        break;
    }
}

} // namespace sfdsp