#pragma once

#include "dsp_base.h"
#include "excitation_models.h"
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
    float GetFrequency() const;

    void SetLastMidiNote(float midi_note);
    float GetLastMidiNote() const;

    float GetVelocity() const;

    /// @brief Set the velocity of the bow
    /// @param v The velocity of the bow
    void SetVelocity(float v);

    void Strike();

    float Tick(const ExcitationModel* excitation_model);

  private:
    Waveguide<4096> waveguide_;

    float bow_position_ = 0.f;
    float last_midi_note_ = 0.f;

    Termination nut_;
    Termination bridge_;

    OnePoleFilter reflection_filter_;
    Biquad body_filters_[6];
    float samplerate_;
    float freq_;
    float velocity_ = 0.5f;
};
} // namespace dsp