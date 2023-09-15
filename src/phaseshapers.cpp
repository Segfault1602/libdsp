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
#include <stdint.h>

#define G_B(x) (2 * x - 1)

// -- Linear transformations

inline float g_lin(float x, float a1 = 1, float a0 = 0)
{
    return a1 * x + a0;
}

inline float g_ramp(float x, float a1 = 1, float a0 = 0)
{
    return std::fmod(g_lin(x, a1, a0), 1.f);
}

inline float g_tri(float x, float a1 = 1, float a0 = 0)
{
    return std::fmod(g_lin(std::abs(G_B(x)), a1, a0), 1.f);
}

inline float g_pulse(float x, float phaseIncrement, float width = 0.5f, float period = 100.f)
{
    float d = width * period;
    return x - std::fmod(x + phaseIncrement * d, 1.f) + width;
}

inline float g_pulse_trivial(float x, float width = 0.5f)
{
    return (x < width) ? 0.f : 1.f;
}

inline float s_tri(float x)
{
    if (x < 0.5f)
    {
        return 2.f * x;
    }
    else
    {
        return 2.f - 2.f * x;
    }
}

float polyBLEP(float phase, float phaseIncrement, float h = -1.f)
{
    float out = 0.f;
    float p = phase;
    p -= floorf(p);

    if (p > (1 - phaseIncrement))
    {
        float t = (p - 1) / phaseIncrement;
        float c = 0.5f * t * t + t * 0.5f;
        c *= h;
        out = c;
    }
    else if (p < phaseIncrement)
    {
        float t = p / phaseIncrement;
        float c = -0.5f * t * t + t - 0.5f;
        c *= h;
        out = c;
    }

    return out;
}

namespace dsp
{

void Phaseshaper::Init(float sampleRate)
{
    m_sampleRate = sampleRate;
    m_phase = 0.f;
    m_freq = 220.f;
    m_phaseIncrement = m_freq / m_sampleRate;
    m_period = m_sampleRate / m_freq;
}

void Phaseshaper::SetMod(float mod)
{
    mod = std::clamp(mod, 0.f, 1.f);
    m_mod = mod;
}

float Phaseshaper::Process()
{
    float wave1 = std::floor(m_waveform);
    float out1 = ProcessWave(static_cast<Waveform>(wave1));

    float wave2 = std::ceil(m_waveform);
    float out2 = ProcessWave(static_cast<Waveform>(wave2));

    float w1 = 1.f - (m_waveform - wave1);
    float w2 = 1.f - w1;

    m_phase += m_phaseIncrement;
    m_phase = std::fmod(m_phase, 1.f);

    return out1 * w1 + out2 * w2;
}

float Phaseshaper::ProcessWaveSlice() const
{
    float a1 = 0.25f + (1.f + m_mod) * 0.10f;
    float slicePhase = g_lin(m_phase, a1);
    float trivial = G_B(std::sin(TWO_PI * slicePhase));

    float blep = polyBLEP(m_phase, m_phaseIncrement, -2);
    return trivial + blep;
}

float Phaseshaper::ProcessHardSync() const
{
    float a1 = 2.5f + m_mod;
    return G_B(g_ramp(m_phase, a1));
}

float Phaseshaper::ProcessSoftSync() const
{
    float a1 = 1.25f + m_mod;
    float softPhase = g_tri(m_phase, a1);
    return G_B(s_tri(softPhase));
}

float Phaseshaper::ProcessTriMod() const
{
    const float atm = 0.82f; // Roland JP-8000 triangle modulation offset parameter
    float mod = atm + m_mod * 0.15f;
    float trimodPhase = mod * G_B(g_tri(m_phase));
    return 2 * (trimodPhase - std::ceil(trimodPhase - 0.5f));
}

float Phaseshaper::ProcessSupersaw() const
{
    const float m1 = 0.5f + (m_mod * 0.25f);
    const float m2 = 0.88f;
    const float a1 = 1.5f;
    float xs = g_lin(m_phase, a1);

    float supersawPhase = std::fmod(xs, m1) + std::fmod(xs, m2);
    return G_B(std::sin(supersawPhase));
}

float Phaseshaper::ProcessVarSlope() const
{
    float width = 0.5f + m_mod * 0.25f;

    float pulse = g_pulse(m_phase, m_phaseIncrement, width, m_period);
    float vslope = 0.5f * m_phase * (1.0f - pulse) / width + pulse * (m_phase - width) / (1 - width);
    return std::sin(TWO_PI * vslope);
}

float Phaseshaper::ProcessWave(Waveform wave) const
{
    float out = 0.f;
    switch (wave)
    {
    case Waveform::VARIABLE_SLOPE:
        out = ProcessVarSlope();
        break;
    case Waveform::SOFTSYNC:
        out = ProcessSoftSync();
        break;
    case Waveform::WAVESLICE:
        out = ProcessWaveSlice();
        break;
    case Waveform::SUPERSAW:
        out = ProcessSupersaw();
        break;
    case Waveform::HARDSYNC:
        out = ProcessHardSync();
        break;
    case Waveform::TRIANGLE_MOD:
        out = ProcessTriMod();
        break;
    default:
        break;
    }
    return out;
}

} // namespace dsp