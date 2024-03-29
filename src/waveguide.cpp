#include "waveguide.h"

#include <cmath>

namespace sfdsp
{

Waveguide::Waveguide(size_t max_size, InterpolationType interpolation_type)
    : max_size_(max_size), right_traveling_line_(max_size, false, interpolation_type),
      left_traveling_line_(max_size, true, interpolation_type)
{
    SetDelay(static_cast<float>(max_size - 1));
}

void Waveguide::SetDelay(float delay)
{
    if ((delay + 1) > static_cast<float>(max_size_))
    {
        delay = static_cast<float>(max_size_) - 1.f;
    }

    right_traveling_line_.SetDelay(delay);
    left_traveling_line_.SetDelay(delay);
    current_delay_ = delay;
}

float Waveguide::GetDelay() const
{
    return current_delay_;
}

void Waveguide::NextOut(float& right, float& left)
{
    right = right_traveling_line_.NextOut();
    left = left_traveling_line_.NextOut();
}

void Waveguide::Tick(float right, float left)
{
    right_traveling_line_.Tick(right);
    left_traveling_line_.Tick(left);
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
    float right = 0.f;
    float left = 0.f;
    TapOut(delay, right, left);
    return right + left;
}

float Waveguide::TapOut(float delay, InterpolationStrategy* interpolation_strategy)
{
    float right = 0.f;
    float left = 0.f;
    TapOut(delay, right, left, interpolation_strategy);
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

void Waveguide::TapOut(float delay, float& right_out, float& left_out, InterpolationStrategy* interpolation_strategy)
{
    assert(delay < max_size_);
    if (delay >= current_delay_)
    {
        delay = current_delay_;
    }

    right_out = right_traveling_line_.TapOut(delay, interpolation_strategy);
    left_out = left_traveling_line_.TapOut(delay, interpolation_strategy);
}

const Delayline& Waveguide::operator[](size_t index) const
{
    if (index == 0)
    {
        return right_traveling_line_;
    }

    assert(index == 1);
    return left_traveling_line_;
}

Delayline& Waveguide::operator[](size_t index)
{
    if (index == 0)
    {
        return right_traveling_line_;
    }

    assert(index == 1);
    return left_traveling_line_;
}
} // namespace sfdsp