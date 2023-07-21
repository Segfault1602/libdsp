#pragma once

#include <cstddef>

#include "delayline_linear.h"
#include "dsp_base.h"
#include "junction.h"

namespace dsp
{

/// @brief Implements a waveguide with a maximum delay of MAX_SIZE
/// @tparam MAX_SIZE The maximum delay in samples
template <size_t MAX_SIZE>
class Waveguide
{
  public:
    Waveguide();
    ~Waveguide() = default;

    void SetDelay(float delay);

    float GetDelay() const;

    void SetJunction(float pos);

    void NextOut(float& right, float& left);

    void Tick(float right, float left);

    void TapIn(float delay, float input);
    void TapIn(float delay, float right, float left);

    float TapOut(float delay);

    void TapOut(float delay, float& right_out, float& left_out);

  private:
    float current_delay_ = MAX_SIZE;
    LinearDelayline<MAX_SIZE> right_traveling_line_;
    LinearDelayline<MAX_SIZE> left_traveling_line_;

    Junction junction_;
};

template <size_t MAX_SIZE>
Waveguide<MAX_SIZE>::Waveguide() : right_traveling_line_(false), left_traveling_line_(true)
{
    SetDelay(MAX_SIZE - 1);
    SetJunction(0);
}

template <size_t MAX_SIZE>
void Waveguide<MAX_SIZE>::SetDelay(float delay)
{
    if ((delay + 1) > MAX_SIZE)
    {
        delay = MAX_SIZE - 1;
    }

    right_traveling_line_.SetDelay(delay);
    left_traveling_line_.SetDelay(delay);
    current_delay_ = delay;
}

template <size_t MAX_SIZE>
float Waveguide<MAX_SIZE>::GetDelay() const
{
    return current_delay_;
}

template <size_t MAX_SIZE>
void Waveguide<MAX_SIZE>::SetJunction(float pos)
{
    junction_.SetDelay(pos);
}

template <size_t MAX_SIZE>
void Waveguide<MAX_SIZE>::NextOut(float& right, float& left)
{
    right = right_traveling_line_.NextOut();
    left = left_traveling_line_.NextOut();
}

template <size_t MAX_SIZE>
void Waveguide<MAX_SIZE>::Tick(float right, float left)
{
    junction_.Tick(left_traveling_line_, right_traveling_line_);
    right_traveling_line_.Tick(left);
    left_traveling_line_.Tick(right);
}

template <size_t MAX_SIZE>
void Waveguide<MAX_SIZE>::TapIn(float delay, float input)
{
    assert(delay < MAX_SIZE);
    if (delay >= current_delay_)
    {
        delay = current_delay_;
    }

    right_traveling_line_.TapIn(delay, input);
    left_traveling_line_.TapIn(delay, input);
}

template <size_t MAX_SIZE>
void Waveguide<MAX_SIZE>::TapIn(float delay, float right, float left)
{
    assert(delay < MAX_SIZE);
    if (delay >= current_delay_)
    {
        delay = current_delay_;
    }

    right_traveling_line_.TapIn(delay, right);
    left_traveling_line_.TapIn(delay, left);
}

template <size_t MAX_SIZE>
float Waveguide<MAX_SIZE>::TapOut(float delay)
{
    float right, left;
    TapOut(delay, right, left);
    return right + left;
}

template <size_t MAX_SIZE>
void Waveguide<MAX_SIZE>::TapOut(float delay, float& right_out, float& left_out)
{
    assert(delay < MAX_SIZE);
    if (delay >= current_delay_)
    {
        delay = current_delay_;
    }

    right_out = right_traveling_line_.TapOut(delay);
    left_out = left_traveling_line_.TapOut(delay);
}
} // namespace dsp