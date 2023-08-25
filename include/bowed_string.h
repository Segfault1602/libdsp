#pragma once

#include "bow_table.h"
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
    BowedString();
    ~BowedString() = default;

    void Init(float samplerate);

    /// @brief Set the frequency of the string
    /// @param f
    void SetFrequency(float f);
    float GetFrequency() const;

    void SetDelay(float delay);

    void SetLastMidiNote(float midi_note);
    float GetLastMidiNote() const;

    float GetVelocity() const;

    void SetForce(float f);

    /// @brief Set the velocity of the bow
    /// @param v The velocity of the bow
    void SetVelocity(float v);

    void Strike();

    float Tick(bool note_on);

  private:
    Waveguide waveguide_;

    float bow_position_ = 0.f;
    float last_midi_note_ = 0.f;

    Termination nut_;
    Termination bridge_;
    BowTable bow_table_;

    OnePoleFilter reflection_filter_;
    Biquad body_filters_[6];
    float samplerate_;
    float freq_;
    float velocity_ = 0.f;

    constexpr static float max_velocity_ = 0.3f;
    constexpr static float velocity_offset_ = 0.03f;
};
} // namespace dsp