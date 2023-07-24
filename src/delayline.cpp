#include "delayline.h"

#include <cstdint>

namespace dsp
{
Delayline::Delayline(size_t max_delay, bool reverse) : reverse_(reverse), max_delay_(max_delay)
{
    SetDelay(static_cast<float>(max_delay_));
}

void Delayline::SetDelay(float delay)
{
    if (delay >= static_cast<const float>(max_delay_))
    {
        delay = static_cast<float>(max_delay_ - 1);
    }

    uint32_t delay_integer = static_cast<uint32_t>(delay);
    frac_ = delay - static_cast<float>(delay_integer);
    delay_ = delay_integer;
}

float Delayline::GetDelay() const
{
    return static_cast<const float>(delay_) + frac_;
}
} // namespace dsp