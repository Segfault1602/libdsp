#include "string_ensemble.h"

#include <span>

using namespace dsp;

void StringEnsemble::Init(float samplerate, const std::array<float, kStringCount>& frequencies)
{
    for (auto i = 0; i < kStringCount; ++i)
    {
        strings_[i].Init(samplerate);
        strings_[i].SetFrequency(frequencies[i]);
    }

    transmission_filter_.SetPole(0.6f);
    transmission_filter_.SetGain(1.f);
}

void StringEnsemble::TuneStrings(uint8_t string_num, float frequencies)
{
    strings_[string_num].SetFrequency(frequencies);
}

void StringEnsemble::SetFrequency(uint8_t string_number, float f)
{
    strings_[string_number].SetFrequency(f);
}

float StringEnsemble::GetFrequency(uint8_t string_number) const
{
    return strings_[string_number].GetFrequency();
}

void StringEnsemble::SetVelocity(uint8_t string_number, float v)
{
    strings_[string_number].SetVelocity(v);
}

float StringEnsemble::GetVelocity(uint8_t string_number) const
{
    return strings_[string_number].GetVelocity();
}

void StringEnsemble::SetForce(uint8_t string_number, float f)
{
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
    return strings_[string_number].GetForce();
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

    return output;
}
