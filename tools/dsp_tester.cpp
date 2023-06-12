#include "dsp_tester.h"

constexpr DspFloat CHORUS_BASE_DELAY = 20.f;

void ChorusTester::Init(size_t samplerate)
{
    chorus_.Init(samplerate, CHORUS_BASE_DELAY);
}

DspFloat ChorusTester::Tick()
{
    // This is an effect that requires audio input
    assert(false);
    return 0.f;
}

DspFloat ChorusTester::Tick(DspFloat input)
{
    return chorus_.Tick(input);
}

void WaveguideTester::Init(size_t samplerate)
{
    string_.Init(static_cast<DspFloat>(samplerate));
    string_.SetFrequency(440);
    string_.Excite();
}

DspFloat WaveguideTester::Tick()
{
    return string_.Tick();
}

DspFloat WaveguideTester::Tick(DspFloat)
{
    return string_.Tick();
}
