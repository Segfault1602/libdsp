#pragma once

#include "bow_table.h"
#include "dsp_utils.h"
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

/// @brief Configuration for the bowed string model.
struct BowedStringConfig
{
    /// @brief The samplerate of the system.
    float samplerate;
    /// @brief The maximum size of the delayline used in the underlying waveguide, in samples.
    size_t max_delay_size;
    /// @brief The frequency of the open string.
    float open_string_tuning;
    /// @brief The gain (loss) at the nut.
    float nut_gain;
    /// @brief Optional. The bridge filter. The default bridge filter will be used if not specified.
    std::optional<OnePoleFilter> bridge_filter;
};

/// @brief Default configuration for the bowed string model. Corresponds to a string tuned to 196 Hz.
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
    /// @brief Construct a bowed string model.
    /// @param max_size The maximum size of the delayline used in the underlying waveguide, in samples.
    BowedString(size_t max_size = 1024);
    ~BowedString() = default;

    /// @brief Initialize the string
    /// @param config The configuration of the string.
    void Init(const BowedStringConfig& config = kDefaultStringConfig);

    /// @brief Set the frequency of the string
    /// @param f The frequency of the string in Hz
    void SetFrequency(float f);

    /// @brief Returns the frequency of the string in Hz
    /// @return The frequency of the string in Hz
    float GetFrequency() const;

    /// @brief Pluck the string.
    void Pluck();

    /// @brief Return the next sample at the bridge.
    /// @return The next sample at the bridge.
    float NextOut();

    /// @brief Tick the string.
    /// @param input Energy coming from the bridge.
    /// @return The output sample at the bridge.
    float Tick(float input);

    /// @brief Modifiable parameters for the bowed string model.
    enum class ParamId
    {
        /// @brief The velocity of the bow. 0 the bow is not moving, 1 the bow is moving at full speed.
        Velocity,
        /// @brief The force of the bow. 0 the bow is not touching the string, 1 the bow is applying full force.
        Force,
        /// @brief The position of the bow. 0 the bow is at the bridge, 1 the bow is at the nut.
        BowPosition,
        /// @brief The finger pressure. 0 the finger is not touching the string, 1 the finger is applying full pressure.
        /// Values between 0 and 0.1 can result in harmonics.
        FingerPressure,
        /// @brief The gain (loss) of the nut. At 0, all the energy is lost at the nut. At 1, all the energy is
        /// reflected back.
        NutGain,
        /// @brief The cutoff frequency of the bridge filter. At 0, the cutoff is at a normalized frequency of 0.025. At
        /// 1, the cutoff is at 0.125.
        /// @details Changing the bridge filter can slightly detune the string. Use the tuning adjustment parameter to
        /// manually tune the string. Not recommended for real time use.
        BridgeFilterCutoff,
        /// @brief Used to fine tune the frequency of the string. At 0, 5 samples of delay are removed from the string.
        /// At 1, 5 samples of delay are added.
        TuningAdjustment,
    };

    /// @brief Sets a parameter for the string.
    /// @param param_id The parameter to set.
    /// @param value The value of the parameter. Between 0 and 1.
    void SetParameter(ParamId param_id, float value);

  private:
    void SetForce(float f);
    void SetVelocity(float v);
    void SetBowPosition(float p);

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