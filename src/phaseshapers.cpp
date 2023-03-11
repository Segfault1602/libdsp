// =============================================================================
// phaseshapers.cpp -- Phaseshaper implementations
//
// Original implementation by:
// J.Kleimola, V.Lazzarini, J.Timoney, V.Valimaki,
// "PHASESHAPING OSCILLATOR ALGORITHMS FOR MUSICAL SOUND SYNTHESIS",
// SMC 2010, Barcelona, Spain, July 21-24, 2010.
//
// Adapted from python by:
// Alex St-Onge
// =============================================================================

#include "phaseshapers.h"

#include <cmath>
#include <stdint.h>

#define PI_F 3.1415927410125732421875f
#define TWOPI_F (2.0f * PI_F)

#define G_B(x) (2 * x - 1)

// -- Linear transformations

inline float g_lin(float x, float a1 = 1, float a0 = 0)
{
    return a1 * x + a0;
}

inline float g_ramp(float x, float a1 = 1, float a0 = 0)
{
    return fmodf(g_lin(x, a1, a0), 1.f);
}

inline float g_tri(float x, float a1 = 1, float a0 = 0)
{
    return fmodf(g_lin(fabsf(G_B(x)), a1, a0), 1.f);
}

inline float g_pulse(float x, float phaseIncrement, float width = 0.5f, int period = 100)
{
    float d = static_cast<int>(width * period);
    return x - fmodf(x + phaseIncrement * d, 1.f) + width;
}

inline float g_pulse_trivial(float x, float width = 0.5f)
{
    return (x < width) ? 0 : 1;
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

    return 0.f;
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

float Phaseshaper::Process()
{
    float wave1 = std::floor(m_waveform);
    float out1 = ProcessWave(static_cast<Waveform>(wave1));

    float wave2 = std::ceil(m_waveform);
    float out2 = ProcessWave(static_cast<Waveform>(wave2));

    float w1 = 1 - (m_waveform - wave1);
    float w2 = 1 - w1;

    m_phase += m_phaseIncrement;
    m_phase = fmodf(m_phase, 1.f);

    return out1 * w1 + out2 * w2;
}

float Phaseshaper::ProcessWaveSlice()
{
    float a1 = 0.25f + (1 + m_mod) * 0.10;
    float slicePhase = g_lin(m_phase, a1);
    float trivial = G_B(std::sin(TWOPI_F * slicePhase));

    float blep = polyBLEP(m_phase, m_phaseIncrement, -2);
    return trivial + blep;
}

float Phaseshaper::ProcessHardSync()
{
    float a1 = 2.5f + m_mod;
    return G_B(g_ramp(m_phase, a1));
}

float Phaseshaper::ProcessSoftSync()
{
    float a1 = 1.25f + m_mod;
    float softPhase = g_tri(m_phase, a1);
    return G_B(s_tri(softPhase));
}

float Phaseshaper::ProcessTriMod()
{
    const float atm = 0.82f; // Roland JP-8000 triangle modulation offset parameter
    float mod = atm + m_mod * 0.15;
    float trimodPhase = mod * G_B(g_tri(m_phase));
    return 2 * (trimodPhase - std::ceil(trimodPhase - 0.5f));
}

float Phaseshaper::ProcessSupersaw()
{
    const float m1 = 0.5f + (m_mod * 0.25f);
    const float m2 = 0.88f;
    const float a1 = 1.5f;
    float xs = g_lin(m_phase, a1);

    float supersawPhase = fmodf(xs, m1) + fmodf(xs, m2);
    return G_B(std::sin(supersawPhase));
}

float Phaseshaper::ProcessVarSlope()
{
    float width = 0.5f + m_mod * 0.25f;

    float pulse = g_pulse(m_phase, m_phaseIncrement, width, m_period);
    float vslope = 0.5f * m_phase * (1.0f - pulse) / width + pulse * (m_phase - width) / (1 - width);
    return std::sin(TWOPI_F * vslope);
}
} // namespace PhaseShapers