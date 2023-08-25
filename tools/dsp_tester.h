#pragma once

#include "dsp_base.h"

#include "bowed_string.h"
#include "chorus.h"

class DspTester
{
  public:
    DspTester() = default;
    virtual ~DspTester() = default;

    virtual void Init(size_t samplerate, uint32_t frame_count) = 0;
    virtual float Tick() = 0;
    virtual float Tick(float input) = 0;

    size_t GetSamplerate() const
    {
        return samplerate_;
    }

    uint32_t GetFrameCount() const
    {
        return frame_count_;
    }

  protected:
    size_t samplerate_ = 48000;
    uint32_t frame_count_ = 0;
};

class ChorusTester : public DspTester
{
  public:
    ChorusTester();
    ~ChorusTester() override = default;

    void Init(size_t samplerate, uint32_t frame_count) override;
    float Tick() override;
    float Tick(float input) override;

  private:
    dsp::Chorus chorus_;
};

class WaveguideTester : public DspTester
{
  public:
    WaveguideTester() = default;
    ~WaveguideTester() override = default;

    void Init(size_t samplerate, uint32_t frame_count) override;
    float Tick() override;
    float Tick(float input) override;

  private:
    dsp::BowedString string_;

    float delay_ = 0.f;
    uint32_t sample_per_delay_ = 0;
    float delay_increment_ = 0.01f;
    uint32_t counter_ = 0;
};