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

namespace sfdsp
{

/// @brief Phaseshaper oscillator that can morph between different waveforms
class Phaseshaper
{
  public:
    /// @brief Waveform types
    /// @note The order of the waveforms is intentional: the waveform are ordered in such a
    /// way that make interpolating between them more aesthetically pleasing.
    enum class Waveform : uint8_t
    {
        VARIABLE_SLOPE = 0, // 0.10 - 0.50
        // VARIABLE_TRIANGLE, // 1.25 - 1.75 w=05
        WAVESLICE, // 0.25 - 0.40
        SUPERSAW,  // 0.25 - 0.75
        RIPPLE,
        // HARDSYNC, // 2 - 3
        SOFTSYNC,     // 1 - 1.5
        TRIANGLE_MOD, // 0.5 - 1.5
        NUM_WAVES,
    };

    Phaseshaper() = default;
    ~Phaseshaper() = default;

    /// @brief Initializes the oscillator
    /// @param sampleRate
    void Init(float sampleRate);

    /// @brief Sets the waveform
    /// @param wave floating point value between Waveform::VARIABLE_SLOPE and Waveform::NUM_WAVES
    /// @note The fractional part of the value is used to interpolate between waveforms.
    /// For example, a value of 1.5 will interpolate between Waveform::SOFTSYNC and Waveform::WAVESLICE equally.
    void SetWaveform(float wave)
    {
        m_waveform = wave;
    }

    void SetWaveform(Waveform wave)
    {
        m_waveform = static_cast<float>(wave);
    }

    /// @brief Sets the frequency
    /// @param freq Frequency in Hz
    void SetFreq(float freq)
    {
        m_freq = freq;
        m_phaseIncrement = m_freq / m_sampleRate;
        m_period = m_sampleRate / m_freq;
    }

    /// @brief Sets the modulation amount
    /// @param mod Modulation amount, clamped between 0 and 1
    /// The modulation parameter is used differently depending on the waveform.
    /// For example, for Waveform::VARIABLE_SLOPE, the modulation parameter controls the width of the waveform.
    void SetMod(float mod);

    /// @brief Processes the oscillator
    /// @return
    float Process();

  private:
    float ProcessWaveSlice() const;
    float ProcessHardSync() const;
    float ProcessSoftSync() const;
    float ProcessTriMod() const;
    float ProcessSupersaw() const;
    float ProcessVarSlope() const;
    float ProcessVarTri() const;
    float ProcessRipple() const;

    float ProcessWave(Waveform wave) const;

  private:
    float m_sampleRate = 0.f;
    float m_freq = 220.f;
    float m_phaseIncrement = 0.f;
    float m_phase = 0.f;
    float m_period = 0.f;
    float m_waveform = static_cast<float>(Waveform::WAVESLICE);

    float m_mod = 0.f;
};
} // namespace sfdsp