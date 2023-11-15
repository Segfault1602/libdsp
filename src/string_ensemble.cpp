#include "string_ensemble.h"

using namespace sfdsp;

static constexpr float kMaxBridgeTransmission = 0.20f;

void StringEnsemble::Init(float samplerate, const std::array<float, kStringCount>& frequencies)
{
    for (size_t i = 0; i < kStringCount; ++i)
    {
        sfdsp::BowedStringConfig config = sfdsp::kDefaultStringConfig;
        config.samplerate = samplerate;
        config.open_string_tuning = frequencies[i];

        strings_[i].Init(config);
    }

    openTuning_ = frequencies;

    transmission_filter_.SetPole(0.6f);
    transmission_filter_.SetGain(1.f);

    // Body filter provided by Esteban Maestre (cascade of second-order sections)
    // https://github.com/thestk/stk/blob/cc2dd22e9752bf5fd94f0799e01d19d5e8399058/src/Bowed.cpp#L62
    body_filters_[0].SetCoefficients(1.0f, 1.5667f, 0.3133f, -0.5509f, -0.3925f);
    body_filters_[1].SetCoefficients(1.0f, -1.9537f, 0.9542f, -1.6357f, 0.8697f);
    body_filters_[2].SetCoefficients(1.0f, -1.6683f, 0.8852f, -1.7674f, 0.8735f);
    body_filters_[3].SetCoefficients(1.0f, -1.8585f, 0.9653f, -1.8498f, 0.9516f);
    body_filters_[4].SetCoefficients(1.0f, -1.9299f, 0.9621f, -1.9354f, 0.9590f);
    body_filters_[5].SetCoefficients(1.0f, -1.9800f, 0.9888f, -1.9867f, 0.9923f);
    body_filters_[5].SetGain(0.1248f);
}

void StringEnsemble::TuneStrings(uint8_t string_number, float frequencies)
{
    assert(string_number < kStringCount);
    strings_[string_number].SetFrequency(frequencies);
}

void StringEnsemble::FingerOff(uint8_t string_number)
{
    assert(string_number < kStringCount);

    strings_[string_number].SetFrequency(openTuning_[string_number]);
}

void StringEnsemble::SetBridgeTransmission(float t)
{
    assert(t >= 0.f && t <= 1.f);

    bridgeTransmission_ = t * kMaxBridgeTransmission;
}

float StringEnsemble::GetBridgeTransmission() const
{
    return bridgeTransmission_;
}

float StringEnsemble::Tick()
{
    float out = 0;
    ProcessBlock(&out, 1);
    return out;
}

void StringEnsemble::ProcessBlock(float* out, size_t size)
{
    assert(out != nullptr);

    for (size_t i = 0; i < size; ++i)
    {
        std::array<float, kStringCount> string_outs = {0.f};

        float transmission = 0.f;
        for (size_t j = 0; j < kStringCount; ++j)
        {
            string_outs[j] = strings_[j].NextOut();
            out[i] += string_outs[j];
            transmission += string_outs[j] * bridgeTransmission_;
            string_outs[j] *= (1.f - bridgeTransmission_);
        }

        // filter the bridge output
        transmission = transmission_filter_.Tick(transmission) * 0.25f;

        for (size_t j = 0; j < kStringCount; ++j)
        {
            strings_[j].Tick(string_outs[j] + transmission);
        }
    }

    // filter the body output
    // for (size_t i = 0; i < body_filters_.size(); ++i)
    // {
    //     body_filters_[i].ProcessBlock(out, out, size);
    // }
}

const BowedString& StringEnsemble::operator[](uint8_t string_number) const
{
    assert(string_number < kStringCount);

    return strings_[string_number];
}

BowedString& StringEnsemble::operator[](uint8_t string_number)
{
    assert(string_number < kStringCount);

    return strings_[string_number];
}

void StringEnsemble::SetParameter(ParamId param_id, float value)
{
    switch (param_id)
    {
    case ParamId::BridgeTransmission:
        SetBridgeTransmission(value);
        break;
    case ParamId::BridgeTransmissionFilterCutoff:
        transmission_filter_.SetLowpass(value);
        break;
    default:
        break;
    }
}
