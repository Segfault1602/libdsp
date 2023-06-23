#pragma once

#include "dsp_base.h"
#include "filter.h"
#include "termination.h"
#include "waveguide.h"

#include <algorithm>

namespace dsp
{

/// @brief Implements a bowed string model using a waveguide.
class BowedString
{
  public:
    BowedString() = default;
    ~BowedString() = default;

    void Init(float samplerate);

    /// @brief Set the frequency of the string
    /// @param f
    void SetFrequency(float f);

    float GetVelocity() const;

    /// @brief Set the velocity of the bow
    /// @param v The velocity of the bow
    void SetVelocity(float v);

    float GetForce() const;

    /// @brief Set the force of the bow
    /// @param f The force of the bow
    void SetForce(float f);

    /// @brief Excite the string
    void Excite();

    void Strike();

    float Tick();

  private:
    // Simple bowed string non-linear function taken from the STK.
    // https://github.com/thestk/stk/blob/master/include/BowTable.h
    float ComputeBowOutput(float in) const
    {
        float sample = (in + offset_) * force_;

        float out = std::fabs(sample) + 0.75f;
        out = std::pow(out, -4.0f);

        constexpr float min_output = 0.01f;
        constexpr float max_output = 0.98f;
        out = std::clamp(out, min_output, max_output);
        return out;
    }

    Waveguide<4096> waveguide_;
    OnePoleFilter reflection_filter_;
    Biquad body_filters_[6];
    float samplerate_;
    float output_pickup_;
    float freq_;
    float offset_ = 0.001f;
    float velocity_ = 0.25f;
    float force_ = 3.f;
};
} // namespace dsp