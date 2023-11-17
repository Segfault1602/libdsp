#include "bowed_string.h"

#include <cmath>
#include <vector>

#include "basic_oscillators.h"
#include "window_functions.h"

namespace sfdsp
{

static constexpr float kMaxBridgeFilterCutoff = 0.10f;
static constexpr float kBridgeFilterCutoffOffset = 0.025f;

static constexpr float max_velocity_ = 0.2f;
static constexpr float velocity_offset_ = 0.03f;

BowedString::BowedString(size_t max_size) : waveguide_(max_size, InterpolationType::Linear), gate_(true, 0.f, 1.f)
{
}

void BowedString::Init(const BowedStringConfig& config)
{
    samplerate_ = config.samplerate;
    SetFrequency(config.open_string_tuning); // open string

    if (config.bridge_filter.has_value())
    {
        reflection_filter_ = config.bridge_filter.value();
    }
    else
    {
        reflection_filter_.SetGain(0.95f);
        reflection_filter_.SetPole(0.75f - (0.2f * 22050.0f / samplerate_));
    }

    bridge_.SetGain(1.f);

    float string_length = (samplerate_ / config.open_string_tuning) * 0.5f;
    open_string_delay_ = string_length - 1.f;
    waveguide_.SetDelay(open_string_delay_);
    bridge_.SetFilter(&reflection_filter_);

    nut_.SetGain(config.nut_gain);

    // Decay filter for add. noise
    constexpr float decayDb = -12.f;
    constexpr float timeMs = 20.f;
    decay_filter_.SetDecayFilter(decayDb, timeMs, config.samplerate);
    decay_filter_.SetGain(1.f);

    noise_lp_filter_.SetPole(0.8f);

    velocity_.Init(samplerate_, SmoothParam::SmoothingType::Exponential);
    bow_force_.Init(samplerate_, SmoothParam::SmoothingType::Exponential);
    gate_delay_.Init(samplerate_, SmoothParam::SmoothingType::Exponential);
}

void BowedString::SetFrequency(float f)
{
    freq_ = f;
    float delay = (samplerate_ / freq_) * 0.5f;
    delay -= 1.f; // delay compensation, tuned by ear
    delay += tuning_adjustment_;
    gate_delay_.SetTarget(delay);
    SetBowPosition(relative_bow_position_);

    // retune open string
    waveguide_.SetDelay(open_string_delay_ + tuning_adjustment_);
}

float BowedString::GetFrequency() const
{
    return freq_;
}

void BowedString::Pluck()
{
    float L = gate_.GetDelay();
    for (size_t i = 1; i < static_cast<size_t>(L); ++i)
    {
        waveguide_.TapIn(static_cast<float>(i), Hann(static_cast<float>(i) - 1.f, L));
    }
}

float BowedString::NextOut()
{
    float bridge = 0.f;
    float nut = 0.f;
    waveguide_.NextOut(nut, bridge);
    return bridge;
}

float BowedString::Tick(float input)
{
    gate_.SetDelay(gate_delay_.Tick());

    float vel = velocity_.Tick();
    bow_table_.SetForce(bow_force_.Tick());

    float bridge = 0.f;
    float nut = 0.f;
    waveguide_.NextOut(nut, bridge);

    float vsl_plus = 0.f;
    float vsr_plus = 0.f;
    waveguide_.TapOut(bow_position_, vsl_plus, vsr_plus, &bow_interpolation_strategy_);

    float bow_output = 0.f;
    if (note_on_)
    {
        float velocity_delta = vel - (vsl_plus + vsr_plus);
        constexpr float noise_db = -30;
        const float noise_gain = std::pow(10.f, noise_db / 20.f);

        float env = std::sqrt(decay_filter_.Tick(velocity_delta * velocity_delta));
        float additive_noise = noise_lp_filter_.Tick(Noise()) * env * noise_gain;

        bow_output = (velocity_delta + additive_noise) * bow_table_.Tick(velocity_delta + additive_noise);
    }

    waveguide_.TapIn(bow_position_, bow_output);

    gate_.Process(waveguide_);
    waveguide_.Tick(bridge_.Tick(-input), nut_.Tick(nut));

    return bridge;
}

void BowedString::SetParameter(ParamId param_id, float value)
{
    assert(value >= 0.f && value <= 1.f);
    switch (param_id)
    {
    case ParamId::Velocity:
        SetVelocity(value);
        break;
    case ParamId::Force:
        SetForce(value);
        break;
    case ParamId::BowPosition:
        SetBowPosition(value);
        break;
    case ParamId::FingerPressure:
        gate_.SetCoeff(value);
        break;
    case ParamId::NutGain:
        value = 0.90f + value * 0.10f;
        nut_.SetGain(-value);
        break;
    case ParamId::BridgeFilterCutoff:
        value = kBridgeFilterCutoffOffset + value * kMaxBridgeFilterCutoff;
        reflection_filter_.SetLowpass(value);
        break;
    case ParamId::TuningAdjustment:
        value = value * 10.f - 5.f;
        tuning_adjustment_ = value;
        SetFrequency(freq_);
        break;
    }
}

void BowedString::SetVelocity(float v)
{
    v = std::clamp(v, -1.f, 1.f);
    velocity_.SetTarget(velocity_offset_ + max_velocity_ * v);
}

void BowedString::SetForce(float f)
{
    note_on_ = f > 0.01f;

    bow_force_.SetTarget(f);
}

void BowedString::SetBowPosition(float pos)
{
    relative_bow_position_ = pos;

    // The gate delay is where the 'finger' is. We want the bow position to be relative to that.
    float bow_pos = gate_delay_.GetTarget() * relative_bow_position_;

    if (bow_pos <= 1.f)
    {
        bow_pos += 1.f;
    }
    else if (bow_pos > gate_delay_.GetTarget() - 2.f)
    {
        bow_pos = gate_delay_.GetTarget() - 2.f;
    }

    assert(bow_pos > 0);
    bow_position_ = std::ceil(bow_pos);
}

} // namespace sfdsp