#pragma once

#include "dsp_base.h"

#include "delayline.h"

namespace dsp
{
class Junction
{
  public:
    Junction() = default;
    ~Junction() = default;

    void SetDelay(DspFloat delay)
    {
        delay_ = delay;
    }

    void Tick(Delayline& left_traveling_line, Delayline& right_traveling_line)
    {
        DspFloat tap = left_traveling_line.TapOut(left_traveling_line.GetDelay() - delay_);
        DspFloat prev = right_traveling_line.TapOut(delay_ - 1);
        right_traveling_line.TapIn(delay_ - 1, tap - prev);
    }

    private:
        DspFloat delay_ = 0;
};
} // namespace dsp