#pragma once

#include <cstddef>

#include "delayline_linear.h"
#include "dsp_base.h"
#include "termination.h"
#include "junction.h"

namespace dsp
{

template <size_t MAX_SIZE>
class Waveguide
{
  public:
    Waveguide() = default;

    void SetDelay(DspFloat delay)
    {
        // right_traveling_line_.SetDelay(delay);
        // left_traveling_line_.SetDelay(delay);
        junction_.SetDelay(delay);
        current_delay_ = delay;
    }

    void Tick()
    {
        junction_.Tick(left_traveling_line_, right_traveling_line_);
        left_termination_.Tick(left_traveling_line_, right_traveling_line_);
        right_termination_.Tick(left_traveling_line_, right_traveling_line_);
    }

    void TapIn(DspFloat delay, DspFloat input)
    {
        assert(delay < MAX_SIZE);
        if (delay >= current_delay_)
        {
            delay = current_delay_;
        }

        right_traveling_line_.TapIn(delay, input);
        left_traveling_line_.TapIn(current_delay_ - delay, input);
    }

    DspFloat TapOut(DspFloat delay)
    {
        assert(delay < MAX_SIZE);
        if (delay >= current_delay_)
        {
            delay = current_delay_;
        }

        return right_traveling_line_.TapOut(delay) + left_traveling_line_.TapOut(current_delay_ - delay);
    }

  private:
    DspFloat current_delay_ = MAX_SIZE;
    LinearDelayline<MAX_SIZE> right_traveling_line_;
    LinearDelayline<MAX_SIZE> left_traveling_line_;

    LeftTermination left_termination_;
    RightTermination right_termination_;
    Junction junction_;
};
} // namespace dsp