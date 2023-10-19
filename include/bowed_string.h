#pragma once

#include "bow_table.h"
#include "dsp_base.h"
#include "filter.h"
#include "rms.h"
#include "termination.h"
#include "waveguide.h"

#include <algorithm>

namespace dsp
{

/// @brief Implements a bowed string model using a waveguide composed of a right traveling wave and a left traveling
/// wave.
class BowedString
{
  public:
    BowedString(size_t max_size = 1024);
    ~BowedString() = default;

    void Init(float samplerate);

    /// @brief Set the frequency of the string
    /// @param f The frequency of the string in Hz
    void SetFrequency(float f);

    /// @brief Returns the frequency of the string in Hz
    /// @return The frequency of the string in Hz
    float GetFrequency() const;

    /// @brief Set the length of the string in samples
    /// @param delay The length of the string in samples
    /// @note this method exists mostly to allow easy testing of the model with different delay values. Use
    /// `SetFrequency()` to change the pitch of the string.
    void SetDelay(float delay);

    /// @brief Return the current bow velocity.
    /// @return The current bow velocity.
    float GetVelocity() const;

    /// @brief Set the velocity of the bow
    /// @param v The velocity of the bow. Value should be between 0 and 1.
    void SetVelocity(float v);

    /// @brief Set the force of the bow
    /// @param f the force of the bow. Value should be between 0 and 1.
    void SetForce(float f);

    /// @brief Returns the force of the bow
    /// @return The force of the bow
    float GetForce() const;

    /// @brief Set the position of the bow on the string.
    /// @param pos The position of the bow on the string. Value should be between 0 and 1.
    /// @note A position of 0 would be right at the bridge, and a position of 1 would be at the nut/finger.
    void SetBowPosition(float pos);

    /// @brief Returns the position of the bow on the string.
    /// @return The position of the bow on the string.
    /// @note A position of 0 would be right at the bridge, and a position of 1 would be at the nut/finger.
    float GetBowPosition() const;

    /// @brief Set the note on state of the string.
    /// @param note_on
    void SetNoteOn(bool note_on);

    /// @brief Returns the note on state of the string.
    bool GetNoteOn() const;

    /// @brief Pluck the string.
    void Pluck();

    /// @brief Return the next sample at the bridge.
    /// @return The next sample at the bridge.
    float NextOut();

    /// @brief Tick the string.
    /// @param input Energy coming from the bridge. Optional.
    /// @return The output sample at the bridge.
    float Tick(float input = 0.f);

  private:
    Waveguide waveguide_;

    float bow_position_ = 0.f;
    float relative_bow_position_ = 0.2f;

    Termination nut_;
    Termination bridge_;
    BowTable bow_table_;
    LinearInterpolation bow_interpolation_strategy_;

    OnePoleFilter reflection_filter_;
    float samplerate_;
    float freq_;
    float velocity_ = 0.f;
    bool note_on_ = false;

    constexpr static float max_velocity_ = 0.2f;
    constexpr static float velocity_offset_ = 0.03f;

    OnePoleFilter decay_filter_;
    OnePoleFilter noise_lp_filter_;
};
} // namespace dsp