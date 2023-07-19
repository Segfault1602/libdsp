#include "junction.h"

#include <cmath>

namespace dsp
{
void Junction::SetDelay(float delay)
{
    if (delay >= 0)
    {
        delay_ = delay;
    }
    else
    {
        delay_ = 0;
    }
}

void Junction::Tick(Delayline& left_traveling_line, Delayline& right_traveling_line)
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

    float d_ptr = left_traveling_line.GetDelay() - delay_ - 1;
    float d = left_traveling_line.TapOut(d_ptr);
    float a = right_traveling_line.TapOut(delay_ - 1);
    right_traveling_line.TapIn(std::ceil(delay_ - 1), d * gain_ - a);

    // Do the other side of the loop
    left_traveling_line.TapIn(std::ceil(d_ptr), -d);
}
} // namespace dsp