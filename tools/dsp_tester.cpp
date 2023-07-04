#include "dsp_tester.h"

constexpr float CHORUS_BASE_DELAY = 20.f;

void ChorusTester::Init(size_t samplerate)
{
    chorus_.Init(samplerate, CHORUS_BASE_DELAY);
}

float ChorusTester::Tick()
{
    // This is an effect that requires audio input
    assert(false);
    return 0.f;
}

float ChorusTester::Tick(float input)
{
    return chorus_.Tick(input);
}

void WaveguideTester::Init(size_t samplerate)
{
    string_.Init(static_cast<float>(samplerate));
    string_.SetFrequency(220);
}

float WaveguideTester::Tick()
{
    return string_.Tick();
}

float WaveguideTester::Tick(float)
{
    return Tick();
}
