#pragma once

#include "delayline.h"

namespace dsp
{

/// @brief Root Mean Square calculator
class RMS
{
  public:
    /// @brief Construct a new RMS calculator
    /// @param size Size of the RMS window
    RMS(size_t size);
    ~RMS() = default;

    /// @brief Tick the RMS calculator
    /// @param input Input sample
    /// @return The current RMS value
    float Tick(float input);

    /// @brief Get the current RMS value
    /// @return The current RMS value
    float GetRMS() const;

  private:
    Delayline buffer_;
    float running_sum_ = 0.f;
    float factor_ = 0.f;
    float last_out_ = 0.f;
};

} // namespace dsp