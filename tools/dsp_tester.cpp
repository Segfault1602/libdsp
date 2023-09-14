#include "dsp_tester.h"

#include "basic_oscillators.h"

constexpr float CHORUS_BASE_DELAY = 20.f;

std::unique_ptr<DspTester> CreateTester(TesterType type)
{
    switch (type)
    {
    case TesterType::CHORUS:
        return std::make_unique<ChorusTester>();
    case TesterType::SIMPLE_BOWEDSTRING:
        return std::make_unique<SimpleBowedStringTester>();
    case TesterType::PITCHSLIDE_BOWEDSTRING:
        return std::make_unique<PitchSlideBowedStringTester>();
    case TesterType::OSCVELOCITY_BOWEDSTRING:
        return std::make_unique<OscVelocityBowedStringTester>();
    case TesterType::VIBRATO_BOWEDSTRING:
        return std::make_unique<VibratoBowedStringTester>();
    default:
        assert(false);
        return nullptr;
    }
}

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

void SimpleBowedStringTester::Init(size_t samplerate)
{
    samplerate_ = samplerate;

    // 6 seconds audio
    frame_count_ = samplerate * 6;
    midway_frame_ = frame_count_ / 2;
    current_frame_ = 0;
    current_velocity_ = 0.f;
    current_force_ = 0.f;
    param_delta_ = 1.f / static_cast<float>(midway_frame_);
    param_value_ = dsp::Line(0.f, 1.f, midway_frame_);

    string_.Init(static_cast<float>(samplerate));
    string_.SetFrequency(220.f);
    string_.SetForce(0.f);
    string_.SetVelocity(0.f);
}

// Velocity and Force will gradually increase over the first 3 seconds and then decrease until the end.
float SimpleBowedStringTester::Tick()
{
    if (current_frame_ == midway_frame_)
    {
        param_value_ = dsp::Line(1.f, 0.f, midway_frame_);
    }

    current_velocity_ = param_value_.Tick();
    current_force_ = current_velocity_;

    ++current_frame_;
    string_.SetVelocity(current_velocity_);
    string_.SetForce(current_force_);
    return string_.Tick(true);
}

float SimpleBowedStringTester::Tick(float)
{
    return Tick();
}

void OscVelocityBowedStringTester::Init(size_t samplerate)
{
    samplerate_ = samplerate;
    phase_dt_ = kFrequency / static_cast<float>(samplerate);
    string_.Init(static_cast<float>(samplerate));
    string_.SetFrequency(440.f);
    string_.SetForce(0.f);
    string_.SetVelocity(0.f);
}

float OscVelocityBowedStringTester::Tick()
{
    float param = (dsp::Tri(phase_) + 1) * 0.5f;
    phase_ += phase_dt_;
    if (phase_ >= 1.f)
    {
        phase_ -= 1.f;
    }

    string_.SetVelocity(param);
    string_.SetForce(param);
    return string_.Tick(true);
}

float OscVelocityBowedStringTester::Tick(float)
{
    return Tick();
}

void PitchSlideBowedStringTester::Init(size_t samplerate)
{
    samplerate_ = samplerate;

    frame_count_ = samplerate * 1;
    param_value_ = dsp::Line(440.f, 880.f, frame_count_);

    string_.Init(static_cast<float>(samplerate));
    string_.SetFrequency(20.f);
    string_.SetForce(0.5f);
    string_.SetVelocity(0.5f);
}

// Velocity and Force will gradually increase over the first 3 seconds and then decrease until the end.
float PitchSlideBowedStringTester::Tick()
{
    string_.SetFrequency(param_value_.Tick());
    return string_.Tick(true);
}

float PitchSlideBowedStringTester::Tick(float)
{
    return Tick();
}

void VibratoBowedStringTester::Init(size_t samplerate)
{
    samplerate_ = samplerate;

    frame_count_ = samplerate * 5;
    phase_dt_ = kVibratoFrequency / static_cast<float>(samplerate);

    string_.Init(static_cast<float>(samplerate));
    string_.SetFrequency(kFrequency);
    string_.SetForce(0.5f);
    string_.SetVelocity(0.5f);
}

// Velocity and Force will gradually increase over the first 3 seconds and then decrease until the end.
float VibratoBowedStringTester::Tick()
{
    float freq = kFrequency + (dsp::Sine(phase_) * kVibratoDepth);
    phase_ += phase_dt_;
    if (phase_ >= 1.f)
    {
        phase_ -= 1.f;
    }

    string_.SetFrequency(freq);
    return string_.Tick(true);
}

float VibratoBowedStringTester::Tick(float)
{
    return Tick();
}