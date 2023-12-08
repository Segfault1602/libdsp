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

void g_lin_block(float* x, size_t size, float a1 = 1, float a0 = 0)
{
    for (size_t i = 0; i < size; ++i)
    {
        x[i] = a1 * x[i] + a0;
    }
}

inline float g_ramp(float x, float a1 = 1, float a0 = 0)
{
    return MOD1(g_lin(x, a1, a0));
}

void g_ramp_block(float* x, size_t size, float a1 = 1, float a0 = 0)
{
    for (size_t i = 0; i < size; ++i)
    {
        x[i] = MOD1(a1 * x[i] + a0);
    }
}

inline float g_tri(float x, float a1 = 1, float a0 = 0)
{
    return MOD1(g_lin(std::abs(G_B(x)), a1, a0));
}

void g_tri_block(float* x, size_t size, float a1 = 1, float a0 = 0)
{
    for (size_t i = 0; i < size; ++i)
    {
        x[i] = MOD1(a1 * std::abs(G_B(x[i])) + a0);
    }
}

inline float g_pulse(float x, float width = 0.5f)
{
    if (x < width)
    {
        return 0.f;
    }

    return 1.f;
}

void g_pulse_block(float* x, size_t size, float width = 0.5f)
{
    for (size_t i = 0; i < size; ++i)
    {
        if (x[i] < width)
        {
            x[i] = 0.f;
        }
        else
        {
            x[i] = 1.f;
        }
    }
}

inline float s_tri(float x)
{
    if (x < 0.5f)
    {
        return 2.f * x;
    }

    return 2.f - 2.f * x;
}

void s_tri_block(float* x, size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        if (x[i] < 0.5f)
        {
            x[i] = 2.f * x[i];
        }
        else
        {
            x[i] = 2.f - 2.f * x[i];
        }
    }
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

void s_vtri_block(float* x, size_t size, float phaseIncrement, float w = 0.5f, float P = 100)
{
    for (size_t i = 0; i < size; ++i)
    {
        float d = std::ceil(w * P + 0.5f);
        float x1 = 2 * x[i] - 1;
        float xd = 2 * MOD1(x[i] + phaseIncrement * d) - 1;
        float xdiff = x1 * x1 - xd * xd;
        float at = 1.f / (8.f * (w - w * w));
        constexpr float bt = 0.5f;
        x[i] = g_lin(xdiff, at, bt);
    }
}

inline float g_vtri(float x, float phaseIncrement, float w = 0.5f, float a1 = 1.f, float a0 = 0.f, float P = 100)
{
    float vtri = s_vtri(x, phaseIncrement, w, P);
    return MOD1(g_lin(vtri, a1, a0));
}

void g_vtri_block(float* x, size_t size, float phaseIncrement, float w = 0.5f, float a1 = 1.f, float a0 = 0.f,
                  float P = 100)
{
    for (size_t i = 0; i < size; ++i)
    {
        float vtri = s_vtri(x[i], phaseIncrement, w, P);
        x[i] = MOD1(a1 * vtri + a0);
    }
}

inline float g_ripple(float x, float m = 1.f)
{
    // Orignal equation was 'x+MODM(x,m)', but I found that subtracting the modulo resulted in the same output but
    // without clipping.
    return G_B(x - MODM(x, m));
}

void g_ripple_block(float* x, size_t size, float m = 1.f)
{
    for (size_t i = 0; i < size; ++i)
    {
        x[i] = G_B(x[i] - MODM(x[i], m));
    }
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
    float out2 = 0.f;
    if (wave1 != wave2)
    {
        out2 = ProcessWave(static_cast<Waveform>(wave2));
    }

    float w1 = 1.f - (m_waveform - wave1);
    float w2 = 1.f - w1;

    m_phase += m_phaseIncrement;
    m_phase = MOD1(m_phase);

    return out1 * w1 + out2 * w2;
}

void Phaseshaper::ProcessBlock(float* out, size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        out[i] = m_phase;
        m_phase = MOD1(m_phase + m_phaseIncrement);
    }

    ProcessVarSlopeBlock(out, size);
}

float Phaseshaper::ProcessWaveSlice() const
{
    // a1 vary from 0.25  to 0.40
    float a1 = 0.25f + (m_mod * 0.50f);
    float slicePhase = g_lin(m_phase, a1);
    float trivial = G_B(sfdsp::Sine(slicePhase));

    float blep = polyBLEP(trivial, m_phase, m_phaseIncrement, -2);
    return blep;
}

float Phaseshaper::ProcessHardSync() const
{
    // a1 vary from 2 to 3
    float a1 = 2.0f + m_mod;
    return G_B(g_ramp(m_phase, a1));
}

float Phaseshaper::ProcessSoftSync() const
{
    // a1 vary from 1 to 1.5
    float a1 = 1.f + (m_mod * 0.5f);
    float softPhase = g_tri(m_phase, a1);
    return G_B(s_tri(softPhase));
}

float Phaseshaper::ProcessTriMod() const
{
    // atm vary from 0.5 to 1.5
    float mod = 0.5f + m_mod;
    float trimodPhase = mod * G_B(g_tri(m_phase));
    return 2 * (trimodPhase - std::ceil(trimodPhase - 0.5f));
}

float Phaseshaper::ProcessSupersaw() const
{
    // m1 vary from 0.25 to 0.75
    const float m1 = 0.25f + (m_mod * 0.50f);
    const float m2 = 0.88f;
    const float a1 = 1.5f;
    float xs = g_lin(m_phase, a1);

    float supersawPhase = MODM(xs, m1) + MODM(xs, m2);

    // Original equation was sin(supersawPhase) but since sfdsp::Sine expects a value between 0 and 1
    // we need to remove the implied 2pi factor.
    constexpr float one_over_2pi = 1.f / TWO_PI;
    return G_B(sfdsp::Sine(supersawPhase * one_over_2pi));
}

float Phaseshaper::ProcessVarSlope(float phase) const
{
    ProcessVarSlopeBlock(&phase, 1);
    return phase;
}

void Phaseshaper::ProcessVarSlopeBlock(float* x, size_t size) const
{
    // Width can vary from 0.1 to 0.5
    const float width = 0.5f + (m_mod * 0.25f);

    for (size_t i = 0; i < size; ++i)
    {
        float phase = x[i];

        const float pulse = 0.5 * g_pulse(phase, width);
        x[i] = 0.5f * phase * (1.0f - pulse) / width + pulse * (phase - width) / (1 - width);
    }

    sfdsp::Sine(x, x, size);
}

float Phaseshaper::ProcessVarTri() const
{
    // a1 can vary from 1.25 to 1.75
    float a1 = 1.25f + (m_mod * 0.5f);
    float w3 = 0.50;
    float vtri = g_vtri(m_phase, m_phaseIncrement, w3, a1, 0, m_period);
    return sfdsp::Sine(vtri);
}

float Phaseshaper::ProcessRipple() const
{
    // ripples amount goes from no ripple at 0 to some ripples at 1.
    float ripple_amount = m_mod * 0.1f;
    return g_ripple(m_phase, ripple_amount);
}

float Phaseshaper::ProcessWave(Waveform wave) const
{
    float out = 0.f;
    switch (wave)
    {
    case Waveform::VARIABLE_SLOPE:
        out = ProcessVarSlope(m_phase);
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

} // namespace sfdsp