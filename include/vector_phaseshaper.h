#pragma once

#include <cstddef>

namespace sfdsp
{

class VectorPhaseshaper
{
  public:
    enum class FormantMode
    {
        /// @brief The formants frequencies are not tied to the fundamental
        FREE,

        /// @brief The formants are centered on exact harmonics of the fundamental
        RATIO,
    };

    VectorPhaseshaper() = default;
    ~VectorPhaseshaper() = default;

    /// @brief Initializes the oscillator
    /// @param samplerate
    void Init(float samplerate);

    /// @brief Sets the frequency
    /// @param freq Frequency in Hz
    void SetFreq(float freq);

    /// @brief Sets the modulation parameters
    /// @param d position of the point of inflection on the x-axis
    /// @param v position of the point of inflection on the y-axis
    void SetMod(float d, float v);

    /// @brief Sets the formant mode
    /// @param mode The formant mode
    void SetFormantMode(FormantMode mode);

    FormantMode GetFormantMode() const;

    /// @brief Processes a block of samples
    /// @param out The output buffer
    /// @param size The size of the buffer
    void ProcessBlock(float* out, size_t size);

  private:
    float samplerate_ = 0.f;
    float freq_ = 0.f;
    float phase_ = 0.f;
    float phaseIncrement_ = 0.f;

    float d_ = 0.5f;
    float v_ = 0.5f;

    FormantMode formantMode_ = FormantMode::FREE;
};

}; // namespace sfdsp