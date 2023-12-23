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

/// @brief Compute a sine wave from a buffer of phases
/// @param phases The buffer of phases
/// @param out The output buffer, can be the same as `phases`
/// @param size The size of the output buffer
/// @return The phase of the sine wave after processing the buffer
void Sine(const float* phases, float* out, size_t size);

/// @brief Simple cosine wave
/// @param phase Phase of the cosine wave
/// @return The value of the cosine wave at the given phase
/// @ingroup Oscillators
float Cosine(float phase);

/// @brief Compute a cosine wave from a buffer of phases
/// @param phases The buffer of phases
/// @param out The output buffer, can be the same as `phases`
/// @param size The size of the output buffer
/// @return The phase of the cosine wave after processing the buffer
void Cosine(const float* phases, float* out, size_t size);

/// @brief Non-bandlimited triangle wave
/// @param phase Phase of the triangle wave
/// @return The value of the triangle wave at the given phase
/// @ingroup Oscillators
float Tri(float phase);

/// @brief Non-bandlimited saw wave
/// @param phase Phase of the saw wave
/// @return The value of the saw wave at the given phase
/// @ingroup Oscillators
float Saw(float phase);

/// @brief Non-bandlimited square wave
/// @param phase Phase of the square wave
/// @return The value of the square wave at the given phase
/// @ingroup Oscillators
float Square(float phase, float duty = 0.5f);

/// @brief Simple noise generator
/// @return A random value between -1 and 1
/// @ingroup Oscillators
float Noise();

enum class OscillatorType
{
    Sine,
    Cosine,
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

    /// @brief Set the duty cycle of the oscillator when oscillator type is square
    /// @param duty
    void SetDuty(float duty);

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
    float duty_ = 0.5f;

    float phase_ = 0.f;
    float phase_increment_ = 0.f;
};
} // namespace sfdsp