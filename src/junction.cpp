#include "junction.h"

#include <cmath>

namespace dsp
{
void Junction::SetDelay(float delay)
{
    // There is a minimum delay of 2 samples to perform the linear interpolation
    if (delay >= 2)
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

    // The following is based on the following paper:
    // Karjalainen, M., & Laine, U. K. (1991). A model for real-time sound synthesis of guitar on a floating-point
    // signal processor. [Proceedings] ICASSP 91: 1991 International Conference on Acoustics, Speech, and Signal
    // Processing. doi:10.1109/icassp.1991.151066 

    float x = delay_ - std::floor(delay_);
    float n = std::floor(delay_);

    float delayline_size = left_traveling_line.GetDelay();

    if (x < 0.5f)
    {
        float read_ptr = n + 2 * x;
        float sample = left_traveling_line.TapOut(delayline_size - read_ptr + 1);
        sample *= gain_;

        right_traveling_line.TapIn(delayline_size - n, sample);

        // Assume full reflection at the junction
        left_traveling_line.SetIn(delayline_size - std::floor(read_ptr) - 1, 0.f);
    }
    else
    {
        float read_ptr = n + 1;
        float sample = left_traveling_line.TapOut(delayline_size - read_ptr);
        sample *= gain_;

        float write_ptr = n + 2 * (x - 0.5f);
        right_traveling_line.TapIn(delayline_size - write_ptr - 1, sample);

        // Assume full reflection at the junction
        left_traveling_line.SetIn(delayline_size - std::floor(read_ptr) - 1, 0.f);
    }
}
} // namespace dsp