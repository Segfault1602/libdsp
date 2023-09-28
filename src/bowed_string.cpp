#include "bowed_string.h"

#include <cmath>
#include <vector>

#include "basic_oscillators.h"
#include "window_functions.h"

// void PrintWaveguide(dsp::Waveguide& wave, size_t delay_size)
// {
//     std::vector<float> right_samples, left_samples;
//     for (size_t i = 1; i <= delay_size; ++i)
//     {
//         float right, left;
//         wave.TapOut(i, right, left);
//         right_samples.push_back(right);
//         left_samples.push_back(left);
//     }

//     for (auto sample : right_samples)
//     {
//         printf("%s%5.3f ", sample >= 0 ? " " : "", sample);
//     }
//     printf("\n");

//     for (auto sample : left_samples)
//     {
//         printf("%s%5.3f ", sample >= 0 ? " " : "", sample);
//     }
//     printf("\n");
// }

namespace dsp
{

BowedString::BowedString() : waveguide_(1024, InterpolationType::Linear), bow_output_rms_(128)
{
}

void BowedString::Init(float samplerate)
{
    samplerate_ = samplerate;
    SetFrequency(220);

    reflection_filter_.SetGain(0.95f);
    reflection_filter_.SetPole(0.75f - (0.2f * 22050.0f / samplerate_));

    bridge_.SetGain(-1.f);
    waveguide_.SetDelay(1024);
    bridge_.SetFilter(&reflection_filter_);
}

void BowedString::SetFrequency(float f)
{
    freq_ = f;
    float delay = (samplerate_ / freq_) * 0.5f;
    waveguide_.SetJunctionDelay(delay);
    SetBowPosition(relative_bow_position_);
}

void BowedString::SetDelay(float delay)
{
    waveguide_.SetJunctionDelay(delay);
    SetBowPosition(relative_bow_position_);

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
    velocity_ = velocity_offset_ + max_velocity_ * v;
}

void BowedString::SetForce(float f)
{
    bow_table_.SetForce(f);
}

float BowedString::GetForce() const
{
    return bow_table_.GetForce();
}

void BowedString::SetBowPosition(float pos)
{
    relative_bow_position_ = pos;

    bow_position_ = waveguide_.GetJunctionDelay() * relative_bow_position_;

    if (bow_position_ <= 1.f)
    {
        bow_position_ += 1.f;
    }
    else if (bow_position_ > waveguide_.GetJunctionDelay() - 2.f)
    {
        bow_position_ = waveguide_.GetJunctionDelay() - 2.f;
    }

    bow_position_ = std::ceil(bow_position_);
}

float BowedString::GetBowPosition() const
{
    return relative_bow_position_;
}

void BowedString::Pluck()
{
    float L = waveguide_.GetJunctionDelay();
    for (float i = 1; i < static_cast<float>(L); ++i)
    {
        waveguide_.TapIn(i, Hann(i - 1, L));
    }
}

float BowedString::Tick(bool note_on)
{
    float bridge, nut;
    waveguide_.NextOut(nut, bridge);

    float vsl_plus, vsr_plus;
    waveguide_.TapOut(bow_position_, vsl_plus, vsr_plus, &bow_interpolation_strategy_);

    float bow_output = 0.f;
    if (note_on)
    {
        float velocity_delta = velocity_ - (vsl_plus + vsr_plus);
        bow_output = velocity_delta * bow_table_.Tick(velocity_delta);

        // float rms = bow_output_rms_.Tick(bow_output);
        // float additive_noise = Noise() * rms * 0.1f;
        // bow_output += additive_noise;
    }

    waveguide_.TapIn(bow_position_, bow_output);
    waveguide_.Tick(bridge_.Tick(bridge), nut_.Tick(nut));

    return bridge;
}
} // namespace dsp