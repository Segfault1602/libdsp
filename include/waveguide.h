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

    /// @brief Sets the gain of the right termination. The gain for the left termination is always -1.
    /// @param gain
    void SetGain(DspFloat gain)
    {
        right_termination_.SetGain(gain);
    }

    void SetJunction(DspFloat pos)
    {
        junction_.SetDelay(pos);
    }

    void Tick()
    {
        // The order here is important
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

  private:
    DspFloat current_delay_ = MAX_SIZE;
    LinearDelayline<MAX_SIZE> right_traveling_line_;
    LinearDelayline<MAX_SIZE> left_traveling_line_;

    LeftTermination left_termination_;
    RightTermination right_termination_;
    Junction junction_;
};
} // namespace dsp