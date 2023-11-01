#pragma once

#include "arpegiator.h"
#include "dsp_tester.h"

class SimpleBowedString : public DspTester
{
  public:
    SimpleBowedString() = default;
    ~SimpleBowedString() override = default;

    void Init(size_t samplerate) override;
    float Tick() override;
    float Tick(float input) override;

  private:
    dsp::BowedString string_;
};

class CrescendoBowedStringTester : public DspTester
{
  public:
    CrescendoBowedStringTester() = default;
    ~CrescendoBowedStringTester() override = default;

    void Init(size_t samplerate) override;
    float Tick() override;
    float Tick(float input) override;

  private:
    dsp::BowedString string_;

    size_t midway_frame_ = 0;
    size_t current_frame_ = 0;

    float current_velocity_ = 0.f;
    float current_force_ = 0.f;
    float param_delta_ = 0.f;
    dsp::Line param_value_;
};

class PitchSlideBowedStringTester : public DspTester
{
  public:
    PitchSlideBowedStringTester() = default;
    ~PitchSlideBowedStringTester() override = default;

    void Init(size_t samplerate) override;
    float Tick() override;
    float Tick(float input) override;

  private:
    dsp::BowedString string_;
    dsp::Line param_value_;
    size_t current_frame_;
};

class OscVelocityBowedStringTester : public DspTester
{
  public:
    OscVelocityBowedStringTester() = default;
    ~OscVelocityBowedStringTester() override = default;

    void Init(size_t samplerate) override;
    float Tick() override;
    float Tick(float input) override;

  private:
    dsp::BowedString string_;
    const float kFrequency = 7.f;
    float phase_dt_ = 0.f;
    float phase_ = 0.f;
};

class VibratoBowedStringTester : public DspTester
{
  public:
    VibratoBowedStringTester() = default;
    ~VibratoBowedStringTester() override = default;

    void Init(size_t samplerate) override;
    float Tick() override;
    float Tick(float input) override;

  private:
    dsp::BowedString string_;
    const float kFrequency = 440.f;
    const float kVibratoFrequency = 5.f;
    const float kVibratoDepth = 0.7f;
    float phase_dt_ = 0.f;
    float phase_ = 0.f;
};

class ScaleBowedStringTester : public DspTester
{
  public:
    ScaleBowedStringTester() = default;
    ~ScaleBowedStringTester() override = default;

    void Init(size_t samplerate) override;
    float Tick() override;
    float Tick(float input) override;

  private:
    dsp::BowedString string_;
    const float kVelSpeed = 7.f;
    float vel_phase_dt_ = 0.f;
    float vel_phase_ = 0.f;

    float current_frame_ = 0.f;
    float frame_per_note_ = 0.f;
    Arpegiator arp_;

    const float kVibratoFrequency = 5.f;
    const float kVibratoDepth = 1.2f;
    float vib_phase_dt_ = 0.f;
    float vib_phase_ = 0.f;
};