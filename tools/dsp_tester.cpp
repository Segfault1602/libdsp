#include "dsp_tester.h"

constexpr float CHORUS_BASE_DELAY = 20.f;

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
    string_.Init(static_cast<float>(samplerate));
    delay_ = 300.f;
    string_.SetDelay(delay_);

    sample_per_delay_ = frame_count / 2000;
    delay_increment_ = 0.1f;
}

float WaveguideTester::Tick()
{
    if (++counter_ % sample_per_delay_ == 0 && delay_ > 20.f)
    {
        delay_ -= delay_increment_;
        string_.SetDelay(delay_);
        counter_ = 0;
    }
    return string_.Tick(&bow_table_);
}

float WaveguideTester::Tick(float)
{
    return Tick();
}
