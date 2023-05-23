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

#define G_B(x) (2 * x - 1)

// -- Linear transformations

inline DspFloat g_lin(DspFloat x, DspFloat a1 = 1, DspFloat a0 = 0)
{
    return a1 * x + a0;
}

inline DspFloat g_ramp(DspFloat x, DspFloat a1 = 1, DspFloat a0 = 0)
{
    return std::fmodf(g_lin(x, a1, a0), 1.f);
}

inline DspFloat g_tri(DspFloat x, DspFloat a1 = 1, DspFloat a0 = 0)
{
    return std::fmodf(g_lin(std::abs(G_B(x)), a1, a0), 1.f);
}

inline DspFloat g_pulse(DspFloat x, DspFloat phaseIncrement, DspFloat width = 0.5f, int period = 100)
{
    DspFloat d = static_cast<int>(width * period);
    return x - std::fmodf(x + phaseIncrement * d, 1.f) + width;
}

inline DspFloat g_pulse_trivial(DspFloat x, DspFloat width = 0.5f)
{
    return (x < width) ? 0 : 1;
}

inline DspFloat s_tri(DspFloat x)
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

DspFloat polyBLEP(DspFloat phase, DspFloat phaseIncrement, DspFloat h = -1.f)
{
    DspFloat out = 0.f;
    DspFloat p = phase;
    p -= floorf(p);

    if (p > (1 - phaseIncrement))
    {
        DspFloat t = (p - 1) / phaseIncrement;
        DspFloat c = 0.5f * t * t + t * 0.5f;
        c *= h;
        out = c;
    }
    else if (p < phaseIncrement)
    {
        DspFloat t = p / phaseIncrement;
        DspFloat c = -0.5f * t * t + t - 0.5f;
        c *= h;
        out = c;
    }

    return out;
}

namespace dsp
{

DspFloat Phaseshaper::Process()
{
    DspFloat wave1 = std::floor(m_waveform);
    DspFloat out1 = ProcessWave(static_cast<Waveform>(wave1));

    DspFloat wave2 = std::ceil(m_waveform);
    DspFloat out2 = ProcessWave(static_cast<Waveform>(wave2));

    DspFloat w1 = 1 - (m_waveform - wave1);
    DspFloat w2 = 1 - w1;

    m_phase += m_phaseIncrement;
    m_phase = std::fmodf(m_phase, 1.f);

    return out1 * w1 + out2 * w2;
}

DspFloat Phaseshaper::ProcessWaveSlice()
{
    DspFloat a1 = 0.25f + (1 + m_mod) * 0.10;
    DspFloat slicePhase = g_lin(m_phase, a1);
    DspFloat trivial = G_B(std::sin(TWO_PI * slicePhase));

    DspFloat blep = polyBLEP(m_phase, m_phaseIncrement, -2);
    return trivial + blep;
}

DspFloat Phaseshaper::ProcessHardSync()
{
    DspFloat a1 = 2.5f + m_mod;
    return G_B(g_ramp(m_phase, a1));
}

DspFloat Phaseshaper::ProcessSoftSync()
{
    DspFloat a1 = 1.25f + m_mod;
    DspFloat softPhase = g_tri(m_phase, a1);
    return G_B(s_tri(softPhase));
}

DspFloat Phaseshaper::ProcessTriMod()
{
    const DspFloat atm = 0.82f; // Roland JP-8000 triangle modulation offset parameter
    DspFloat mod = atm + m_mod * 0.15;
    DspFloat trimodPhase = mod * G_B(g_tri(m_phase));
    return 2 * (trimodPhase - std::ceil(trimodPhase - 0.5f));
}

DspFloat Phaseshaper::ProcessSupersaw()
{
    const DspFloat m1 = 0.5f + (m_mod * 0.25f);
    const DspFloat m2 = 0.88f;
    const DspFloat a1 = 1.5f;
    DspFloat xs = g_lin(m_phase, a1);

    DspFloat supersawPhase = std::fmodf(xs, m1) + std::fmodf(xs, m2);
    return G_B(std::sin(supersawPhase));
}

DspFloat Phaseshaper::ProcessVarSlope()
{
    DspFloat width = 0.5f + m_mod * 0.25f;

    DspFloat pulse = g_pulse(m_phase, m_phaseIncrement, width, m_period);
    DspFloat vslope = 0.5f * m_phase * (1.0f - pulse) / width + pulse * (m_phase - width) / (1 - width);
    return std::sin(TWO_PI * vslope);
}
} // namespace dsp