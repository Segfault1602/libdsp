#include "delayline.h"

#include <algorithm>
#include <cstdint>

namespace dsp
{
Delayline::Delayline(size_t max_size, bool reverse) : max_size_(max_size), reverse_(reverse)
{
    line_ = std::make_unique<float[]>(max_size_);
    interpolation_strategy_ = std::make_unique<LinearInterpolation>();
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
    last_out_ = NextOut();
    do_next_out_ = true;

    line_[write_ptr_] = input;
    write_ptr_ = (write_ptr_ - 1 + max_size_) % max_size_;

    return last_out_;
}

float Delayline::TapOut(float delay) const
{
    if (delay >= delay_)
    {
        delay = delay_;
    }

    if (reverse_)
    {
        delay = delay_ - delay + 1;
    }

    return interpolation_strategy_->TapOut(line_.get(), max_size_, write_ptr_, delay);
}

void Delayline::TapIn(float delay, float input)
{
    if (reverse_)
    {
        delay = delay_ - delay+1;
    }

    interpolation_strategy_->TapIn(line_.get(), max_size_, write_ptr_, delay, input);
}

void Delayline::SetIn(float delay, float input)
{
    if (reverse_)
    {
        delay = delay_ - delay - 1.f;
    }

    uint32_t delay_integer = static_cast<uint32_t>(delay);
    float frac = delay - static_cast<float>(delay_integer);

    line_[(write_ptr_ + delay_integer + 1) % max_size_] = input * (1.f - frac);
    line_[(write_ptr_ + delay_integer + 2) % max_size_] = input * frac;
}

} // namespace dsp