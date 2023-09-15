#include "bowed_string.h"

#include <cmath>

#include "window_functions.h"

namespace dsp
{

BowedString::BowedString() : waveguide_(1024, InterpolationType::Linear)
{
}

void BowedString::Init(float samplerate)
{
    samplerate_ = samplerate;
    SetFrequency(220);

    reflection_filter_.SetGain(0.98f);
    reflection_filter_.SetPole(0.75f - (0.2f * 22050.0f / samplerate_));

    bridge_.SetGain(-1.f);
    waveguide_.SetDelay(1024);
    bridge_.SetFilter(&reflection_filter_);
}

void BowedString::SetFrequency(float f)
{
    freq_ = f;
    float delay = (samplerate_ / freq_) * 0.5f;
    constexpr float bow_pos_ratio = 0.70f;
    waveguide_.SetJunction(delay);
    bow_position_ = delay * bow_pos_ratio;
}

void BowedString::SetDelay(float delay)
{
    constexpr float bow_pos_ratio = 0.70f;
    waveguide_.SetJunction(delay);
    // waveguide_.SetDelay(delay);
    bow_position_ = delay * bow_pos_ratio;

    freq_ = samplerate_ / (delay * 2);
}

float BowedString::GetFrequency() const
{
    return freq_;
}

float BowedString::GetVelocity() const
{
    return (velocity_ - velocity_offset_) / max_velocity_;
}

void BowedString::SetVelocity(float v)
{
    v = std::clamp(v, -1.f, 1.f);
    velocity_ = max_velocity_ * (velocity_offset_ + v);
}

void BowedString::SetForce(float f)
{
    bow_table_.SetForce(f);
}

float BowedString::GetForce() const
{
    return bow_table_.GetForce();
}

void BowedString::Pluck()
{
    float L = waveguide_.GetDelay();
    for (float i = 1; i < static_cast<float>(L); ++i)
    {
        waveguide_.TapIn(i, Hann(i - 1, L));
    }
}

float BowedString::Tick(bool note_on)
{
    float bridge, nut;
    waveguide_.NextOut(nut, bridge);

    float vsl_plus, vsl_min;
    waveguide_.TapOut(bow_position_, vsl_min, vsl_plus, &bow_interpolation_strategy_);
    float vsr_plus, vsr_min;
    waveguide_.TapOut(bow_position_ + 1, vsr_plus, vsr_min, &bow_interpolation_strategy_);

    float bow_output = 0.f;
    if (note_on)
    {
        float velocity_delta = velocity_ - (vsl_plus + vsr_plus);
        bow_output = velocity_delta * bow_table_.Tick(velocity_delta);
    }

    waveguide_.TapIn(bow_position_, bow_output);
    waveguide_.Tick(nut_.Tick(nut), bridge_.Tick(bridge));

    return bridge;
}
} // namespace dsp