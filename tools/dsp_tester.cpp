#include "dsp_tester.h"

#include "basic_oscillators.h"
#include "dsp_base.h"

constexpr float CHORUS_BASE_DELAY = 20.f;

void DspTester::SetFrameCount(uint32_t frame_count)
{
    frame_count_ = frame_count;
}

uint32_t DspTester::GetFrameCount() const
{
    return frame_count_;
}

ChorusTester::ChorusTester() : chorus_(4096)
{
}

void ChorusTester::Init(size_t samplerate)
{
    samplerate_ = samplerate;
    frame_count_ = samplerate * DEFAULT_FRAME_COUNT_RATIO;
    chorus_.Init(static_cast<uint32_t>(samplerate), CHORUS_BASE_DELAY);
    name_ = "chorus.wav";
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