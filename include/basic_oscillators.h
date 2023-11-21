/// @file
/// Implementation of basic oscillators
/// @defgroup Oscillators
#pragma once

#include <cstddef>

namespace sfdsp
{
/// @brief Simple sine wave
/// @param phase Phase of the sine wave
/// @return The value of the sine wave at the given phase
/// @ingroup Oscillators
float Sine(float phase);

/// @brief Simple triangle wave
/// @param phase Phase of the triangle wave
/// @return The value of the triangle wave at the given phase
/// @ingroup Oscillators
float Tri(float phase);

/// @brief Simple saw wave
/// @param phase Phase of the saw wave
/// @return The value of the saw wave at the given phase
/// @ingroup Oscillators
float Saw(float phase);

/// @brief Simple square wave
/// @param phase Phase of the square wave
/// @return The value of the square wave at the given phase
/// @ingroup Oscillators
float Square(float phase);

/// @brief Simple noise generator
/// @return A random value between -1 and 1
/// @ingroup Oscillators
float Noise();

enum class OscillatorType
{
    Sine,
    Tri,
    Saw,
    Square,
};

/// @brief Basic oscillator class
/// @ingroup Oscillators
class BasicOscillator
{
  public:
    /// @brief Construct a BasicOscillator object
    BasicOscillator() = default;

    /// @brief Initialize the oscillator
    /// @param samplerate The samplerate of the audio system
    /// @param type The type of the oscillator
    void Init(float samplerate, float freq, OscillatorType type = OscillatorType::Sine);

    /// @brief Set the type of the oscillator
    /// @param type The type of the oscillator
    void SetType(OscillatorType type);

    /// @brief Return the type of the oscillator
    /// @return The type of the oscillator
    OscillatorType GetType() const;

    /// @brief Set the frequency of the oscillator
    /// @param frequency The frequency of the oscillator in Hz
    void SetFrequency(float frequency);

    /// @brief Return the frequency of the oscillator
    /// @return The frequency of the oscillator in Hz
    float GetFrequency() const;

    /// @brief Set the phase of the oscillator
    /// @param phase The phase of the oscillator
    void SetPhase(float phase);

    /// @brief Return the current phase of the oscillator
    /// @return The current phase of the oscillator
    float GetPhase() const;

    float Tick();

    void ProcessBlock(float* out, size_t size);

  private:
    OscillatorType type_ = OscillatorType::Sine;
    float frequency_ = 0.f;
    float samplerate_;

    float phase_ = 0.f;
    float phase_increment_ = 0.f;
};
} // namespace sfdsp