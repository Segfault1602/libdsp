// =============================================================================
// chorus.h
// Simple chorus effect.
// =============================================================================
#pragma once

#define CHORUS_LINEAR_DELAY

#ifdef CHORUS_LINEAR_DELAY
#include "delayline.h"
#else
#include "delayline_sinc.h"
#endif
#include "dsp_base.h"
#include <cmath>

namespace sfdsp
{
/// @brief Simple chorus effect.
class Chorus : public DspBase
{
  public:
    /// @brief Constructor.
    /// @param max_delay_size Maximum delay size in samples.
    Chorus(size_t max_delay_size);

    ~Chorus() override = default;
    Chorus(const Chorus& c) = delete;

    /// @brief Initialize the chorus.
    /// @param samplerate Samplerate in Hz.
    /// @param delay_ms Delay in milliseconds.
    /// @param width Width in samples.
    /// @param speed Speed in Hz.
    void Init(uint32_t samplerate, float delay_ms, float width = 100, float speed = .5f);

    /// @brief Reset the chorus.
    /// @note This will clear the delayline.
    void Reset();

    /// @brief Set the delay in milliseconds.
    /// @param delay_ms Delay in milliseconds.
    void SetDelay(float delay_ms);

    /// @brief Set the mix between the dry signal and the chorus.
    /// @param mix The mix between the dry signal and the chorus.
    void SetMix(float mix);

    /// @brief Set the width in samples.
    /// @param width Width in samples.
    /// @note A bigger width will result in a more noticeable effect. Vibrato like sounds can be achieved by incresing
    /// this value.
    void SetWidth(float width);

    /// @brief Set the speed in Hz.
    /// @param speed Speed in Hz.
    void SetSpeed(float speed);

    /// @brief Tick the chorus.
    /// @param in audio sample
    /// @return The processed audio sample.
    float Tick(float in) override;

  private:
    uint32_t samplerate_ = 48000;
    float samples_per_ms_ = static_cast<float>(samplerate_) / 1000.f;

    float base_delay_ms_ = 0.f;
    float base_delay_ = 0.f;
    Delayline delay_;

    float width_ = 50.f;
    float speed_ = 5.f;

    float mod_phase_dt_ = 0.f;
    float mod_phase_ = 0.f;
    float delta_t_ = 0.f;

    float chorus_mix_ = 0.5f;
    float input_mix_ = 0.5f;
};
} // namespace sfdsp