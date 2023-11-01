#include "phaseshapers_tests.h"

void PhaseShaperTest::Init(size_t samplerate)
{
    samplerate_ = samplerate;
    phaseshaper_.Init(static_cast<float>(samplerate));
    phaseshaper_.SetWaveform(current_waveform_);
    phaseshaper_.SetMod(0.f);
    phaseshaper_.SetFreq(220.f);

    frame_per_wave_ = (samplerate / 220) * 8;
    // 4 periods per waveform
    frame_count_ = frame_per_wave_ * static_cast<size_t>(dsp::Phaseshaper::Waveform::NUM_WAVES);
    name_ = "phaseshaper.wav";
}

float PhaseShaperTest::Tick()
{
    ++current_frame_;

    if (current_frame_ == frame_per_wave_)
    {
        current_frame_ = 0;
        ++current_waveform_;
        phaseshaper_.SetWaveform(current_waveform_);
    }
    return phaseshaper_.Process();
}

float PhaseShaperTest::Tick(float input)
{
    return Tick();
}