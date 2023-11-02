#include "string_ensemble.h"

#include <span>

using namespace dsp;

void StringEnsemble::Init(float samplerate, const std::array<float, kStringCount>& frequencies)
{
    for (auto i = 0; i < kStringCount; ++i)
    {
        strings_[i].Init(samplerate, frequencies[i]);
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
}

void StringEnsemble::TuneStrings(uint8_t string_number, float frequencies)
{
    assert(string_number < kStringCount);
    strings_[string_number].SetFrequency(frequencies);
}

void StringEnsemble::SetFrequency(uint8_t string_number, float f)
{
    assert(string_number < kStringCount);
    strings_[string_number].SetFrequency(f);
}

float StringEnsemble::GetFrequency(uint8_t string_number) const
{
    assert(string_number < kStringCount);

    return strings_[string_number].GetFrequency();
}

void StringEnsemble::SetVelocity(uint8_t string_number, float v)
{
    assert(string_number < kStringCount);

    strings_[string_number].SetVelocity(v);
}

float StringEnsemble::GetVelocity(uint8_t string_number) const
{
    assert(string_number < kStringCount);

    return strings_[string_number].GetVelocity();
}

void StringEnsemble::SetForce(uint8_t string_number, float f)
{
    assert(string_number < kStringCount);

    strings_[string_number].SetForce(f);

    if (f > 0.f)
    {
        strings_[string_number].SetNoteOn(true);
    }
    else
    {
        strings_[string_number].SetNoteOn(false);
    }
}

float StringEnsemble::GetForce(uint8_t string_number) const
{
    assert(string_number < kStringCount);

    return strings_[string_number].GetForce();
}

void StringEnsemble::FingerOff(uint8_t string_number)
{
    assert(string_number < kStringCount);

    strings_[string_number].SetFrequency(openTuning_[string_number]);
}

float StringEnsemble::Tick()
{
    std::array<float, kStringCount> string_outs;

    constexpr float kTransmissionRatio = 0.10f;
    float transmission = 0.f;
    float output = 0.f;
    for (auto i = 0; i < kStringCount; ++i)
    {
        string_outs[i] = strings_[i].NextOut();
        output += string_outs[i];
        transmission += string_outs[i] * kTransmissionRatio;
        string_outs[i] *= (1.f - kTransmissionRatio);
    }

    // filter the bridge output
    transmission = transmission_filter_.Tick(transmission);

    for (size_t i = 0; i < kStringCount; ++i)
    {
        strings_[i].Tick(string_outs[i] + transmission * 0.25f);
    }

    float out = 0.1248f * body_filters_[5].Tick(body_filters_[4].Tick(body_filters_[3].Tick(
                              body_filters_[2].Tick(body_filters_[1].Tick(body_filters_[0].Tick(output))))));

    return out;
}
