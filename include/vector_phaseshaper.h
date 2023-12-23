#pragma once

#include <cstddef>

namespace sfdsp
{

class VectorPhaseshaper
{
  public:
    VectorPhaseshaper() = default;
    ~VectorPhaseshaper() = default;

    /// @brief Initializes the oscillator
    /// @param samplerate
    void Init(float samplerate);

    /// @brief Sets the frequency
    /// @param freq Frequency in Hz
    void SetFreq(float freq);

    void SetMod(float d, float v);

    void ProcessBlock(float* out, size_t size);

  private:
    float samplerate_ = 0.f;
    float freq_ = 0.f;
    float phase_ = 0.f;
    float phaseIncrement_ = 0.f;

    float d_ = 0.5f;
    float v_ = 0.5f;
};

}; // namespace sfdsp