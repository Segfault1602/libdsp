#pragma once

#include "filter.h"

namespace sfdsp
{

/// @brief Class that allows smoothing of a parameter. Useful to take a control rate parameter and smooth it to audio
/// rate.
class SmoothParam
{
  public:
    /// @brief Smoothing types
    enum class SmoothingType
    {
        /// @brief No smoothing, `Tick()` always return the target value.
        None,

        /// @brief Exponential smoothing.
        /// @details A one pole filter is used to smooth the parameter.
        Exponential,
    };

    SmoothParam() = default;

    /// @brief Initializes the smoothing parameter.
    /// @param samplerate The samplerate of the system.
    /// @param type The smoothing type.
    /// @param value The initial target value.
    void Init(size_t samplerate, SmoothingType type, float value = 0);

    /// @brief Sets the target value.
    /// @param target The target value.
    void SetTarget(float target);

    /// @brief Returns the target value.
    /// @return The target value.
    float GetTarget() const;

    /// @brief Returns the next smoothed value.
    /// @return The next smoothed value.
    float Tick();

  private:
    SmoothingType type_;
    float value_;

    OnePoleFilter smoothing_filter_;
};
} // namespace sfdsp