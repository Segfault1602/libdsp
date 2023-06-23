#include "delayline.h"

#include <cstdint>

namespace dsp
{
Delayline::Delayline(size_t max_delay) : max_delay_(max_delay)
{
    SetDelay(static_cast<float>(max_delay_));
}

void Delayline::SetDelay(float delay)
{
    if (delay >= max_delay_)
    {
        delay = static_cast<float>(max_delay_ - 1);
    }

    int32_t delay_integer = static_cast<uint32_t>(delay);
    frac_ = delay - static_cast<float>(delay_integer);
    delay_ = delay_integer;
}

float Delayline::GetDelay() const
{
    return delay_ + frac_;
}
} // namespace dsp