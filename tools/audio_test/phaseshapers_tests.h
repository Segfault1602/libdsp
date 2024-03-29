#pragma once

#include "basic_oscillators.h"
#include "dsp_utils.h"
#include "phaseshapers.h"
#include "vector_phaseshaper.h"

#include "dsp_tester.h"

class PhaseShaperTest : public DspTester
{
  public:
    PhaseShaperTest() = default;
    ~PhaseShaperTest() override = default;

    void Init(size_t samplerate) override;
    float Tick() override;
    float Tick(float input) override;

  private:
    sfdsp::Phaseshaper phaseshaper_;
    uint32_t current_frame_ = 0;
    uint32_t frame_per_wave_ = 0;
    float current_waveform_ = static_cast<float>(sfdsp::Phaseshaper::Waveform::VARIABLE_SLOPE);
};

class PhaseShaperBlockTest : public DspTester
{
  public:
    PhaseShaperBlockTest() = default;
    ~PhaseShaperBlockTest() override = default;

    void Init(size_t samplerate) override;
    float Tick() override;
    float Tick(float input) override;

  private:
    sfdsp::Phaseshaper phaseshaper_;
    uint32_t current_frame_ = 0;
    uint32_t frame_per_wave_ = 0;
    float current_waveform_ = static_cast<float>(sfdsp::Phaseshaper::Waveform::VARIABLE_SLOPE);
};

class VectorPhaseshaperTest : public DspTester
{
  public:
    VectorPhaseshaperTest() = default;
    ~VectorPhaseshaperTest() override = default;

    void Init(size_t samplerate) override;
    float Tick() override;
    float Tick(float input) override;

  private:
    sfdsp::VectorPhaseshaper phaseshaper_;

    sfdsp::BasicOscillator lfo_;
};
