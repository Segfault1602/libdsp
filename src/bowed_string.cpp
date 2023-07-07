#include "bowed_string.h"

namespace dsp
{

void BowedString::Init(float samplerate)
{
    samplerate_ = samplerate;
    SetFrequency(220);

    reflection_filter_.SetGain(0.95f);
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
    float total_delay = (samplerate_ / freq_) * 0.5f;
    constexpr float bow_pos_ratio = 0.10f;
    // Todo: fix this with relay delay length
    // nut_to_bow_.SetDelay(100.f);
    nut_to_bow_.SetJunction(4096.f - (total_delay * (1.f - bow_pos_ratio)));
    bow_to_bridge_.SetDelay(total_delay * bow_pos_ratio);
    // float new_delay = (samplerate_ / freq_) * 0.5f;
    // waveguide_.SetDelay(new_delay);
    // // waveguide_.SetJunction(waveguide_.GetDelay() - new_delay);
    // output_pickup_ = new_delay - 10.f;
}

float BowedString::GetVelocity() const
{
    return velocity_;
}

void BowedString::SetVelocity(float v)
{
    velocity_ = v;
}

void BowedString::Strike()
{
    constexpr float relative_position = 0.75f;
    float pos = std::floor(waveguide_.GetDelay() * relative_position);
    waveguide_.TapIn(pos, 0.95f);
}

float BowedString::Tick(const ExcitationModel* excitation_model)
{
    float nut_into_bow, bow_into_nut;
    nut_to_bow_.NextOut(nut_into_bow, bow_into_nut);

    float bow_into_bridge, bridge_into_bow;
    bow_to_bridge_.NextOut(bow_into_bridge, bridge_into_bow);

    float bow_output = 0.f;
    if (excitation_model)
    {
        float velocity_delta = velocity_ - (nut_into_bow + bridge_into_bow);
        bow_output = velocity_delta * excitation_model->Tick(velocity_delta);
    }

    nut_to_bow_.Tick(bow_output + bridge_into_bow, nut_.Tick(bow_into_nut));
    bow_to_bridge_.Tick(bridge_.Tick(bow_into_bridge), bow_output + nut_into_bow);

    float out = 0.1248f * body_filters_[5].Tick(body_filters_[4].Tick(body_filters_[3].Tick(
                              body_filters_[2].Tick(body_filters_[1].Tick(body_filters_[0].Tick(bow_into_bridge))))));
    return out;
}
} // namespace dsp