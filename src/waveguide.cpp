#include "waveguide.h"

#include <cmath>

namespace dsp
{

Waveguide::Waveguide(size_t max_size, InterpolationType interpolation_type)
    : max_size_(max_size), right_traveling_line_(max_size, false, interpolation_type),
      left_traveling_line_(max_size, true, interpolation_type)
{
    SetDelay(static_cast<float>(max_size - 1));
    SetJunction(0);
}

void Waveguide::SetDelay(float delay)
{
    if ((delay + 1) > static_cast<float>(max_size_))
    {
        delay = static_cast<float>(max_size_) - 1.f;
    }

    // Consolidate the fractional part of the delay into the right traveling line
    uint32_t delay_integer = static_cast<uint32_t>(delay);
    float frac = delay - delay_integer;

    right_traveling_line_.SetDelay(delay_integer + 2.f * frac);
    left_traveling_line_.SetDelay(delay_integer);
    current_delay_ = delay;
}

float Waveguide::GetDelay() const
{
    return current_delay_;
}

void Waveguide::SetJunction(float pos)
{
    junction_.SetDelay(pos);
}

void Waveguide::NextOut(float& right, float& left)
{
    right = right_traveling_line_.NextOut();
    left = left_traveling_line_.NextOut();
}

void Waveguide::Tick(float right, float left)
{
    junction_.Tick(left_traveling_line_, right_traveling_line_);
    right_traveling_line_.Tick(left);
    left_traveling_line_.Tick(right);
}

void Waveguide::TapIn(float delay, float input)
{
    assert(delay < max_size_);
    if (delay >= current_delay_)
    {
        delay = current_delay_;
    }

    right_traveling_line_.TapIn(delay, input);
    left_traveling_line_.TapIn(delay, input);
}

void Waveguide::TapIn(float delay, float right, float left)
{
    assert(delay < max_size_);
    if (delay >= current_delay_)
    {
        delay = current_delay_;
    }

    right_traveling_line_.TapIn(delay, right);
    left_traveling_line_.TapIn(delay, left);
}

float Waveguide::TapOut(float delay)
{
    float right, left;
    TapOut(delay, right, left);
    return right + left;
}

void Waveguide::TapOut(float delay, float& right_out, float& left_out)
{
    assert(delay < max_size_);
    if (delay >= current_delay_)
    {
        delay = current_delay_;
    }

    right_out = right_traveling_line_.TapOut(delay);
    left_out = left_traveling_line_.TapOut(delay);
}

} // namespace dsp