#pragma once

#include "dsp_base.h"
#include "filter.h"
#include "waveguide.h"

#include <algorithm>

namespace dsp
{

/// @brief Implements a bowed string model using a waveguide.
class BowedString
{
  public:
    BowedString()
    {
    }
    ~BowedString() = default;

    void Init(DspFloat samplerate)
    {
        samplerate_ = samplerate;
        SetFrequency(220);

        reflection_filter_.SetGain(0.95f);
        reflection_filter_.SetPole(0.75 - (0.2 * 22050.0 / samplerate_));

        waveguide_.RightTermination.SetGain(-1.f);
        waveguide_.RightTermination.SetFilter(&reflection_filter_);

        // Body filter provided by Esteban Maestre (cascade of second-order sections)
        // https://github.com/thestk/stk/blob/cc2dd22e9752bf5fd94f0799e01d19d5e8399058/src/Bowed.cpp#L62
        body_filters_[0].SetCoefficients(1.0, 1.5667, 0.3133, -0.5509, -0.3925);
        body_filters_[1].SetCoefficients(1.0, -1.9537, 0.9542, -1.6357, 0.8697);
        body_filters_[2].SetCoefficients(1.0, -1.6683, 0.8852, -1.7674, 0.8735);
        body_filters_[3].SetCoefficients(1.0, -1.8585, 0.9653, -1.8498, 0.9516);
        body_filters_[4].SetCoefficients(1.0, -1.9299, 0.9621, -1.9354, 0.9590);
        body_filters_[5].SetCoefficients(1.0, -1.9800, 0.9888, -1.9867, 0.9923);
    }

    /// @brief Set the frequency of the string
    /// @param f
    void SetFrequency(DspFloat f)
    {
        freq_ = f;
        DspFloat new_delay = (samplerate_ / freq_) * 0.5;
        waveguide_.SetDelay(new_delay);
        // waveguide_.SetJunction(waveguide_.GetDelay() - new_delay);
        output_pickup_ = new_delay - 10;
    }

    DspFloat GetVelocity() const
    {
        return velocity_;
    }

    /// @brief Set the velocity of the bow
    /// @param v The velocity of the bow
    void SetVelocity(DspFloat v)
    {
        velocity_ = v;
    }

    DspFloat GetForce() const
    {
        return force_;
    }

    /// @brief Set the force of the bow
    /// @param f The force of the bow
    void SetForce(DspFloat f)
    {
        force_ = f;
    }

    /// @brief Excite the string
    void Excite()
    {
        if (force_ < 0.001f )
        {
            return;
        }

        DspFloat bow_velocity = ( 0.2 * 0.5 ) * velocity_;

        constexpr DspFloat relative_position = 0.75f;
        DspFloat pos = waveguide_.GetDelay() * relative_position;
        DspFloat out = waveguide_.TapOut(pos);
        DspFloat deltaVelovity = bow_velocity - out;
        waveguide_.TapIn(pos, 0.3f * ComputeBowOutput(deltaVelovity));
    }

    DspFloat Tick()
    {
        waveguide_.Tick();
        DspFloat right, left;
        waveguide_.TapOut(waveguide_.GetDelay(), right, left);

        DspFloat out = 0.1248 * body_filters_[5].Tick(body_filters_[4].Tick(body_filters_[3].Tick(
                                    body_filters_[2].Tick(body_filters_[1].Tick(body_filters_[0].Tick(right))))));
        return out;
    }

  private:
    // Simple bowed string non-linear function taken from the STK.
    // https://github.com/thestk/stk/blob/master/include/BowTable.h
    DspFloat ComputeBowOutput(DspFloat in) const
    {
        DspFloat sample = (in + offset_) * force_;

        DspFloat out = std::fabs(sample) + 0.75f;
        out = std::pow(out, -4.0);

        constexpr DspFloat min_output = 0.01;
        constexpr DspFloat max_output = 0.98;
        out = std::clamp(out, min_output, max_output);
        return out;
    }

    Waveguide<4096> waveguide_;
    OnePoleFilter reflection_filter_;
    Biquad body_filters_[6];
    DspFloat samplerate_;
    DspFloat output_pickup_;
    DspFloat freq_;
    DspFloat offset_ = 0.001f;
    DspFloat velocity_ = 0.25f;
    DspFloat force_ = 3.f;
};
} // namespace dsp