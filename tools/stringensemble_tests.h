#pragma once

#include "dsp_tester.h"

#include "string_ensemble.h"

class StringEnsembleTest : public DspTester
{
  public:
    StringEnsembleTest() = default;
    ~StringEnsembleTest() override = default;

    void Init(size_t samplerate) override;
    float Tick() override;
    float Tick(float input) override;

  private:
    dsp::StringEnsemble ensemble_;
};