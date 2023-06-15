#pragma once

#include "dsp_base.h"

#include "delayline.h"
#include <cstdio>

namespace dsp
{
/// @brief Implements a junction point between two delaylines
class Junction
{
  public:
    Junction() = default;
    ~Junction() = default;

    void SetDelay(DspFloat delay)
    {
        delay_ = std::round(delay);
    }

    void Tick(Delayline& left_traveling_line, Delayline& right_traveling_line)
    {
        if (delay_ == 0 || delay_ == left_traveling_line.GetDelay())
        {
            return;
        }

        // Assuming a junction point of 3 before DelayLine::Tick() is called:
        //    ▶  1  2  a | b  5  6   ▼
        // Left          |         Right
        //    ▲  1  2  c | d  5  6   ◀
        // d needs to move to a and a is move to d

        DspFloat d_ptr = left_traveling_line.GetDelay() - delay_ - 1;
        DspFloat d = left_traveling_line.TapOut(d_ptr);
        DspFloat a = right_traveling_line.TapOut(delay_ - 1);
        // printf("Taking %f and moving it to %f\n", d, a);
        right_traveling_line.TapIn(delay_ - 1, d * gain_ - a);

        // Do the other side of the loop
        left_traveling_line.TapIn(d_ptr, a * gain_ - d);
    }

  private:
    DspFloat delay_ = 0;
    DspFloat gain_ = -1.f;
};
} // namespace dsp