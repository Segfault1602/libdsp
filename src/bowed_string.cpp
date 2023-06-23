#include "bowed_string.h"

namespace dsp
{

void BowedString::Init(float samplerate)
{
    samplerate_ = samplerate;
    SetFrequency(220);

    reflection_filter_.SetGain(0.95f);
    reflection_filter_.SetPole(0.75f - (0.2f * 22050.0f / samplerate_));

    waveguide_.RightTermination.SetGain(-1.f);
    waveguide_.RightTermination.SetFilter(&reflection_filter_);

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
    float new_delay = (samplerate_ / freq_) * 0.5f;
    waveguide_.SetDelay(new_delay);
    // waveguide_.SetJunction(waveguide_.GetDelay() - new_delay);
    output_pickup_ = new_delay - 10.f;
}

float BowedString::GetVelocity() const
{
    return velocity_;
}

void BowedString::SetVelocity(float v)
{
    velocity_ = v;
}

float BowedString::GetForce() const
{
    return force_;
}

void BowedString::SetForce(float f)
{
    force_ = f;
}

void BowedString::Excite()
{
    float bow_velocity = velocity_ * 0.01f;

    constexpr float relative_position = 0.75f;
    float pos = waveguide_.GetDelay() * relative_position;
    float out = waveguide_.TapOut(pos);
    float deltaVelovity = bow_velocity - out;
    waveguide_.TapIn(pos, ComputeBowOutput(deltaVelovity));
}

void BowedString::Strike()
{
    constexpr float relative_position = 0.75f;
    float pos = std::floor(waveguide_.GetDelay() * relative_position);
    waveguide_.TapIn(pos, 0.95f);
}

float BowedString::Tick()
{
    waveguide_.Tick();
    constexpr float relative_position = 0.90f;
    float pos = waveguide_.GetDelay() * relative_position;
    float right, left;
    waveguide_.TapOut(pos, right, left);

    float out = 0.1248f * body_filters_[5].Tick(body_filters_[4].Tick(body_filters_[3].Tick(
                              body_filters_[2].Tick(body_filters_[1].Tick(body_filters_[0].Tick(right))))));
    return out;
}
} // namespace dsp