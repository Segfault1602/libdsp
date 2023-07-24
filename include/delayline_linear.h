#ifndef STK_DELAYL_H
#define STK_DELAYL_H

#include <cassert>
#include <cmath>
#include <cstdint>

#include "delayline.h"
#include "dsp_base.h"

namespace dsp
{

/// @brief Implements a linear delayline with a maximum delay of MAX_DELAY
/// @tparam MAX_DELAY The maximum delay in samples
template <size_t MAX_DELAY>
class LinearDelayline : public Delayline
{
  public:
    LinearDelayline(bool reverse = false);
    ~LinearDelayline() override = default;

    float NextOut() override;

    float Tick(float input) override;

    float LastOut() const override;

    float TapOut(float delay) const override;

    void TapIn(float delay, float input) override;
    void SetIn(float delay, float input) override;

  private:
    bool do_next_out_ = true;
    float next_out_ = 0.f;
    float last_out_ = 0.f;

    std::array<float, MAX_DELAY> line_;
};

template <size_t MAX_DELAY>
LinearDelayline<MAX_DELAY>::LinearDelayline(bool reverse) : Delayline(MAX_DELAY, reverse)
{
    line_.fill(0.f);
}

template <size_t MAX_DELAY>
float LinearDelayline<MAX_DELAY>::NextOut()
{
    if (do_next_out_)
    {
        float a = line_[(write_ptr_ + delay_) % MAX_DELAY];
        float b = line_[(write_ptr_ + delay_ + 1) % MAX_DELAY];
        next_out_ = a + (b - a) * frac_;
        do_next_out_ = false;
    }

    return next_out_;
}

template <size_t MAX_DELAY>
float LinearDelayline<MAX_DELAY>::Tick(float input)
{
    last_out_ = NextOut();
    do_next_out_ = true;

    line_[write_ptr_] = input;
    write_ptr_ = (write_ptr_ - 1 + MAX_DELAY) % MAX_DELAY;

    return last_out_;
}

template <size_t MAX_DELAY>
float LinearDelayline<MAX_DELAY>::LastOut() const
{
    return last_out_;
}

template <size_t MAX_DELAY>
float LinearDelayline<MAX_DELAY>::TapOut(float delay) const
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

    float a = line_[(write_ptr_ + delay_integer + 1) % MAX_DELAY];
    float b = line_[(write_ptr_ + delay_integer + 2) % MAX_DELAY];

    return a + (b - a) * frac;
}

template <size_t MAX_DELAY>
void LinearDelayline<MAX_DELAY>::TapIn(float delay, float input)
{
    if (reverse_)
    {
        delay = static_cast<float>(delay_) - delay - 1;
    }

    uint32_t delay_integer = static_cast<uint32_t>(delay);
    float frac = delay - static_cast<float>(delay_integer);

    line_[(write_ptr_ + delay_integer + 1) % MAX_DELAY] += input * (1.f - frac);
    line_[(write_ptr_ + delay_integer + 2) % MAX_DELAY] += input * frac;
}

template <size_t MAX_DELAY>
void LinearDelayline<MAX_DELAY>::SetIn(float delay, float input)
{
    if (reverse_)
    {
        delay = static_cast<float>(delay_) - delay - 1;
    }

    uint32_t delay_integer = static_cast<uint32_t>(delay);
    float frac = delay - static_cast<float>(delay_integer);

    line_[(write_ptr_ + delay_integer + 1) % MAX_DELAY] = input * (1.f - frac);
    line_[(write_ptr_ + delay_integer + 2) % MAX_DELAY] = input * frac;
}

} // namespace dsp

#endif