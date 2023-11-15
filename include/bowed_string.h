#pragma once

#include "bow_table.h"
#include "dsp_base.h"
#include "filter.h"
#include "rms.h"
#include "smooth_param.h"
#include "termination.h"
#include "waveguide.h"
#include "waveguide_gate.h"

#include <algorithm>
#include <optional>

namespace sfdsp
{

struct BowedStringConfig
{
    float samplerate;
    size_t max_delay_size;
    float open_string_tuning;
    float nut_gain;
    std::optional<OnePoleFilter> bridge_filter;
};

static BowedStringConfig kDefaultStringConfig = {
    .samplerate = 48000.f,
    .max_delay_size = 1024,
    .open_string_tuning = 196.f,
    .nut_gain = -0.98f,
    .bridge_filter = std::nullopt,
};

/// @brief Implements a bowed string model using a waveguide composed of a right traveling wave and a left traveling
/// wave.
class BowedString
{
  public:
    BowedString(size_t max_size = 1024);
    ~BowedString() = default;

    /// @brief Initialize the string
    /// @param samplerate The samplerate of the system
    /// @param tuning The tuning of the open string in Hz, Default to G3
    void Init(const BowedStringConfig& config = kDefaultStringConfig);

    /// @brief Set the frequency of the string
    /// @param f The frequency of the string in Hz
    void SetFrequency(float f);

    /// @brief Returns the frequency of the string in Hz
    /// @return The frequency of the string in Hz
    float GetFrequency() const;

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
    float Tick(float input);

    /// @brief Set the finger pressure.
    /// @param pressure 0 is no pressure, 1 is full pressure.
    void SetFingerPressure(float pressure);

    enum class ParamId
    {
        Velocity,
        Force,
        BowPosition,
        FingerPressure,
        NutGain,
        BridgeFilterCutoff,
        TuningAdjustment,
    };

    void SetParameter(ParamId param_id, float value);

  private:
    Waveguide waveguide_;
    WaveguideGate gate_;

    float bow_position_ = 0.f;
    SmoothParam velocity_;
    SmoothParam bow_force_;
    SmoothParam gate_delay_;

    float relative_bow_position_ = 0.15f;

    float tuning_adjustment_ = 0.f;
    float open_string_delay_ = 0.f;

    Termination nut_;
    Termination bridge_;
    BowTable bow_table_;
    LinearInterpolation bow_interpolation_strategy_;

    OnePoleFilter reflection_filter_;
    float samplerate_ = 0.f;
    float freq_ = 0.f;
    bool note_on_ = false;

    OnePoleFilter decay_filter_;
    OnePoleFilter noise_lp_filter_;
};
} // namespace sfdsp