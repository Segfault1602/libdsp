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
#include <cstddef>
#include <cstdint>

#include "dsp_utils.h"

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
    /// For example, a value of 1.5 will interpolate between Waveform::SUPERSAW and Waveform::WAVESLICE equally.
    void SetWaveform(float wave)
    {
        waveform_ = wave;
    }

    /// @brief Sets the waveform.
    /// @param wave Waveform to use.
    void SetWaveform(Waveform wave)
    {
        waveform_ = static_cast<float>(wave);
    }

    /// @brief Sets the frequency
    /// @param freq Frequency in Hz
    void SetFreq(float freq)
    {
        freq_ = freq;
        phaseIncrement_ = freq_ / samplerate_;
        period_ = samplerate_ / freq_;
    }

    /// @brief Sets the modulation amount. Can be modified at audio rate.
    /// @param mod Modulation amount, clamped between 0 and 1
    /// The modulation parameter is used differently depending on the waveform.
    /// For example, for Waveform::VARIABLE_SLOPE, the modulation parameter controls the width of the waveform.
    void SetMod(float mod);

    /// @brief Processes the oscillator
    /// @return The next sample
    float Process();

    void ProcessBlock(float* out, size_t size);

  private:
    float ProcessWaveSlice() const;
    void ProcessWaveSliceBlock(const float* p, float* out, size_t size, float gain) const;

    float ProcessHardSync() const;
    void ProcessHardSyncBlock(const float* p, float* out, size_t size, float gain) const;

    float ProcessSoftSync() const;
    void ProcessSoftSyncBlock(const float* p, float* out, size_t size, float gain) const;

    float ProcessTriMod() const;
    void ProcessTriModBlock(const float* p, float* out, size_t size, float gain) const;

    float ProcessSupersaw() const;
    void ProcessSupersawBlock(const float* p, float* out, size_t size, float gain) const;

    float ProcessVarSlope(float phase) const;
    void ProcessVarSlopeBlock(const float* p, float* out, size_t size, float gain) const;

    float ProcessVarTri() const;
    void ProcessVarTriBlock(const float* p, float* out, size_t size, float gain) const;

    float ProcessRipple() const;
    void ProcessRippleBlock(const float* p, float* out, size_t size, float gain) const;

    float ProcessWave(Waveform wave);
    void ProcessWaveBlock(const float* p, float* out, size_t size, Waveform wave, float gain);

  private:
    float samplerate_ = 0.f;
    float freq_ = 220.f;
    float phaseIncrement_ = 0.f;
    float phase_ = 0.f;
    float period_ = 0.f;
    float waveform_ = static_cast<float>(Waveform::WAVESLICE);

    float mod_ = 0.f;
};
} // namespace sfdsp