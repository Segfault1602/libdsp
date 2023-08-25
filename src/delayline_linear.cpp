#include "delayline.h"

namespace dsp
{
LinearDelayline::LinearDelayline(size_t max_size, bool reverse) : Delayline(max_size, reverse)
{
}

float LinearDelayline::NextOut()
{
    if (do_next_out_)
    {
        float a = line_[(write_ptr_ + delay_ + 1) % max_size_];
        float b = line_[(write_ptr_ + delay_1) % max_size_];
        next_out_ = a + (b - a) * frac_;
        do_next_out_ = false;
    }

    return next_out_;
}

float LinearDelayline::Tick(float input)
{
    last_out_ = NextOut();
    do_next_out_ = true;

    line_[write_ptr_] = input;
    write_ptr_ = (write_ptr_ - 1 + max_size_) % max_size_;

    return last_out_;
}

float LinearDelayline::LastOut() const
{
    return last_out_;
}

float LinearDelayline::TapOut(float delay) const
{
    if (delay >= static_cast<float>(delay_))
    {
        delay = static_cast<float>(delay_);
    }

    if (reverse_)
    {
        delay = static_cast<float>(delay_) - delay - 1;
    }

    uint32_t delay_integer = static_cast<uint32_t>(delay);
    float frac = delay - static_cast<float>(delay_integer);

    float a = line_[(write_ptr_ + delay_integer + 1) % max_size_];
    float b = line_[(write_ptr_ + delay_integer + 2) % max_size_];

    return a + (b - a) * frac;
}

void LinearDelayline::TapIn(float delay, float input)
{
    if (reverse_)
    {
        delay = static_cast<float>(delay_) - delay - 1;
    }

    uint32_t delay_integer = static_cast<uint32_t>(delay);
    float frac = delay - static_cast<float>(delay_integer);

    line_[(write_ptr_ + delay_integer + 1) % max_size_] += input * (1.f - frac);
    line_[(write_ptr_ + delay_integer + 2) % max_size_] += input * frac;
}

void LinearDelayline::SetIn(float delay, float input)
{
    if (reverse_)
    {
        delay = static_cast<float>(delay_) - delay - 1;
    }

    uint32_t delay_integer = static_cast<uint32_t>(delay);
    float frac = delay - static_cast<float>(delay_integer);

    line_[(write_ptr_ + delay_integer + 1) % max_size_] = input * (1.f - frac);
    line_[(write_ptr_ + delay_integer + 2) % max_size_] = input * frac;
}
} // namespace dsp