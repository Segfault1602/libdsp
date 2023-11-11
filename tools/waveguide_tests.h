#pragma once

#include "dsp_tester.h"
#include "waveguide.h"
#include "waveguide_gate.h"

class WaveguideTest : public DspTester
{
  public:
    WaveguideTest();
    ~WaveguideTest() override = default;

    void Init(size_t samplerate) override;
    float Tick() override;
    float Tick(float input) override;

  private:
    sfdsp::Waveguide waveguide_;
    sfdsp::WaveguideGate gate_;
    float gate_pos_ = 5.f;
    float gate_dt_ = 0.f;

    float phase_dt_ = 0.f;
    float phase_ = 0.f;
};