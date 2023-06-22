#pragma once

#include <cstddef>

#include "delayline_linear.h"
#include "dsp_base.h"
#include "junction.h"
#include "termination.h"


namespace dsp
{

/// @brief Implements a waveguide with a maximum delay of MAX_SIZE
/// @tparam MAX_SIZE The maximum delay in samples
template <size_t MAX_SIZE>
class Waveguide
{
  public:
    Waveguide()
    {
        SetDelay(MAX_SIZE - 1);
        SetJunction(0);
    }

    void SetDelay(DspFloat delay)
    {
        if ((delay + 1) > MAX_SIZE)
        {
            delay = MAX_SIZE - 1;
        }

        right_traveling_line_.SetDelay(delay);
        left_traveling_line_.SetDelay(delay);
        current_delay_ = delay;
    }

    DspFloat GetDelay() const
    {
        return current_delay_;
    }

    void SetJunction(DspFloat pos)
    {
        junction_.SetDelay(pos);
    }

    void Tick()
    {
        DspFloat right = right_traveling_line_.NextOut();
        DspFloat left = left_traveling_line_.NextOut();
        right = RightTermination.Tick(right);
        left = LeftTermination.Tick(left);
        right_traveling_line_.Tick(left);
        left_traveling_line_.Tick(right);
    }

    void TapIn(DspFloat delay, DspFloat input)
    {
        assert(delay < MAX_SIZE);
        if (delay >= current_delay_)
        {
            delay = current_delay_;
        }

        right_traveling_line_.TapIn(delay, input);
        left_traveling_line_.TapIn(current_delay_ - delay - 1, input);
    }

    DspFloat TapOut(DspFloat delay)
    {
        DspFloat right, left;
        TapOut(delay, right, left);
        return right + left;
    }

    void TapOut(DspFloat delay, DspFloat& right_out, DspFloat& left_out)
    {
        assert(delay < MAX_SIZE);
        if (delay >= current_delay_)
        {
            delay = current_delay_;
        }

        right_out = right_traveling_line_.TapOut(delay);
        left_out = left_traveling_line_.TapOut(current_delay_ - delay - 1);
    }

    Termination LeftTermination;
    Termination RightTermination;

  private:
    DspFloat current_delay_ = MAX_SIZE;
    LinearDelayline<MAX_SIZE> right_traveling_line_;
    LinearDelayline<MAX_SIZE> left_traveling_line_;

    Junction junction_;
};
} // namespace dsp