#include "bowed_string.h"

#include <cmath>

#include "window_functions.h"

namespace dsp
{

BowedString::BowedString() : waveguide_(1024, InterpolationType::Allpass)
{
}

void BowedString::Init(float samplerate)
{
    samplerate_ = samplerate;
    SetFrequency(220);

    reflection_filter_.SetGain(0.98f);
    reflection_filter_.SetPole(0.75f - (0.2f * 22050.0f / samplerate_));

    bridge_.SetGain(-1.f);
    bridge_.SetFilter(&reflection_filter_);

    // Body filter provided by Esteban Maestre (cascade of second-order sections)
    // https://github.com/thestk/stk/blob/cc2dd22e9752bf5fd94f0799e01d19d5e8399058/src/Bowed.cpp#L62
    body_filters_[0].SetCoefficients(1.0f, 1.5667f, 0.3133f, -0.5509f, -0.3925f);
    body_filters_[1].SetCoefficients(1.0f, -1.9537f, 0.9542f, -1.6357f, 0.8697f);
    body_filters_[2].SetCoefficients(1.0f, -1.6683f, 0.8852f, -1.7674f, 0.8735f);
    body_filters_[3].SetCoefficients(1.0f, -1.8585f, 0.9653f, -1.8498f, 0.9516f);
    body_filters_[4].SetCoefficients(1.0f, -1.9299f, 0.9621f, -1.9354f, 0.9590f);
    body_filters_[5].SetCoefficients(1.0f, -1.9800f, 0.9888f, -1.9867f, 0.9923f);
}

void BowedString::SetFrequency(float f)
{
    freq_ = f;
    float delay = (samplerate_ / freq_) * 0.5f;
    constexpr float bow_pos_ratio = 0.70f;
    waveguide_.SetJunction(0.f);
    waveguide_.SetDelay(delay);
    bow_position_ = std::floor(delay * bow_pos_ratio);
}

void BowedString::SetDelay(float delay)
{
    constexpr float bow_pos_ratio = 0.70f;
    waveguide_.SetJunction(0.f);
    waveguide_.SetDelay(delay);
    bow_position_ = std::floor(delay * bow_pos_ratio);

    freq_ = samplerate_ / (delay * 2);
}

float BowedString::GetFrequency() const
{
    return freq_;
}

void BowedString::SetLastMidiNote(float midi_note)
{
    last_midi_note_ = midi_note;
}

float BowedString::GetLastMidiNote() const
{
    return last_midi_note_;
}

float BowedString::GetVelocity() const
{
    return (velocity_ - velocity_offset_) / max_velocity_;
}

void BowedString::SetVelocity(float v)
{
    v = std::clamp(v, 0.f, 1.f);
    velocity_ = max_velocity_ * (velocity_offset_ + v);
}

void BowedString::SetForce(float f)
{
    bow_table_.SetForce(f);
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
    waveguide_.TapOut(bow_position_, vsl_plus, vsl_min);
    float vsr_plus, vsr_min;
    waveguide_.TapOut(bow_position_ + 1, vsr_min, vsr_plus);

    float bow_output = 0.f;
    if (note_on)
    {
        float velocity_delta = velocity_ - (vsl_plus + vsr_plus);
        bow_output = velocity_delta * bow_table_.Tick(velocity_delta);
    }

    waveguide_.TapIn(bow_position_, bow_output);
    waveguide_.Tick(nut_.Tick(nut), bridge_.Tick(bridge));

    // float out = 0.1248f * body_filters_[5].Tick(body_filters_[4].Tick(body_filters_[3].Tick(
    //                           body_filters_[2].Tick(body_filters_[1].Tick(body_filters_[0].Tick(bridge))))));
    return vsl_plus;
}
} // namespace dsp