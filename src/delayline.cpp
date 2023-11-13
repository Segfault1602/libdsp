#include "delayline.h"

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace sfdsp
{
Delayline::Delayline(size_t max_size, bool reverse, InterpolationType interpolation_type)
    : max_size_(max_size), reverse_(reverse)
{
    switch (interpolation_type)
    {
    case InterpolationType::None:
        interpolation_strategy_ = std::make_unique<NoInterpolation>();
        break;
    case InterpolationType::Linear:
        interpolation_strategy_ = std::make_unique<LinearInterpolation>();
        break;
    case InterpolationType::Allpass:
        interpolation_strategy_ = std::make_unique<AllpassInterpolation>();
        break;
    default:
        assert(false);
    }

    line_ = std::make_unique<float[]>(max_size_);
    std::fill(line_.get(), line_.get() + max_size_, 0.f);
    SetDelay(static_cast<float>(max_size_));
}

void Delayline::SetDelay(float delay)
{
    if (delay >= static_cast<const float>(max_size_))
    {
        delay = static_cast<float>(max_size_ - 1);
    }

    delay_ = delay;
}

float Delayline::GetDelay() const
{
    return delay_;
}

void Delayline::Reset()
{
    std::fill(line_.get(), line_.get() + max_size_, 0.f);
}

float Delayline::NextOut()
{
    if (do_next_out_)
    {
        next_out_ = interpolation_strategy_->TapOut(line_.get(), max_size_, write_ptr_, delay_);
        do_next_out_ = false;
    }

    return next_out_;
}

float Delayline::LastOut() const
{
    return last_out_;
}

float Delayline::Tick(float input)
{
    line_[write_ptr_] = input;
    last_out_ = NextOut();
    do_next_out_ = true;
    write_ptr_ = (write_ptr_ - 1 + max_size_) % max_size_;

    return last_out_;
}

float Delayline::TapOut(float delay, InterpolationStrategy* interpolation_strategy)
{
    if (delay >= delay_)
    {
        delay = delay_;
    }

    if (reverse_)
    {
        delay = delay_ - delay + 1;
    }

    if (interpolation_strategy == nullptr)
    {
        LinearInterpolation interpolation;
        return interpolation.TapOut(line_.get(), max_size_, write_ptr_, delay);
    }

    return interpolation_strategy->TapOut(line_.get(), max_size_, write_ptr_, delay);
}

void Delayline::TapIn(float delay, float input)
{
    if (reverse_)
    {
        delay = std::floor(delay_) - delay + 1;
    }

    interpolation_strategy_->TapIn(line_.get(), max_size_, write_ptr_, delay, input);
}

void Delayline::SetIn(float delay, float input)
{
    if (reverse_)
    {
        delay = std::floor(delay_) - delay + 1.f;
    }

    auto delay_integer = static_cast<uint32_t>(delay);
    float frac = delay - static_cast<float>(delay_integer);

    line_[(write_ptr_ + delay_integer) % max_size_] = input * (1.f - frac);
    if (frac != 0.f)
    {
        line_[(write_ptr_ + delay_integer + 1) % max_size_] = input * frac;
    }
}

float& Delayline::operator[](size_t index) const
{
    if (reverse_)
    {
        index = static_cast<size_t>(delay_) - index + 1;
    }
    size_t read_ptr = (write_ptr_ + index) % max_size_;
    return line_[read_ptr];
}

float& Delayline::operator[](size_t index)
{
    if (reverse_)
    {
        index = static_cast<size_t>(delay_) - index + 1;
    }
    size_t read_ptr = (write_ptr_ + index) % max_size_;
    return line_[read_ptr];
}

} // namespace sfdsp