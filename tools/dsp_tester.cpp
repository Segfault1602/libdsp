#include "dsp_tester.h"

constexpr float CHORUS_BASE_DELAY = 20.f;

ChorusTester::ChorusTester() : chorus_(4096)
{
}

void ChorusTester::Init(size_t samplerate, uint32_t frame_count)
{
    samplerate_ = samplerate;
    frame_count_ = frame_count;
    chorus_.Init(static_cast<uint32_t>(samplerate), CHORUS_BASE_DELAY);
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

void WaveguideTester::Init(size_t samplerate, uint32_t frame_count)
{
    samplerate_ = samplerate;
    frame_count_ = frame_count;
    delay_ = 100;
    string_.Init(static_cast<float>(samplerate));
    string_.SetFrequency(440.f);
    // string_.SetDelay(10.5);
    string_.SetForce(0.5f);
    string_.SetVelocity(0.5f);
    // string_.Pluck();
}

float WaveguideTester::Tick()
{
    return string_.Tick(true);
}

float WaveguideTester::Tick(float)
{
    return Tick();
}
