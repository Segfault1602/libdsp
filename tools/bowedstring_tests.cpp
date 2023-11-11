#include "bowedstring_tests.h"

#include "basic_oscillators.h"

void SimpleBowedString::Init(size_t samplerate)
{
    samplerate_ = samplerate;
    string_.Init(static_cast<float>(samplerate));
    string_.SetFrequency(220.f);
    string_.SetForce(0.5f);
    string_.SetVelocity(1.f);
    string_.SetNoteOn(true);

    name_ = "bowedstring.wav";
}

float SimpleBowedString::Tick()
{
    float bridge = string_.NextOut();
    return string_.Tick(bridge);
}

float SimpleBowedString::Tick(float)
{
    return Tick();
}

void CrescendoBowedStringTester::Init(size_t samplerate)
{
    samplerate_ = samplerate;

    // 6 seconds audio
    frame_count_ = samplerate * 6;
    midway_frame_ = frame_count_ / 2;
    current_frame_ = 0;
    current_velocity_ = 0.f;
    current_force_ = 0.f;
    param_delta_ = 1.f / static_cast<float>(midway_frame_);
    param_value_ = sfdsp::Line(0.f, 1.f, midway_frame_);

    string_.Init(static_cast<float>(samplerate));
    string_.SetFrequency(440.f);
    string_.SetForce(0.f);
    string_.SetVelocity(0.f);
    string_.SetNoteOn(true);

    name_ = "crescendo_bowedstring.wav";
}

// Velocity and Force will gradually increase over the first 3 seconds and then decrease until the end.
float CrescendoBowedStringTester::Tick()
{
    if (current_frame_ == midway_frame_)
    {
        param_value_ = sfdsp::Line(1.f, 0.f, midway_frame_);
    }

    current_velocity_ = param_value_.Tick();
    current_force_ = current_velocity_;

    ++current_frame_;
    string_.SetVelocity(current_velocity_);
    string_.SetForce(current_force_);
    float bridge = string_.NextOut();
    return string_.Tick(bridge);
}

float CrescendoBowedStringTester::Tick(float)
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
    string_.SetNoteOn(true);

    name_ = "bowedstring_oscvelocity.wav";
}

float OscVelocityBowedStringTester::Tick()
{
    float param = (sfdsp::Sine(phase_) + 1) * 2.f;
    param = std::clamp(param, 0.f, 1.f);
    phase_ += phase_dt_;
    if (phase_ >= 1.f)
    {
        phase_ -= 1.f;
    }

    string_.SetVelocity(param);
    string_.SetForce(param);

    float bridge = string_.NextOut();
    return string_.Tick(bridge);
}

float OscVelocityBowedStringTester::Tick(float)
{
    return Tick();
}

void PitchSlideBowedStringTester::Init(size_t samplerate)
{
    samplerate_ = samplerate;

    frame_count_ = samplerate * 4;
    constexpr float kStartNote = 76;
    constexpr float kEndNote = 69;
    param_value_ = sfdsp::Line(kStartNote, kEndNote, samplerate / 2);

    string_.Init(static_cast<float>(samplerate));
    string_.SetFrequency(sfdsp::FastMidiToFreq(kStartNote));
    string_.SetForce(0.5f);
    string_.SetVelocity(0.5f);
    string_.SetNoteOn(true);

    current_frame_ = 0;
    name_ = "bowedstring_pitchslide.wav";
}

// Velocity and Force will gradually increase over the first 3 seconds and then decrease until the end.
float PitchSlideBowedStringTester::Tick()
{
    // Stay on the first note for one second before the glissando
    if (current_frame_ > samplerate_)
    {
        string_.SetFrequency(sfdsp::FastMidiToFreq(param_value_.Tick()));
    }
    ++current_frame_;

    float bridge = string_.NextOut();
    return string_.Tick(bridge);
}

float PitchSlideBowedStringTester::Tick(float)
{
    return Tick();
}

