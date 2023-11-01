#pragma once

#include "dsp_base.h"

#include "delayline.h"

namespace dsp
{

class WaveguideGates
{
  public:
    WaveguideGates();
    ~WaveguideGates() = default;

    void SetDelay(float delay);
    float GetDelay() const;

    void SetCoeff(float c);

    void Process(Delayline& left_traveling_line, Delayline& right_traveling_line);

  private:
    float coeff_ = 0.f;
    float delay_ = 0.f;
    size_t delay_integer_ = 0;
    float delay_fractional_ = 0.f;
    float inv_delay_fractional_ = 0.f;
    Delayline delay_left_;
    Delayline delay_right_;
};
} // namespace dsp