#include "phaseshapers_tests.h"

void PhaseShaperTest::Init(size_t samplerate)
{
    constexpr float kFrequency = 200.f;
    samplerate_ = samplerate;
    phaseshaper_.Init(static_cast<float>(samplerate));
    phaseshaper_.SetWaveform(current_waveform_);
    phaseshaper_.SetMod(0.5f);
    phaseshaper_.SetFreq(kFrequency);

    float period = samplerate_ / kFrequency;
    frame_per_wave_ = static_cast<uint32_t>(period) * 10;
    frame_count_ = frame_per_wave_ * static_cast<uint32_t>(sfdsp::Phaseshaper::Waveform::NUM_WAVES);
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

void PhaseShaperBlockTest::Init(size_t samplerate)
{
    constexpr float kFrequency = 200.f;
    samplerate_ = samplerate;
    phaseshaper_.Init(static_cast<float>(samplerate));
    phaseshaper_.SetWaveform(current_waveform_);
    phaseshaper_.SetMod(0.5f);
    phaseshaper_.SetFreq(kFrequency);

    float period = samplerate_ / kFrequency;
    frame_per_wave_ = static_cast<uint32_t>(period) * 10;
    frame_count_ = frame_per_wave_ * static_cast<uint32_t>(sfdsp::Phaseshaper::Waveform::NUM_WAVES);
    name_ = "phaseshaperblock.wav";
}

float PhaseShaperBlockTest::Tick()
{
    ++current_frame_;

    if (current_frame_ == frame_per_wave_)
    {
        current_frame_ = 0;
        ++current_waveform_;
        phaseshaper_.SetWaveform(current_waveform_);
    }
    float out = 0.f;
    phaseshaper_.ProcessBlock(&out, 1);
    return out;
}

float PhaseShaperBlockTest::Tick(float input)
{
    return Tick();
}

void VectorPhaseshaperTest::Init(size_t samplerate)
{
    constexpr float kFrequency = 200.f;
    samplerate_ = samplerate;
    phaseshaper_.Init(static_cast<float>(samplerate));
    phaseshaper_.SetFreq(kFrequency);
    phaseshaper_.SetMod(0.25f, 1.f);

    frame_count_ = samplerate * 5.f;

    lfo_.Init(static_cast<float>(samplerate), 2.f, sfdsp::OscillatorType::Sine);

    name_ = "vectorphaseshaper.wav";
}

float VectorPhaseshaperTest::Tick()
{
    float mod = 0.25f + lfo_.Tick() * 0.20f;
    phaseshaper_.SetMod(mod, 1.f);

    float out = 0.f;
    phaseshaper_.ProcessBlock(&out, 1);
    return out;
}

float VectorPhaseshaperTest::Tick(float input)
{
    return Tick();
}