void VibratoBowedStringTester::Init(size_t samplerate)
{
    samplerate_ = samplerate;

    frame_count_ = samplerate * 3;
    phase_dt_ = kVibratoFrequency / static_cast<float>(samplerate);

    string_.Init(static_cast<float>(samplerate));
    string_.SetFrequency(kFrequency);
    string_.SetForce(0.5f);
    string_.SetVelocity(0.5f);
    string_.SetNoteOn(true);

    name_ = "bowedstring_vibrato.wav";
}

// Velocity and Force will gradually increase over the first 3 seconds and then decrease until the end.
float VibratoBowedStringTester::Tick()
{
    float freq = kFrequency + (sfdsp::Sine(phase_) * kVibratoDepth);
    phase_ += phase_dt_;

    string_.SetFrequency(freq);
    float bridge = string_.NextOut();
    return string_.Tick(bridge);
}

float VibratoBowedStringTester::Tick(float)
{
    return Tick();
}

void ScaleBowedStringTester::Init(size_t samplerate)
{
    samplerate_ = samplerate;

    vel_phase_dt_ = kVelSpeed / static_cast<float>(samplerate);
    frame_per_note_ = static_cast<float>(samplerate_) / kVelSpeed;

    frame_count_ = samplerate * 4;
    vib_phase_dt_ = kVibratoFrequency / static_cast<float>(samplerate);
    vib_phase_ = 0.f;

    arp_.SetNotes({60, 62, 64, 65, 67, 69, 71, 72, 71, 69, 67, 65, 64, 62});

    string_.Init(static_cast<float>(samplerate));
    string_.SetFrequency(sfdsp::midi_to_freq[static_cast<size_t>(arp_.Next())]);
    string_.SetForce(0.5f);
    string_.SetVelocity(0.5f);
    string_.SetNoteOn(true);

    name_ = "bowedstring_scale.wav";
}

// Velocity and Force will gradually increase over the first 3 seconds and then decrease until the end.
float ScaleBowedStringTester::Tick()
{
    float vel_param = (sfdsp::Tri(vel_phase_) + 1) * 2.f;
    vel_param = std::clamp(vel_param, 0.f, 1.f);
    vel_phase_ += vel_phase_dt_;
    if (vel_phase_ >= 1.f)
    {
        vel_phase_ -= 1.f;
    }

    string_.SetVelocity(vel_param);

    float vibrato = (sfdsp::Sine(vib_phase_) * kVibratoDepth);
    vib_phase_ += vib_phase_dt_;
    if (vib_phase_ >= 1.f)
    {
        vib_phase_ -= 1.f;
    }
    (void)vibrato;

    if (current_frame_ >= frame_per_note_)
    {
        current_frame_ -= frame_per_note_;
        float freq = sfdsp::FastMidiToFreq(arp_.Next());
        string_.SetFrequency(freq);
    }

    ++current_frame_;
    float bridge = string_.NextOut();
    return string_.Tick(bridge);
}

float ScaleBowedStringTester::Tick(float)
{
    return Tick();
}

void FingerPressBowedStringTester::Init(size_t samplerate)
{
    samplerate_ = samplerate;
    frame_count_ = samplerate * 4;
    string_.Init(static_cast<float>(samplerate), 220.f);
    string_.SetFrequency(440.f);
    string_.SetForce(0.8f);
    string_.SetVelocity(0.7f);
    string_.SetNoteOn(true);
    string_.SetFingerPressure(0.f);

    constexpr size_t press_time_ms = 10;
    const size_t press_time_sample = press_time_ms * samplerate / 1000;

    pressure_ = sfdsp::Line(0.f, 0.2f, press_time_sample);

    name_ = "fingerpress.wav";
}

float FingerPressBowedStringTester::Tick()
{
    ++current_frame_;

    if (current_frame_ > samplerate_)
    {
        string_.SetFingerPressure(pressure_.Tick());
    }

    if (current_frame_ == samplerate_ * 2)
    {
        constexpr size_t depress_time = 1;
        pressure_ = sfdsp::Line(0.2f, 0.f, depress_time);
        string_.SetBowPosition(0.30f);
    }

    float bridge = string_.NextOut();
    return string_.Tick(bridge);
}

float FingerPressBowedStringTester::Tick(float)
{
    return Tick();
}