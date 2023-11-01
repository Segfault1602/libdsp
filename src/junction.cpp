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

float Junction::GetDelay() const
{
    return delay_;
}

#if 0
void Junction::Tick(Delayline& left_traveling_line, Delayline& right_traveling_line)
{
    if (delay_ == 0 || delay_ == left_traveling_line.GetDelay())
    {
        return;
    }

    // full reflection at the junction
    constexpr float k = -1.f;

    // Linear interpolator does not have any state so it can be reused for everything.
    LinearInterpolation interpolator;

    float right_sample = right_traveling_line.TapOut(delay_, &interpolator);
    float left_sample = left_traveling_line.TapOut(delay_ + 1, &interpolator);

    float scattering_into_left = right_sample * k + left_sample * (1.f - k);
    float scattering_into_right = right_sample * (1 + k) + left_sample * (-k);

    right_traveling_line.SetIn(std::floor(delay_), scattering_into_right);
    left_traveling_line.SetIn(std::floor(delay_) - 1, scattering_into_left);
}
#endif

void Junction::Tick(Delayline& left_traveling_line, Delayline& right_traveling_line)
{
    if (delay_ == 0 || delay_ == left_traveling_line.GetDelay())
    {
        return;
    }

    // full reflection at the junction
    constexpr float k = -1.f;

    // The following is based on the following paper:
    // Karjalainen, M., & Laine, U. K. (1991). A model for real-time sound synthesis of guitar on a floating-point
    // signal processor. [Proceedings] ICASSP 91: 1991 International Conference on Acoustics, Speech, and Signal
    // Processing. doi:10.1109/icassp.1991.151066 

    float x = delay_ - std::floor(delay_);
    float n = std::floor(delay_);

    if (x < 0.5f)
    {
        float read_ptr = n + 2 * x;
        float sample = right_traveling_line.TapOut(read_ptr);
        sample *= k;

        left_traveling_line.TapIn(n + 1, sample);

        // Assume full reflection at the junction
        right_traveling_line.SetIn(read_ptr + 1, 0.f);
    }
    else
    {
        float read_ptr = n + 1;
        float sample = right_traveling_line.TapOut(read_ptr);
        sample *= k;

        float write_ptr = n + 2 * (x - 0.5f);
        left_traveling_line.TapIn(write_ptr + 1, sample);

        // Assume full reflection at the junction
        right_traveling_line.SetIn(read_ptr + 1, 0.f);
    }
}
} // namespace dsp