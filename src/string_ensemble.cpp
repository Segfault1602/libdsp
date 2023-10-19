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

    transmission_filter_.SetPole(0.9);
    transmission_filter_.SetGain(0.1);
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
    float output = 0.f;
    for (auto i = 0; i < kStringCount; ++i)
    {
        string_outs[i] = strings_[i].NextOut();
        output += string_outs[i];
    }

    // filter the bridge output
    std::array<float, kStringCount> scattered_out = Scatter(string_outs, 0.5f);

    for (size_t i = 0; i < kStringCount; ++i)
    {
        strings_[i].Tick(scattered_out[i]);
    }

    return output;
}
