#pragma once

#include "dsp_base.h"

#include "bowed_string.h"
#include "chorus.h"

class DspTester
{
  public:
    DspTester() = default;
    virtual ~DspTester() = default;

    virtual void Init(size_t samplerate) = 0;
    virtual DspFloat Tick() = 0;
    virtual DspFloat Tick(DspFloat input) = 0;
};

class ChorusTester : public DspTester
{
  public:
    ChorusTester() = default;
    ~ChorusTester() override = default;

    void Init(size_t samplerate) override;
    DspFloat Tick() override;
    DspFloat Tick(DspFloat input) override;

  private:
    dsp::Chorus<4096> chorus_;
};

class WaveguideTester : public DspTester
{
  public:
    WaveguideTester() = default;
    ~WaveguideTester() override = default;

    void Init(size_t samplerate) override;
    DspFloat Tick() override;
    DspFloat Tick(DspFloat input) override;

  private:
    dsp::BowedString string_;
};