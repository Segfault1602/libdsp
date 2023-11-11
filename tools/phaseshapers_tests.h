#pragma once

#include "dsp_base.h"
#include "phaseshapers.h"

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
    size_t current_frame_ = 0;
    size_t frame_per_wave_ = 0;
    float current_waveform_ = static_cast<float>(sfdsp::Phaseshaper::Waveform::VARIABLE_SLOPE);
};