// =============================================================================
// phaseshapers.h -- Phaseshaper implementations
//
// Original implementation by:
// J.Kleimola, V.Lazzarini, J.Timoney, V.Valimaki,
// "PHASESHAPING OSCILLATOR ALGORITHMS FOR MUSICAL SOUND SYNTHESIS",
// SMC 2010, Barcelona, Spain, July 21-24, 2010.
//
// Adapted from python by:
// Alex St-Onge
// =============================================================================
#pragma once
#include <stdint.h>

#include "dsp_base.h"

namespace dsp
{

class Phaseshaper
{
  public:
    enum class Waveform : uint8_t
    {
        VARIABLE_SLOPE = 0,
        SOFTSYNC,
        WAVESLICE,
        SUPERSAW,
        HARDSYNC,
        TRIANGLE_MOD,
        NUM_WAVES,
    };

    Phaseshaper() = default;
    ~Phaseshaper() = default;

    void Init(float sampleRate)
    {
        m_sampleRate = sampleRate;
        m_phase = 0.f;
        m_freq = 220.f;
        m_phaseIncrement = m_freq / m_sampleRate;
        m_period = m_sampleRate / m_freq;
    }

    void SetWaveform(float wave)
    {
        m_waveform = wave;
    }

    void SetFreq(float freq)
    {
        m_freq = freq;
        m_phaseIncrement = m_freq / m_sampleRate;
        m_period = m_sampleRate / m_freq;
    }

    void SetMod(float mod)
    {
        m_mod = mod;
    }

    float Process();

  private:
    float ProcessWaveSlice();
    float ProcessHardSync();
    float ProcessSoftSync();
    float ProcessTriMod();
    float ProcessSupersaw();
    float ProcessVarSlope();
    float ProcessVarTri();

    inline float ProcessWave(Waveform wave)
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

  private:
    float m_sampleRate = 0.f;
    float m_freq = 220.f;
    float m_phaseIncrement = 0.f;
    float m_phase = 0.f;
    float m_period = 0.f;
    float m_waveform = static_cast<float>(Waveform::WAVESLICE);

    float m_mod = 0.f;
};
} // namespace dsp